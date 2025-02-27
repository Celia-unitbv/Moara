#ifndef LOGGER_H
#define LOGGER_H

#include "EMessageType.h"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>


class Logger {
public:
	static Logger& instance(const std::string& filename);
	void log(const std::string& message,EMessageType mType);

private:
	Logger(const std::string& filename);
	~Logger();
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	std::ofstream logFile;
	std::mutex logMutex;

	std::string currentDateTime();
};

#endif // LOGGER_H
