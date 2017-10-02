#include "Includes.hpp"

#pragma once

class SyntacticAnalyser
{
private:
	LexicalAnalyser *m_LexicalAnalyser;
	LexicalRegister *m_CurrentRegister;
	LexicalRegister *m_PreviousRegister;
	int m_BeginsFound;
	bool m_Error;

	RegisterType ExtractType(Token token);

	void MatchToken(Token expectedToken);

	void MatchToken(TokenType expectedType);

	void Start();

	void Constant();

	void Declaration(RegisterType type);

	bool Attribution(Symbol *symbol);

	void Command();

	void While();

	void If();

	void ReadLn();

	void Write();

	Function *Expression();

	Function *SubExpression();

	Function *Term();

	Function *Factor();

public:

	SyntacticAnalyser();

	void RunAnalysis();
};