#pragma once
#include <string>
#include <regex>
#include "TokenType.h"

class Tokenizer
{
public:
    static TokenType ClassifyLexeme(const std::string& lexeme);
};

std::string TokenTypeToString(TokenType type);