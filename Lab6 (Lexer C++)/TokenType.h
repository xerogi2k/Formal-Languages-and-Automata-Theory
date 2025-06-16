#pragma once
#include <map>

enum class TokenType
{
    IDENTIFIER,
    KEYWORD,
    SEPARATOR,
    MATH_OPERATION,
    LOGICAL_OPERATOR,
    CHAR,
    STRING,
    INTEGER,           
    DOUBLE,            
    FLOAT,             
    BINARY,            
    OCTAL,             
    HEXADECIMAL,       
    BOOLEAN,        
    BRACKETS,       
    COMMENTS,
    END_OF_FILE,
    ERROR,
};
