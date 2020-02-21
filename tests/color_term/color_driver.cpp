#include"color_term.h"

#include<iostream>

int main(void) {
 
  if (isTerminalStream(std::cout)) {
    std::cout << "I'm a terminal\n";
    if (supportsColor()) {
      std::cout << "I support color\n";
    }
  } else {
    std::cout << "Not a terminal\n";
  }
  return 0; 
}
