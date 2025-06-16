#include "Lexer.h"
#include "Tokenizer.h"
#include "Logger.h"
#include <unordered_set>
#include <unordered_map>

namespace
{
    const std::unordered_set<char> IGNORED_CHARS = { ' ', '\t', '\n' };
    const std::unordered_set<char> SEPARATORS = { ' ', '\t', '\n', ';', ',', '{', '}', '(', ')', '[', ']', '=', '<', '>', '!', '/', '*', '+', '-', '"' };

    const std::unordered_map<std::string, bool> COMPOUND_SEPARATORS = {
        {"==", true},
        {"<>", true},
        {"<=", true},
        {">=", true},
        {"!=", true},
        {"&&", true},
        {"||", true}
    };
}

Lexer::Lexer(std::istream& strm)
    : m_strm(strm), m_currentLine(1), m_currentPos(0)
{
    m_strm >> std::noskipws;
}

Lexeme Lexer::GetLexeme()
{
    std::string lexeme;
    size_t lexemeStartLine = 0;
    size_t lexemeStartPos = 1;

    while (true)
    {
        try
        {
            SkipIgnored();

            lexemeStartLine = m_currentLine;
            lexemeStartPos += m_currentPos;

            lexeme = GetLexemeImpl();

            if (m_strm.eof() && lexeme.empty())
            {
                return { TokenType::END_OF_FILE, "", m_currentLine, m_currentPos };
            }

            if (!lexeme.empty())
            {
                break;
            }
        }
        catch (const std::exception& ex)
        {
            Logger::LogError("Exception in Lexer::GetLexeme: " + std::string(ex.what()));
            return { TokenType::ERROR, "", m_currentLine, m_currentPos };
        }
    }

    return { Tokenizer::ClassifyLexeme(lexeme), lexeme, lexemeStartLine, lexemeStartPos };
}

std::string Lexer::GetLexemeImpl()
{
    SkipIgnored();

    if (m_strm.eof())
    {
        return "";
    }

    char ch;
    std::string lexeme;

    while (!m_strm.eof() && GetNextChar(ch))
    {
        if ((ch >= 'А' && ch <= 'я') || ch == 'Ё' || ch == 'ё') 
        {
            Logger::LogError("Only English language supported : " + std::string(1, ch));
            std::exit(EXIT_FAILURE);
        }

        if (SEPARATORS.count(ch))
        {
            if (!lexeme.empty())
            {
                PutCharBack();
                break;
            }

            lexeme += ch;

            char nextChar;
            if (GetNextChar(nextChar))
            {
                std::string potentialCompound = lexeme + nextChar;
                if (COMPOUND_SEPARATORS.count(potentialCompound))
                {
                    lexeme = potentialCompound;
                }
                else
                {
                    PutCharBack();
                }
            }

            if (lexeme == "\"")
            {
                return ProcessString();
            }

            return lexeme;
        }

        lexeme += ch;

        if (std::isdigit(ch) || ch == '.' || ch == 'e' || ch == 'E' || ch == '+' || ch == '-')
        {
            bool hasDecimalPoint = (ch == '.');
            bool hasExponent = (ch == 'e' || ch == 'E');
            bool hasSignAfterExponent = false;

            while (!m_strm.eof() && m_strm.peek() != '\n')
            {
                char next = m_strm.peek();
                if (std::isdigit(next))
                {
                    GetNextChar(next);
                    lexeme += next;
                }
                else if (next == '.' && !hasDecimalPoint && !hasExponent)
                {
                    GetNextChar(next);
                    lexeme += next;
                    hasDecimalPoint = true;
                }
                else if ((next == 'e' || next == 'E') && !hasExponent)
                {
                    GetNextChar(next);
                    lexeme += next;
                    hasExponent = true;
                }
                else if ((next == '+' || next == '-') && hasExponent && !hasSignAfterExponent)
                {
                    GetNextChar(next);
                    lexeme += next;
                    hasSignAfterExponent = true;
                }
                else
                {
                    break;
                }
            }
        }
    }

    return lexeme;
}


void Lexer::SkipIgnored()
{
    char ch;
    while (!m_strm.eof() && GetNextChar(ch))
    {
        if (IGNORED_CHARS.count(ch))
        {
            continue;
        }

        if (ch == '/' && m_strm.peek() == '/')
        {
            while (!m_strm.eof() && GetNextChar(ch) && ch != '\n');
            continue;
        }

        if (ch == '/' && m_strm.peek() == '*')
        {
            std::string comment = "/*";
            GetNextChar(ch);

            while (!m_strm.eof())
            {
                GetNextChar(ch);
                comment += ch;

                if (ch == '*' && m_strm.peek() == '/')
                {
                    GetNextChar(ch);
                    comment += ch;
                    break;
                }
            }

            if (m_strm.eof() && comment.substr(comment.size() - 2) != "*/")
            {
                throw std::runtime_error("Unterminated comment detected: " + comment);
            }

            continue;
        }

        PutCharBack();
        break;
    }
}


std::string Lexer::ProcessString()
{
    std::string lexeme = "\"";
    char ch;
    while (!m_strm.eof() && GetNextChar(ch))
    {
        if (ch == '"')
        {
            lexeme += ch;
            return lexeme;
        }
        lexeme += ch;
    }

    Logger::LogError("Unterminated string found.");
    return lexeme;
}

std::istream& Lexer::GetNextChar(char& ch)
{
    if (m_strm.get(ch)) // Используем `get` вместо `>>`, чтобы избежать проблем с пропуском символов
    {
        if (ch == '\n')
        {
            ++m_currentLine;
            m_currentPos = 0; // Обнуляем позицию при новой строке
        }
        else
        {
            ++m_currentPos; // Увеличиваем позицию только если это не новая строка
        }
    }
    return m_strm;
}

void Lexer::PutCharBack()
{
    if (m_strm.unget()) // Проверяем, успешно ли `unget`
    {
        if (m_currentPos == 0 && m_currentLine > 1) // Если в начале строки, нужно вернуть на предыдущую строку
        {
            --m_currentLine;
            m_currentPos = 0; // Здесь, возможно, придется рассчитать длину предыдущей строки (если это нужно)
        }
        else if (m_currentPos > 0)
        {
            --m_currentPos;
        }
    }
}


void Lexer::UpdateCurrentLine(char ch)
{
    if (ch == '\n')
    {
        ++m_currentLine;
        m_currentPos = 0;
    }
}
