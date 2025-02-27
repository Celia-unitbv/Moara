#include "logger.h"
#include <iomanip>

Logger::Logger(const std::string& filename) 
{
	logFile.open(filename, std::ios_base::app);
	if (!logFile.is_open()) {
		std::cerr << "Failed to open log file." << std::endl;
	}
}

Logger::~Logger() {
	if (logFile.is_open()) {
		logFile.close();
	}
}

Logger& Logger::instance(const std::string& filename) {
	static Logger instance (filename);
	return instance;
}

void Logger::log(const std::string& message, EMessageType mType) {
	std::string type;
	switch (mType)
	{
	case EMessageType::Error:
		type = "Error";
		break;
	case EMessageType::Warning:
		type = "Warning";

		break;
	case EMessageType::Info:
		type = "Info";
	break;
	
	}
	std::lock_guard<std::mutex> guard(logMutex);
	logFile << currentDateTime() << " - " <<type<<" - "<< message << std::endl;
	logFile.flush();
}

std::string Logger::currentDateTime() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm buf;
	localtime_s(&buf, &in_time_t);  
	std::stringstream ss;
	ss << std::put_time(&buf, "%Y-%m-%d %X");
	return ss.str();
}
