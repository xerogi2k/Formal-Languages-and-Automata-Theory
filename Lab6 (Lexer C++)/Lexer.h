#pragma once
#include <istream>
#include "Lexeme.h"
#include "Tokenizer.h"

class Lexer
{
public:
    explicit Lexer(std::istream& strm);
    Lexeme GetLexeme();

private:
    std::string GetLexemeImpl();
    void SkipIgnored();
    std::string ProcessString();
    std::istream& GetNextChar(char& ch);
    void PutCharBack();
    void UpdateCurrentLine(char ch);

    std::istream& m_strm;
    size_t m_currentLine = 1;
    size_t m_currentPos = 0;
};
