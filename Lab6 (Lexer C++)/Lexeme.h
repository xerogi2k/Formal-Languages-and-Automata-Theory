#pragma once
#include <string>
#include "TokenType.h"

struct Lexeme
{
    TokenType type;
    std::string lexeme;
    size_t lineNum;
    size_t linePos;
};
