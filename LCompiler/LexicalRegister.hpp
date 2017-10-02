#pragma once

#include "Includes.hpp"

class LexicalRegister
{
public:

	Token m_Token;
	std::string m_Lexeme;
	TokenType m_Type;
	int m_LineNumber;
	RegisterType m_RegisterType;
	IdClass m_ClassId;

	LexicalRegister(Token token, std::string Lexeme, int lineNumber);
};