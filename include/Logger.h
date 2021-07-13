#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H
//////////////////////////////////////////////////
// 
// This class is used to log application events
// for the purpose of debugging and generating
// a results document. Debug level can be set
// by changing the initialize function in main()
// to a log level code defined in defines.h.
// 
//                  Created by Chris Foytek - 2016
//////////////////////////////////////////////////
//Includes
#include "defines.h"
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

class Logger {
private:
  Logger() {}
  Logger(Logger const &);
  ~Logger() {}
  void operator=(Logger const &);

  std::string generateLogFileName();
  std::string fmtTime(double time);
  int LOG_LEVEL;
  std::string logName;
  std::string logFolder;
  std::string resultsFileName;
public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }
  void intializeLogger(int logLevel, std::string topologyFile);
  void Log(int logLevel, double time, std::string message);
  void Log(int logLevel, std::string message);
  static std::string packetCodeToString(int packetCode);
  static std::string eventCodeToString(int eventCode);
};
#endif // !DEBUG_LOGGER_H
