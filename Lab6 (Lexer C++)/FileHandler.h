#pragma once
#include <fstream>
#include <string>

class FileHandler
{
public:
    FileHandler(const std::string& inputFileName, const std::string& outputFileName);
    std::ifstream& GetInputStream();
    std::ofstream& GetOutputStream();
    ~FileHandler();

private:
    std::ifstream m_input;
    std::ofstream m_output;
};
