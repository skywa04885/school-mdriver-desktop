#ifndef _LIB_LOGGER_H
#define _LIB_LOGGER_H

#include "default.h"

typedef enum : uint8_t {
  DEBUG = 0,
  PARSER,
  INFO,
  WARN,
  ERROR,
  FATAL
} LoggerLevel;

typedef enum : uint8_t {
  ENDL = 0,
  CLASSIC,
  FLUSH
} LoggerOpts;

class Logger {
public:
  Logger(const std::string &l_Prefix, LoggerLevel l_Level):
    l_Prefix(l_Prefix), l_Level(l_Level)
  {}

  static void saveToDisk(const std::string &message);

  template<typename T>
  Logger &append(const T &a) {
    this->l_Stream << a;
    return *this;
  }

  Logger &append(const LoggerLevel& a) {
    this->l_Old = this->l_Level;
    this->l_Level = a;

    return *this;
  }

  Logger &append(const LoggerOpts &a) {
    switch (a)
    {
      case LoggerOpts::CLASSIC:
        this->l_Level = this->l_Old;
        break;
      case LoggerOpts::ENDL:
      case LoggerOpts::FLUSH: {
        std::ostringstream oss;

        char dateBuffer[64];
        time_t rawTime;
        struct tm *timeInfo = nullptr;

        time(&rawTime);
        timeInfo = localtime(&rawTime);
        strftime(dateBuffer, sizeof(dateBuffer), "%a, %d %b %Y %T", timeInfo);
        std::cout << dateBuffer << "->";

        // Appends the level, and checks if the message should be logged to
        //  the file, this only happens on an error.

        switch (this->l_Level) {
          case LoggerLevel::DEBUG:
            oss << "\033[36m(DEBUG@" << this->l_Prefix << "): \033[0m";
            break;
          case LoggerLevel::PARSER:
            oss << "\033[34m(PARSER@" << this->l_Prefix << "): \033[0m";
            break;
          case LoggerLevel::INFO:
            oss << "\033[32m(INFO@" << this->l_Prefix << "): \033[0m";
            break;
          case LoggerLevel::WARN:
            oss << "\033[33m(WARN@" << this->l_Prefix << "): \033[0m";
            break;
          case LoggerLevel::ERROR:
            oss << "\033[31m(ERR@" << this->l_Prefix << "): \033[0m";
            break;
          case LoggerLevel::FATAL:
            oss << "\033[31m[FATAL@" << this->l_Prefix << "]: \033[0m";
            break;
        }



        oss << this->l_Stream.str();

        if (a == LoggerOpts::FLUSH)
          std::cout << oss.str() << std::flush;
        else
          std::cout << oss.str() << std::endl;

        this->l_Stream.str("");
        this->l_Stream.clear();
        break;
      }
    }

    return *this;
  }

  template<typename T>
  Logger &operator << (const T &a) {
    return this->append(a);
  }
private:
  std::ostringstream l_Stream;
  std::string l_Prefix;
  LoggerLevel l_Level, l_Old;
};

#endif