#include "Logger.h"

void Logger::LogError(const std::string& message)
{
    std::cerr << "[ERROR]: " << message << std::endl;
}

void Logger::LogInfo(const std::string& message)
{
    std::cout << "[INFO]: " << message << std::endl;
}
