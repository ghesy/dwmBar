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

/* C++ modules for the status bar (implementation) */
/* Implementation of classes that provide output useful for display in the status bar. */

#include <cstddef>
#include <cstdio>
#include <sys/statvfs.h>
#include <ios>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "modules.hpp"

using std::string;
using std::stringstream;
using std::fstream;
using std::ios;
using std::mutex;
using std::unique_lock;
using std::chrono::seconds;

using namespace DWMBspace;

void Module::operator()() const {
    if (refreshInterval_) { // if not zero, do a time-lapse loop
        mutex mtx;
        while (true) {
            runModule_();
            unique_lock<mutex> lk(mtx);
            signalCondition_->wait_for(lk, seconds(refreshInterval_));
        }
    } else { // wait for a real-time signal
        runModule_();
        mutex mtx;
        while (true) {
            unique_lock<mutex> lk(mtx);
            signalCondition_->wait(lk);
            runModule_();
            lk.unlock();
        }
    }
}

/* static member */
const size_t ModuleExtern::lengthLimit_ = 500;

void ModuleExtern::runModule_() const {
    char buffer[100];
    string output;
    FILE *pipe = popen(extCommand_.c_str(), "r");
    if (!pipe) { // fail silently
        return;
    }
    while ( !feof(pipe) ) {
        if (fgets(buffer, 100, pipe) != NULL) {
            output += buffer;
        }
        if (output.size() > lengthLimit_) {
            output.erase( output.begin()+lengthLimit_, output.end() );
            break;
        }
    }
    pclose(pipe);
    mutex mtx;
    unique_lock<mutex> lk(mtx);
    *outString_ = output;
    outputCondition_->notify_one();
    lk.unlock();
}
