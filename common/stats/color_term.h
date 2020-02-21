#ifndef __COLOR_TERM_H__
#define __COLOR_TERM_H__ // prevent against double-inclusion

#include<iostream>
#include<cstdlib>
#include<cstring>
extern "C" {
#include<unistd.h>
}

#define C_RESET      "\033[0m"
#define C_MAGENTA    "\033[35m"
#define C_BOLD_RED   "\033[1m\033[31m"
#define C_BOLD_GREEN "\033[1m\033[32m"
#define C_BOLD_BLUE  "\033[1m\033[34m"

bool isTerminalStream(std::ostream& os);
// Returns true of os is a stream to a terminal
// (as opposed to a file redirection).

bool supportsColor();
// Returns true if the current terminal supports color output.

#endif
