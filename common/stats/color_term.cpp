#include"color_term.h"

bool isTerminalStream(std::ostream& os) {
  if ( (&os == &std::cout && isatty( STDOUT_FILENO ))
        || (&os == &std::cerr && isatty( STDERR_FILENO ))
        || (&os == &std::clog && isatty( STDERR_FILENO )) ) {
    //  is a terminal...
    return true;
  }
  return false;
}

bool supportsColor() {
  if (const char *env_p = std::getenv("TERM")) {
    const char *const terms[] = {
       "xterm", "xterm-256", "xterm-256color", "vt100",
       "color", "ansi", "cygwin", "linux"};
    for (auto const term: terms) {
      if (std::strcmp(env_p, term) == 0) return true;
    }
  }
  return false;
}
