#include "logger.h"

LogLevel Logger::priority = (LogLevel)LOG_INFO;
OutputType Logger::outType = (OutputType)LOG_CONSOLE;

std::string Logger::filePath = LOG_DEFAULT_FILE;
std::ofstream Logger::fileStream;

std::mutex Logger::mut;