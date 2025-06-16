#include "FileHandler.h"
#include <stdexcept>

FileHandler::FileHandler(const std::string& inputFileName, const std::string& outputFileName)
    : m_input(inputFileName), m_output(outputFileName)
{
    if (!m_input.is_open() || !m_output.is_open())
    {
        throw std::runtime_error("Failed to open input or output file.");
    }
}

std::ifstream& FileHandler::GetInputStream()
{
    return m_input;
}

std::ofstream& FileHandler::GetOutputStream()
{
    return m_output;
}

FileHandler::~FileHandler()
{
    if (m_input.is_open())
    {
        m_input.close();
    }
    if (m_output.is_open())
    {
        m_output.close();
    }
}
