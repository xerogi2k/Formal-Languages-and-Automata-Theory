#pragma once
#include <iostream>
#include <string>

class Logger
{
public:
    static void LogError(const std::string& message);
    static void LogInfo(const std::string& message);
};
