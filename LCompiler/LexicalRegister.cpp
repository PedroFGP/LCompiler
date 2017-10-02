#include "Includes.hpp"

LexicalRegister::LexicalRegister(Token token, std::string lexeme, int lineNumber)
{
	m_Token = token;
	m_Lexeme = lexeme;
	m_LineNumber = lineNumber;
	m_Type = TokenToType(token);

	std::unordered_map<std::string, std::unique_ptr<Symbol>>::iterator it = Symbols::SymbolsTable.find(m_Lexeme);

	if (it != Symbols::SymbolsTable.end())
	{
		m_Type = TokenToType(it->second->m_Token);
		m_Token = it->second->m_Token;
		m_RegisterType = it->second->m_Type;
		m_ClassId = it->second->m_Class;
	}
	else
	{
		m_RegisterType = REGISTER_TYPE_NA;
		m_ClassId = ID_CLASS_NA;
	}
}