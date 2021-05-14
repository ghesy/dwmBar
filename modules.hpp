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

/* C++ modules for the status bar (definitions) */
/* definitions of classes that provide output useful for display in the status bar. */

#ifndef modules_hpp
#define modules_hpp

#include <cstddef>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

using std::string;
using std::condition_variable;

namespace DWMBspace {

    /* base module class */
    /* establishes the common parameters for all modules. Modules are functors that write output to a `string` variable. */
    class Module {
    public:
        /* destructor */
        virtual ~Module(){ outString_ = nullptr; outputCondition_ = nullptr; };

        /* run the module */
        /* runs the module, refreshing at the specified interval or after receiving a refresh signal. */
        void operator()() const;

    protected:
        /** default constructor */
        Module() : refreshInterval_{0}, outString_{nullptr}, outputCondition_{nullptr}, signalCondition_{nullptr} {};

        /*
          constructor
          interval: refresh time interval in seconds
          output: pointer to the output storing string
          cVar: pointer to the condition variable for change signaling
          sigVar: pointer to the condition variable to monitor real-time signals
       */
        Module(const uint32_t &interval, string *output, condition_variable *cVar, condition_variable *sigVar) : refreshInterval_{interval}, outString_{output}, outputCondition_{cVar}, signalCondition_{sigVar} {};

        /** refresh interval in seconds */
        uint32_t refreshInterval_;

        /** pointer to the `string` that receives output */
        string *outString_;

        /* pointer to a condition variable to signal change in state
         * the module is using this to communicate to the main thread. */
        condition_variable *outputCondition_;

        /* pointer to a condition variable to accept signal events
         * the module is waiting for this if it relies on a real-time signal to refresh. */
        condition_variable *signalCondition_;

        /* run the module once
         * retrieves the data specific to the module and formats the output. */
        virtual void runModule_() const = 0;
    };

    /* external scripts
     * runs an external script or shell command and displays the output.
     * no formatting of the external output is performed, but it is truncated to 500 characters. */
    class ModuleExtern final : public Module {
    public:
        /* Default constructor */
        ModuleExtern() : Module() {};
        /*
          constructor
          interval: refresh time interval in seconds
          command: external command
          output: pointer to the output storing string
          cVar: pointer to the condition variable for change signaling
          sigVar: pointer to the condition variable to monitor real-time signals
        */
        ModuleExtern(const uint32_t &interval, const string &command, string *output, condition_variable *cVar, condition_variable *sigVar) : Module(interval, output, cVar, sigVar), extCommand_{command} {};

        /* destructor */
        ~ModuleExtern() {};
    protected:
        /* output length limit */
        static const size_t lengthLimit_;

        /* external command string */
        const string extCommand_;

        /* run the module once
         * runs the external shell command or script and returns the output, truncating to 500. */
        void runModule_() const override;
    };
}

#endif
