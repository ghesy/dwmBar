/*
 * Copyright (c) 2020 Anthony J. Greenberg
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * author Anthony J. Greenberg
 * copyright Copyright (c) 2020 Anthony J. Greenberg,
 * (c) 2020-2021 Ehsan Ghorbannezhad (https://github.com/soystemd) <ehsangn@protonmail.ch>
 * version 0.9
 */

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <bits/stdint-intn.h>
#include <csignal>
#include <cstddef>
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "xrdb.h"
#include "modules.hpp"
#include "config.hpp"
#include "xrdb.c"

using std::string;
using std::stoi;
using std::vector;
using std::thread;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using std::chrono::seconds;
using std::cerr;

using namespace DWMBspace;

/* number of possible real-time signals */
static const int sigRTNUM = 30;
/* condition variables that will respond to real-time signals */
static vector<condition_variable> signalCondition(sigRTNUM);

/*
 * make bar output
 * takes individual module outputs and puts them together for printing.
 * moduleOutput: vector of individual module outputs
 * delimiter: delimiter character(s) between modules
 * barText: compiled text to be printed to the bar
 */
void makeBarOutput(const vector<string> &moduleOutput, const string &delimiter, string &barText){
    barText.clear();
    for (auto moIt = moduleOutput.begin(); moIt != (moduleOutput.end() - 1); ++moIt){
        barText += (*moIt) + delimiter;
    }
    barText += moduleOutput.back();
}

/*
 * render the bar
 * renders the bar text by printing the provided string to the root window.
 * This is how dwm handles status bars.
 * barOutput: text to be displayed
 */
void printRoot(const string &barOutput){
    Display *d = XOpenDisplay(NULL);
    if (d == nullptr) {
        return;         // fail silently
    }
    const int32_t screen = DefaultScreen(d);
    const Window root    = RootWindow(d, screen);
    XStoreName( d, root, barOutput.c_str() );
    XCloseDisplay(d);
}

/*
 * process real-time signals
 * receive and process real-time signals to trigger relevant modules.
 * sig: signal number (starting at `SIGRTMIN`)
 */
void processSignal(int sig){
    /* do nothing silently if wrong signal received */
    if ( (sig < SIGRTMIN) || (sig > SIGRTMAX) ) {
        return;
    }
    size_t sigInd = sig - SIGRTMIN;
    signalCondition[sigInd].notify_one();
}

int main(){

    for (int sigID = SIGRTMIN; sigID <= SIGRTMAX; sigID++) {
        signal(sigID, processSignal);
    }

    XrmInitialize();
    xrdb_read();

    static const std::string col_dim(dim);
    static const std::string col_bright(bright);

    /* the stuff enclosed in ^ characters are for the status2d patch */
    static const std::string topDelimiter = "^v^^c" + col_dim + "^" + delim + "^t^ ";

    /* string that comes before the first module */
    static const std::string beginDelimiter = "^c" + col_bright + "^^v^^c" + col_dim + "^" + delimBegin + "^t^ ";

    /* string that comes after the last module */
    static const std::string endDelimiter = "^c" + col_dim + "^" + delimEnd;

    mutex mtx;
    condition_variable commonCond; /* this triggers printing to the bar from individual modules */
    vector<string> topModuleOutputs( topModuleList.size() );
    vector<thread> moduleThreads;
    size_t moduleID = 0;

    for (auto &tb : topModuleList){

        if (tb.size() != 3) {
            cerr << "ERROR: top bar module description vector must have exactly three elements, yours has " << tb.size() << " (module " << tb[0] << ")\n";
            exit(1);
        }

        int32_t interval = stoi(tb[1]);
        if (interval < 0) {
            cerr << "ERROR: refresh interval cannot be negative, yours is " << interval << " (module " << tb[0] << ")\n";
            exit(2);
        }

        int32_t rtSig = stoi(tb[2]);
        if (rtSig < 0) {
            cerr << "ERROR: real-time signal cannot be negative, yours is " << rtSig << " (module " << tb[0] << ")\n";
            exit(3);
        }

        moduleThreads.push_back(thread{ModuleExtern(interval, tb[0], &topModuleOutputs[moduleID], &commonCond, &signalCondition[rtSig])});
        moduleID++;
    }

    vector<string> bottomModuleOutputs;
    if (twoBars) {
        bottomModuleOutputs.resize( bottomModuleList.size() );
        moduleID = 0;

        for (auto &bb : bottomModuleList){

            if (bb.size() != 3) {
                cerr << "ERROR: top bar module description vector must be have exactly four elements, yours has " << bb.size() << " (module " << bb[0] << ")\n";
                exit(1);
            }

            int32_t interval = stoi(bb[1]);
            if (interval < 0) {
                cerr << "ERROR: refresh interval cannot be negative, yours is " << interval << " (module " << bb[0] << ")\n";
                exit(2);
            }

            int32_t rtSig = stoi(bb[2]);
            if (rtSig < 0) {
                cerr << "ERROR: real-time signal cannot be negative, yours is " << rtSig << " (module " << bb[0] << ")\n";
                exit(3);
            }

            moduleThreads.push_back(thread{ModuleExtern(interval, bb[0], &bottomModuleOutputs[moduleID], &commonCond, &signalCondition[rtSig])});
            moduleID++;
        }
    }

    string barTextBottom;
    string barText;

    while (true) {

        unique_lock<mutex> lk(mtx);
        commonCond.wait(lk);
        makeBarOutput(topModuleOutputs, topDelimiter, barText);
        barText = beginDelimiter + barText + endDelimiter;

        if (twoBars) {
            makeBarOutput(bottomModuleOutputs, bottomDelimiter, barTextBottom);
            barText = barText + botTopDelimiter + beginDelimiterBottom + barTextBottom + endDelimiterBottom;
        }
        lk.unlock();

        /* replace newline characters with a space */
        std::regex newlines_regex("\n+");
        barText = std::regex_replace(barText,newlines_regex," ");

        printRoot(barText);
    }

    for (auto &t : moduleThreads){
        if ( t.joinable() ) {
            t.join();
        }
    }
    exit(0);
}
