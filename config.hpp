#ifndef config_hpp
#define config_hpp

#include <string>
#include <vector>

/** \brief Top delimiter
 *
 * Delimiter that goes between modules on the top bar
 */
static const std::string topDelimiter("^v^^c#555555^][^t^ ");
static const std::string beginDelimiter("^c#888888^^v^^#c#555555^[^t^ ");
static const std::string endDelimiter("^c#555555^]");

/** \brief Bottom delimiter
 *
 * Delimiter that goes between modules on the bottom bar
 */
static const std::string bottomDelimiter(" | ");

/** \brief Are there two bars?
 *
 * If there is only one bar, the top bar information is used regardless of where you put your bar.
 */
static const bool twoBars = false;

/** \brief Delimiter between top and bottom
 *
 * The delimiter used by the dwm-extrabar patch.
 */
static const std::string botTopDelimiter(";");

/** List of top modules
 *
 * Names of modules for the top bar.
 * The necessary module information is:
 * - module name (one of the provided internal objects, the path to the relevant script, or a shell command)
 * - internal/external keyword
 * - refresh interval (in seconds; 0 means update only on receiving a real-time signal)
 * - `SIGRTMIN` signal ID, must be between 0 and 30.
 *   If the refresh interval is not zero, a real-time signal ca still be used to trigger the module before the interval expires.
 */
static const std::vector< std::vector<std::string> > topModuleList = {
	{"dwmbar-net",       "external",  "8",   "4"},
	{"dwmbar-tor",       "external",  "9",   "3"},
	{"dwmbar-mem",       "external",  "3",   "2"},
	{"dwmbar-audio",     "external", "30",   "5"},
	{"dwmbar-date",      "external", "30",   "1"},
};

/** List of bottom modules
 *
 * Names of modules for the bottom bar.
 * See the top bar info for instructions.
 */
static const std::vector< std::vector<std::string> > bottomModuleList = {
	{"dwmbar-date", "external", "60",  "1"},
};

/** \brief Date format for the internal date/time module */
static const std::string dateFormat("%a %b %e %H:%M %Z");

/** \brief List of file systems to monitor
 *
 * File systems to monitor for available space using the built-in disk space module.
 */
static const std::vector<std::string> fsNames{"/home", "/home/tonyg/extra"};

#endif // config_hpp
