#include "Logger.h"

std::string Logger::generateLogFileName() {
  std::ostringstream stringBuilder;
  std::time_t time = std::time(0);
  struct tm* dateTime = gmtime(&time);
  stringBuilder << "log_" << (dateTime->tm_year + 1900) << "_"
                << (dateTime->tm_mon + 1) << "_" << dateTime->tm_mday << "_"
                << dateTime->tm_hour << dateTime->tm_min << dateTime->tm_sec
                << ".txt";
  return stringBuilder.str();
}

std::string Logger::fmtTime(double time) {
  std::ostringstream sb;
  sb << std::setprecision(11) << "[t=" << time << "]: ";
  return sb.str();
}

void Logger::intializeLogger(int logLevel, std::string topologyFile) {
  LOG_LEVEL = logLevel;
  logName = generateLogFileName();
  if (LOG_LEVEL == LOG_RESULTS) {
    std::ofstream logOut;
    logOut.open("Results.txt", std::fstream::out | std::fstream::app);
    logOut << "\n\n------Results for " << topologyFile << "------\n\n";
    logOut.close();
  }
}

void Logger::Log(int logLevel, double time, std::string message) {
  if (LOG_LEVEL >= logLevel) {
    std::ofstream logOut;
    if (logLevel == LOG_RESULTS) {
      // Print message to the results file
      logOut.open("Results.txt", std::fstream::out | std::fstream::app);
      logOut << fmtTime(time) << message << std::endl;
      logOut.close();
    } else {
      logOut.open(logName.c_str(), std::fstream::out | std::fstream::app);
      logOut << fmtTime(time) << message << std::endl;
      logOut.close();
    }
  }
}

void Logger::Log(int logLevel, std::string message) {
  if (LOG_LEVEL >= logLevel) {
    std::ofstream logOut;
    if (logLevel == LOG_RESULTS) {
      // Print the message to the results file
      logOut.open("Results.txt", std::fstream::out | std::fstream::app);
      logOut << message << std::endl;
      logOut.close();
    } else {
      logOut.open(logName.c_str(), std::fstream::out | std::fstream::app);
      logOut << message << std::endl;
      logOut.close();
    }
  }
}

std::string Logger::packetCodeToString(int packetCode) {
  switch (packetCode) {
    case DVPACKET:
      return "DVPACKET";
    case DATAPACKET:
      return "DATAPACKET";
  }
  return "Packet code doesn't exist.";
}
std::string Logger::eventCodeToString(int eventCode) {
  switch (eventCode) {
    case SENDDV:
      return "SENDDV";
    case RECVDV:
      return "RECVDV";
    case SENDTRIGDV:
      return "SENDTRIGDV";
    case RMLINKBTWNNODES:
      return "RMLINKBTWNNODES";
    case FWDDATAPKT:
      return "FWDDATAPKT";
  }
  return "Event code doesn't exist.";
}