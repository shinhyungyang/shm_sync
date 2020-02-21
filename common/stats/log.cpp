#include"log.h"
#include"globals.h"

#include<mutex>
#include<iostream>
#include<fstream>
#include<sstream>

static std::mutex mtx_outfile;

void logStatus(const char *file, int line, const std::string & msg) {
  mtx_outfile.lock();
  std::ostringstream msgstrm;
  msgstrm << "STATUS: " << msg << ", file " << file;
  msgstrm << ", line # " << line << std::endl;
  std::cerr << msgstrm.str();
  std::cerr.flush();
  mtx_outfile.unlock();
}

void logStatus(const char *file, int line, const std::string & msg, int err) {
  std::ostringstream msgstrm;
  msgstrm << msg << ", error: " << err << std::endl;
  logStatus(file, line, msgstrm.str());
}

void logSampleStatus(const char * str, long long s, int tid, int exec, int run,
                     const char * file, int line)
{
  std::ostringstream msgstrm;
  msgstrm << str << " " << tid << ", sample: " << s << ", exec: "
          << exec << ", run: " << run;
  logStatus(file, line, msgstrm.str());
}

void logSimple(const char * str, int tid, const char * str1,
               const char * file, int line)
{
  std::ostringstream msgstrm;
  msgstrm << str << " " << tid << " " << str1;
  logStatus(file, line, msgstrm.str());
}
