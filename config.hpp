#ifndef config_hpp
#define config_hpp

#include <string>
#include <vector>

/* delimiter between modules */
static const std::string delim("][");
static const std::string delimBegin("[");
static const std::string delimEnd("]");

/* colors */
static char dim[] = "#444444";
static char bright[] = "#888888";

/* specify colors to read from xresources */
XCOLORS
    XLOAD( dim,    "*.color2" );
    XLOAD( bright, "*.color6" );
XCOLORS_END

/*
  list of modules
. module-name: is name or path to a script or program.
    can start with ~/... for stuff in your home directory.
. refresh-interval: in seconds, 0 means only update on receiving a real-time signal
. signal-id: SIGRTMIN signal id; must be between 0 and 30.*/

/* module-name, refresh-interval, signal-id */
static const std::vector< std::vector<std::string> > topModuleList = {
    {"dwmbar-net",       "8",   "4"},
    {"dwmbar-tor",       "9",   "3"},
    {"dwmbar-mem",       "3",   "2"},
    {"dwmbar-audio",     "0",   "5"},
#ifdef LAPTOP
    {"dwmbar-battery",  "60",  "15"},
#endif
    {"dwmbar-date",     "30",   "1"},
};

/* enable if you have the dwm-extrabar patch. */
static const bool twoBars = false;

/* the delimiter used by the dwm-extrabar patch. */
static const std::string botTopDelimiter(";");

/* list of modules for extra (bottom) bar */
static const std::vector< std::vector<std::string> > bottomModuleList = {};

#endif
