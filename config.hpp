#ifndef config_hpp
#define config_hpp

#include <string>
#include <vector>

/* delimiter between modules */
/* the stuff enclosed in ^ characters are for my status2d patch */
static const std::string topDelimiter("^v^^c#444444^][^t^ ");

/* string that comes before the first module */
static const std::string beginDelimiter("^c#888888^^v^^#c#444444^[^t^ ");

/* string that comes after the last module */
static const std::string endDelimiter("^c#444444^]");

/* enable if you have the dwm-extrabar patch. */
static const bool twoBars = false;

/* delimiter between modules on your extra (bottom) bar */
static const std::string bottomDelimiter(" | ");

/* strings that come before and after the extrabar */
static const std::string beginDelimiterBottom(" ");
static const std::string endDelimiterBottom(" ");

/* the delimiter used by the dwm-extrabar patch. */
static const std::string botTopDelimiter(";");

/*
  list of modules

. module-name: is name or path to a script or program.
    can start with ~/... for stuff in your home directory.
. internal/external: use internal if you want to use
. refresh-interval: in seconds, 0 means only update on receiving a real-time signal
. signal-id: SIGRTMIN signal id; must be between 0 and 30.*/

/*  module-name       refresh-interval   signal-id */
static const std::vector< std::vector<std::string> > topModuleList = {
    {"dwmbar-net",    "8",               "4"},
    {"dwmbar-tor",    "9",               "3"},
    {"dwmbar-mem",    "3",               "2"},
    {"dwmbar-audio",  "0",               "5"},
    {"dwmbar-date",   "30",              "1"},
};

/* list of modules for extra (bottom) bar */
static const std::vector< std::vector<std::string> > bottomModuleList = {};

#endif
