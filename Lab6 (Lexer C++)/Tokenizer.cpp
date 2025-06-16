#include "Tokenizer.h"
#include <unordered_set>
#include <regex>

namespace
{
    const std::unordered_set<std::string> KEYWORDS = {
        "void", "string", "double", "int", "bool",
        "while", "break", "continue", "if", "else", "return"
    };

    const std::unordered_set<std::string> MATH_OPERATIONS = {
        "+", "-", "*", "/", "="
    };

    const std::unordered_set<std::string> LOGICAL_OPERATORS = {
        "==", "!=", "&&", "||", "<", ">", ">=", "<=", "<>"
    };

    const std::unordered_set<std::string> SEPARATORS = {
        ";", ",", ":", "."
    };

    const std::unordered_set<std::string> BRACKETS = {
        "(", ")", "{", "}", "[", "]"
    };

    bool isMathOperation(const std::string& lexeme)
    {
        if ((lexeme[0] == '=' || lexeme[0] == '+') && lexeme[1] == '=')
            return false;
        return MATH_OPERATIONS.count(lexeme);
    }

    bool IsInteger(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"([-+]?\d+)"));
    }

    bool IsFloat(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"(^(?:\d+|\d+\.\d+|\.\d+)(?:[eE][+-]?\d+)?$)"));
    }


    bool IsDouble(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"(^\d*\.\d+$)"));
    }

    bool IsBinary(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"(0[bB][01]+)"));
    }

    bool IsOctal(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"(0[0-7]+)"));
    }

    bool IsHexadecimal(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"(0[xX][0-9a-fA-F]+)"));
    }

    bool IsIdentifier(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"([a-zA-Z_][a-zA-Z0-9_]*)"));
    }

    bool IsChar(const std::string& lexeme)
    {
        return std::regex_match(lexeme, std::regex(R"('\\?.')"));
    }

    bool IsComment(const std::string& lexeme)
    {
        if (std::regex_match(lexeme, std::regex(R"(//.*)")))
            return true;

        if (std::regex_match(lexeme, std::regex(R"(/\*.*\*/)")))
            return true;

        return false;
    }
}

TokenType Tokenizer::ClassifyLexeme(const std::string& lexeme)
{
    if (IsComment(lexeme)) return TokenType::COMMENTS;
    if (KEYWORDS.count(lexeme)) return TokenType::KEYWORD;

    if (lexeme.size() < 20 && IsOctal(lexeme)) return TokenType::OCTAL;
    if (lexeme.size() < 18 && IsDouble(lexeme)) return TokenType::DOUBLE;
    if (lexeme.size() < 24 && IsFloat(lexeme)) return TokenType::FLOAT;
    if (lexeme.size() < 12 && IsInteger(lexeme)) return TokenType::INTEGER;

    if (isMathOperation(lexeme)) return TokenType::MATH_OPERATION;
    if (LOGICAL_OPERATORS.count(lexeme)) return TokenType::LOGICAL_OPERATOR;
    if (SEPARATORS.count(lexeme)) return TokenType::SEPARATOR;
    if (BRACKETS.count(lexeme)) return TokenType::BRACKETS;
    if (lexeme.size() > 1 && lexeme.size() < 66 && lexeme.front() == '"' && lexeme.back() == '"') return TokenType::STRING;
    if (lexeme == "true" || lexeme == "false") return TokenType::BOOLEAN;
    if (lexeme.size() < 20 && IsBinary(lexeme)) return TokenType::BINARY;
    if (lexeme.size() < 20 && IsHexadecimal(lexeme)) return TokenType::HEXADECIMAL;
    if (lexeme.size() == 3 && IsChar(lexeme)) return TokenType::CHAR;
    if (lexeme.size() > 0 && lexeme.size() < 65 && IsIdentifier(lexeme)) return TokenType::IDENTIFIER;

    return TokenType::ERROR;
}

std::string TokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::KEYWORD:
        return "KEYWORD";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::SEPARATOR:
        return "SEPARATOR";
    case TokenType::BRACKETS:
        return "BRACKETS";
    case TokenType::MATH_OPERATION:
        return "MATH_OPERATION";
    case TokenType::LOGICAL_OPERATOR:
        return "LOGICAL_OPERATOR";
    case TokenType::STRING:
        return "STRING";
    case TokenType::INTEGER:
        return "INTEGER";
    case TokenType::FLOAT:
        return "FLOAT";
    case TokenType::DOUBLE:
        return "DOUBLE";
    case TokenType::BINARY:
        return "BINARY";
    case TokenType::OCTAL:
        return "OCTAL";
    case TokenType::HEXADECIMAL:
        return "HEXADECIMAL";
    case TokenType::BOOLEAN:
        return "BOOLEAN";
    case TokenType::CHAR:
        return "CHAR";
    case TokenType::COMMENTS:
        return "COMMENTS";
    case TokenType::END_OF_FILE:
        return "END_OF_FILE";
    case TokenType::ERROR:
    default:
        return "ERROR";
    }
}
