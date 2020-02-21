#ifndef __LOG_H__
#define __LOG_H__ // prevent against double-inclusion

#include<string>

void logStatus(const char *file, int line, const std::string & msg);
// Log a status message (thread-safe).

void logStatus(const char *file, int line, const std::string & msg, int err);
// Log a status message, including error-value (thread-safe).

#endif
