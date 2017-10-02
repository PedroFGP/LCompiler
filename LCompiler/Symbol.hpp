#pragma once

class Symbol
{
public:

	Token m_Token;
	std::string m_Lexeme;
	RegisterType m_Type;
	IdClass m_Class;
	int m_LineNumber;
	int m_MemoryAddress;

	Symbol(Token token, std::string lexeme);

	Symbol(Token token, std::string lexeme, RegisterType type, IdClass classe, int lineNumber);
};