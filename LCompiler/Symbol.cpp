#pragma once

#include "Includes.hpp"

Symbol::Symbol(Token token, std::string lexeme)
{
	this->m_Token = token;
	this->m_Lexeme = lexeme;
	this->m_Type = REGISTER_TYPE_NA;
	this->m_Class = ID_CLASS_NA;
	this->m_LineNumber = 0;
	this->m_MemoryAddress = 0;
}

Symbol::Symbol(Token token, std::string lexeme, RegisterType type, IdClass classe, int lineNumber)
{
	this->m_Token = token;
	this->m_Lexeme = lexeme;
	this->m_Type = type;
	this->m_Class = classe;
	this->m_LineNumber = lineNumber;
	this->m_MemoryAddress = 0;
}