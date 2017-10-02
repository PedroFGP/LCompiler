#include "Includes.hpp"

SyntacticAnalyser::SyntacticAnalyser()
{
	m_LexicalAnalyser = new LexicalAnalyser();

	m_CurrentRegister = m_LexicalAnalyser->RunAnalysis();

	if (m_LexicalAnalyser->m_Error)
	{
		m_Error = true;
	}

	m_BeginsFound = 0;
}

void SyntacticAnalyser::RunAnalysis()
{
	Start();
}

#pragma region Grammar Functions

RegisterType SyntacticAnalyser::ExtractType(Token token)
{
	RegisterType value = REGISTER_TYPE_NA;

	switch (token)
	{
	case TOKEN_INTEGER:
	case TOKEN_INTEGER_VALUE:
		value = REGISTER_TYPE_INTEGER;
		break;
	case TOKEN_STRING:
	case TOKEN_STRING_VALUE:
		value = REGISTER_TYPE_STRING;
		break;
	case TOKEN_BOOLEAN:
	case TOKEN_TRUE:
	case TOKEN_FALSE:
		value = REGISTER_TYPE_LOGICAL;
		break;
	case TOKEN_BYTE:
	case TOKEN_HEXADECIMAL_VALUE:
		value = REGISTER_TYPE_BYTE;
		break;
	}

	return value;
}

void SyntacticAnalyser::MatchToken(Token expectedToken)
{
	if (expectedToken != m_CurrentRegister->m_Token)
	{
		fprintf(stderr, "[Syntactic Analyser] - %d:token nao esperado %s.\r\n", m_CurrentRegister->m_LineNumber, m_CurrentRegister->m_Lexeme.c_str());

		//m_Error = true;

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}
	else
	{
		if (expectedToken == TOKEN_END)
		{
			if (m_BeginsFound == 0)
			{
				fprintf(stderr, "[Syntactic Analyser] - %d:fim de arquivo nao esperado.\r\n", m_CurrentRegister->m_LineNumber);

				//m_Error = true;

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			m_BeginsFound--;
		}

		if (expectedToken == TOKEN_BEGIN || expectedToken == TOKEN_MAIN)
		{
			m_BeginsFound++;
		}

		m_PreviousRegister = m_CurrentRegister;

		m_CurrentRegister = m_LexicalAnalyser->RunAnalysis();
	}
}

void SyntacticAnalyser::MatchToken(TokenType expectedType)
{
	if (expectedType != m_CurrentRegister->m_Type)
	{
		fprintf(stderr, "[Syntactic Analyser] - %d:token nao esperado %s.\r\n", m_CurrentRegister->m_LineNumber, m_CurrentRegister->m_Lexeme.c_str());

		//m_Error = true;

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}
	else
	{
		m_PreviousRegister = m_CurrentRegister;

		m_CurrentRegister = m_LexicalAnalyser->RunAnalysis();

		if (expectedType == TOKEN_TYPE_TYPE)
		{
			m_CurrentRegister->m_RegisterType = ExtractType(m_PreviousRegister->m_Token);
		}
	}
}

void SyntacticAnalyser::Start()
{
	CG::Initialize();

	while (m_CurrentRegister->m_Type == TOKEN_TYPE_TYPE)
	{
		MatchToken(TOKEN_TYPE_TYPE);

		if (m_PreviousRegister->m_Token == TOKEN_CONSTANT)
		{
			Constant();
		}
		else
		{
			Declaration(ExtractType(m_PreviousRegister->m_Token));
		}
	}

	CG::FinalizeDeclaration();

	MatchToken(TOKEN_MAIN);

	while (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
	{
		Command();
	}

	CG::EndProgram();

	MatchToken(TOKEN_END);
}

void SyntacticAnalyser::Constant()
{
	MatchToken(TOKEN_ID);

	Symbol *idSymbol = Symbols::SymbolsTable.at(m_PreviousRegister->m_Lexeme).get();

	if (idSymbol->m_Class == ID_CLASS_NA)
	{
		idSymbol->m_Class = ID_CLASS_CONST;
	}
	else
	{
		fprintf(stderr, "[Semantic Analyser] - %d:identificador já declarado %s.\r\n", m_CurrentRegister->m_LineNumber, idSymbol->m_Lexeme.c_str());

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}

	MatchToken(TOKEN_ATTRIBUTION);

	if (m_CurrentRegister->m_Token == TOKEN_MINUS)
	{
		MatchToken(TOKEN_MINUS);

		if (m_CurrentRegister->m_Token != TOKEN_INTEGER_VALUE)
		{
			fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}
	}

	RegisterType type = ExtractType(m_CurrentRegister->m_Token);

	if (type == REGISTER_TYPE_LOGICAL)
	{
		fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}

	idSymbol->m_Type = type;

	MatchToken(TOKEN_TYPE_VALUE);

	CG::addDeclaration(idSymbol, m_PreviousRegister->m_Lexeme);

	MatchToken(TOKEN_SEMICOLON);
}

void SyntacticAnalyser::Declaration(RegisterType type)
{
	MatchToken(TOKEN_ID);

	Symbol *idSymbol = Symbols::SymbolsTable.at(m_PreviousRegister->m_Lexeme).get();

	if (idSymbol->m_Class == ID_CLASS_NA)
	{
		idSymbol->m_Class = ID_CLASS_VAR;
		idSymbol->m_Type = type;
	}
	else
	{
		fprintf(stderr, "[Semantic Analyser] - %d:identificador já declarado %s.\r\n", m_CurrentRegister->m_LineNumber, idSymbol->m_Lexeme.c_str());

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}

	if (m_CurrentRegister->m_Token == TOKEN_ATTRIBUTION)
	{
		MatchToken(TOKEN_ATTRIBUTION);

		if (Attribution(idSymbol))
		{
			m_PreviousRegister->m_Lexeme = "-" + m_PreviousRegister->m_Lexeme;
		}

		CG::addDeclaration(idSymbol, m_PreviousRegister->m_Lexeme);
	}
	else
	{
		CG::addDeclaration(idSymbol);
	}

	while (m_CurrentRegister->m_Token == TOKEN_COMMA)
	{
		MatchToken(TOKEN_COMMA);
		MatchToken(TOKEN_ID);

		idSymbol = Symbols::SymbolsTable.at(m_PreviousRegister->m_Lexeme).get();

		if (idSymbol->m_Class == ID_CLASS_NA)
		{
			idSymbol->m_Class = ID_CLASS_VAR;
			idSymbol->m_Type = type;
		}
		else
		{
			fprintf(stderr, "[Semantic Analyser] - %d:identificador já declarado %s.\r\n", m_CurrentRegister->m_LineNumber, idSymbol->m_Lexeme.c_str());

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}

		if (m_CurrentRegister->m_Token == TOKEN_ATTRIBUTION)
		{
			MatchToken(TOKEN_ATTRIBUTION);

			if (Attribution(idSymbol))
			{
				m_PreviousRegister->m_Lexeme = "-" + m_PreviousRegister->m_Lexeme;
			}

			CG::addDeclaration(idSymbol, m_PreviousRegister->m_Lexeme);
		}
		else
		{
			CG::addDeclaration(idSymbol);
		}
	}

	MatchToken(TOKEN_SEMICOLON);
}

bool SyntacticAnalyser::Attribution(Symbol *symbol)
{
	bool value = false;

	if (m_CurrentRegister->m_Token == TOKEN_MINUS)
	{
		MatchToken(TOKEN_MINUS);

		value = true;

		if (m_CurrentRegister->m_Token != TOKEN_INTEGER_VALUE)
		{
			fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}
	}

	RegisterType currentType = ExtractType(m_CurrentRegister->m_Token);

	if (currentType != symbol->m_Type)
	{
		if (symbol->m_Type == REGISTER_TYPE_INTEGER)
		{
			if (currentType != REGISTER_TYPE_BYTE)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}
		}
		else
		{
			fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}
	}

	MatchToken(TOKEN_TYPE_VALUE);

	return value;
}

void SyntacticAnalyser::Command()
{
	switch (m_CurrentRegister->m_Token)
	{
	case TOKEN_WHILE:

		MatchToken(TOKEN_WHILE);

		While();

		break;

	case TOKEN_IF:

		MatchToken(TOKEN_IF);

		If();

		break;
	case TOKEN_READLN:

		MatchToken(TOKEN_READLN);

		ReadLn();

		break;
	case TOKEN_WRITE:

		MatchToken(TOKEN_WRITE);

		Write();

		break;
	case TOKEN_WRITELN:

		MatchToken(TOKEN_WRITELN);

		Write();

		break;
	case TOKEN_ID:

		MatchToken(TOKEN_ID);

		Symbol *id = Symbols::SymbolsTable.at(m_PreviousRegister->m_Lexeme).get();

		if (id->m_Class == ID_CLASS_NA)
		{
			fprintf(stderr, "[Semantic Analyser] - %d:identificador não declarado %s.\r\n", m_CurrentRegister->m_LineNumber, id->m_Lexeme.c_str());

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}

		MatchToken(TOKEN_ATTRIBUTION);

		CG::g_TemporaryVars = 0;

		Function *Exp = Expression();

		if (id->m_Type != Exp->m_Type)
		{
			fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}

		MatchToken(TOKEN_SEMICOLON);

		if (Exp->m_Type == REGISTER_TYPE_STRING)
		{
			std::string dollar = std::to_string((int)'$');

			std::string CopyStart = CG::NewLabel();
			std::string CopyEnd = CG::NewLabel();

			CG::addLine("\tmov al, DS:[" + Exp->m_Address + "]");
			CG::addLine("\tmov di, " + Exp->m_Address);
			CG::addLine("\tmov si, " + std::to_string(id->m_MemoryAddress));
			CG::addLine(CopyStart + ":");
			CG::addLine("\tcmp al, " + dollar);
			CG::addLine("\tje " + CopyEnd);
			CG::addLine("\tmov DS:[si], al");
			CG::addLine("\tmov dh, 0");
			CG::addLine("\tmov dl, al");
			CG::addLine("\tmov ah, 9");
			CG::addLine("\tint 33");
			CG::addLine("\tadd si, 1");
			CG::addLine("\tadd di, 1");
			CG::addLine("\tmov al, DS:[di]");
			CG::addLine("\tjmp " + CopyStart);
			CG::addLine(CopyEnd + ":");
			CG::addLine("\tmov al, " + dollar);
			CG::addLine("\tmov DS:[si], al");
		}
		else
		{
			std::string tempRegister = (Exp->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "bl";

			CG::addLine("\tmov " + tempRegister + ", DS:[" + Exp->m_Address + "]");
			CG::addLine("\tmov DS:[" + std::to_string(id->m_MemoryAddress) + "], " + tempRegister);
		}

		break;
	}
}

void SyntacticAnalyser::While()
{
	MatchToken(TOKEN_OPENING_PARENTHESIS);

	Function *Exp;

	CG::g_TemporaryVars = 0;

	std::string Start = CG::NewLabel();
	std::string End = CG::NewLabel();

	CG::addLine(Start + ":");

	Exp = Expression();

	MatchToken(TOKEN_CLOSING_PARENTHESIS);

	CG::addLine("\tmov al, DS:[" + Exp->m_Address + "]");
	CG::addLine("\tcmp al, 255");
	CG::addLine("\tjne " + End);

	if (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
	{
		Command();
	}
	else
	{
		MatchToken(TOKEN_BEGIN);

		while (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
		{
			Command();
		}

		MatchToken(TOKEN_END);
	}

	CG::addLine("\tjmp " + Start);
	CG::addLine(End + ":");
}

void SyntacticAnalyser::If()
{
	MatchToken(TOKEN_OPENING_PARENTHESIS);

	CG::g_TemporaryVars = 0;

	std::string Else = CG::NewLabel();
	std::string End = CG::NewLabel();

	Function *Exp = Expression();

	MatchToken(TOKEN_CLOSING_PARENTHESIS);

	CG::addLine("\tmov al, DS:[" + Exp->m_Address + "]");
	CG::addLine("\tcmp al, 255");
	CG::addLine("\tjne " + Else);

	MatchToken(TOKEN_THEN);

	if (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
	{
		Command();
	}
	else
	{
		MatchToken(TOKEN_BEGIN);

		while (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
		{
			Command();
		}

		MatchToken(TOKEN_END);
	}

	if (m_CurrentRegister->m_Token == TOKEN_ELSE)
	{
		MatchToken(TOKEN_ELSE);

		CG::addLine("\tjmp " + End);
		CG::addLine(Else + ":");

		if (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
		{
			Command();
		}
		else
		{
			MatchToken(TOKEN_BEGIN);

			while (m_CurrentRegister->m_Type == TOKEN_TYPE_COMMAND)
			{
				Command();
			}

			MatchToken(TOKEN_END);
		}
	}

	CG::addLine(End + ":");
}

void SyntacticAnalyser::ReadLn()
{
	MatchToken(TOKEN_OPENING_PARENTHESIS);

	MatchToken(TOKEN_ID);

	Symbol *id = Symbols::SymbolsTable.at(m_PreviousRegister->m_Lexeme).get();

	if (m_PreviousRegister->m_ClassId == ID_CLASS_NA)
	{
		fprintf(stderr, "[Semantic Analyser] - %d:identificador não declarado %s.\r\n", m_CurrentRegister->m_LineNumber, m_PreviousRegister->m_Lexeme.c_str());

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}

	if (m_PreviousRegister->m_RegisterType == REGISTER_TYPE_LOGICAL)
	{
		fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}

	MatchToken(TOKEN_CLOSING_PARENTHESIS);

	MatchToken(TOKEN_SEMICOLON);

	std::string buffer = CG::NewTemp(REGISTER_TYPE_STRING, true);

	CG::addLine("\tmov dx, " + buffer);
	CG::addLine("\tmov al, 255");
	CG::addLine("\tmov DS:[" + buffer + "], al");
	CG::addLine("\tmov ah, 10");
	CG::addLine("\tint 33");
	CG::addLine("\tmov ah, 2");
	CG::addLine("\tmov dl, 13");
	CG::addLine("\tint 33");
	CG::addLine("\tmov dl, 10");
	CG::addLine("\tint 33");

	std::string Start = CG::NewLabel();
	std::string End = CG::NewLabel();

	if (id->m_Type == REGISTER_TYPE_INTEGER)
	{
		std::string Positive = CG::NewLabel();

		CG::addLine("\tmov di, " + std::to_string(std::stoi(buffer) + 2));
		CG::addLine("\tmov ax, 0");
		CG::addLine("\tmov cx, 10");
		CG::addLine("\tmov dx, 1");
		CG::addLine("\tmov bh, 0");
		CG::addLine("\tmov bl, DS:[di]");
		CG::addLine("\tcmp bx, 45");
		CG::addLine("\tjne " + Positive);
		CG::addLine("\tmov dx, -1");
		CG::addLine("\tadd di, 1");
		CG::addLine("\tmov bl, DS:[di]");
		CG::addLine(Positive + ":");
		CG::addLine("\tpush dx");
		CG::addLine("\tmov dx, 0");
		CG::addLine(Start + ":");
		CG::addLine("\tcmp bx, 13");
		CG::addLine("\tje " + End);
		CG::addLine("\timul cx");
		CG::addLine("\tadd bx, -48");
		CG::addLine("\tadd ax, bx");
		CG::addLine("\tadd di, 1");
		CG::addLine("\tmov bh, 0");
		CG::addLine("\tmov bl, DS:[di]");
		CG::addLine("\tjmp " + Start);
		CG::addLine(End + ":");
		CG::addLine("\tpop cx");
		CG::addLine("\timul cx");
		CG::addLine("\tmov DS:[" + std::to_string(id->m_MemoryAddress) + "], ax");
	}
	else if (id->m_Type == REGISTER_TYPE_BYTE || id->m_Type == REGISTER_TYPE_LOGICAL)
	{
		CG::addLine("\tmov di, " + std::to_string(std::stoi(buffer) + 2));
		CG::addLine("\tmov al, 0");
		CG::addLine("\tmov cl, 10");
		CG::addLine(Start + ":");
		CG::addLine("\tmov bl, DS:[di]");
		CG::addLine("\tmov bh, 0");
		CG::addLine("\tcmp bx, 13");
		CG::addLine("\tje " + End);
		CG::addLine("\tmul cl");
		CG::addLine("\tadd bl, -48");
		CG::addLine("\tadd al, bl");
		CG::addLine("\tadd di, 1");
		CG::addLine("\tjmp " + Start);
		CG::addLine(End + ":");
		CG::addLine("\tmov DS:[" + std::to_string(id->m_MemoryAddress) + "], al");
	}
	else
	{
		std::string dollar = std::to_string((int)'$');

		CG::addLine("\tmov si, " + std::to_string(std::stoi(buffer) + 2));
		CG::addLine("\tmov di, " + std::to_string(id->m_MemoryAddress));
		CG::addLine(Start + ":");
		CG::addLine("\tmov al, DS:[si]");
		CG::addLine("\tcmp al, 13");
		CG::addLine("\tje " + End);
		CG::addLine("\tmov DS:[di], al");
		CG::addLine("\tadd di, 1");
		CG::addLine("\tadd si, 1");
		CG::addLine("\tjmp " + Start);
		CG::addLine(End + ":");
		CG::addLine("\tmov al, " + dollar);
		CG::addLine("\tmov DS:[di], al");
	}
}

void SyntacticAnalyser::Write()
{
	bool breakLine = (m_PreviousRegister->m_Token == TOKEN_WRITELN) ? true : false;

	MatchToken(TOKEN_OPENING_PARENTHESIS);

	CG::g_TemporaryVars = 0;

	std::vector<Function *> expressions;

	Function *Exp = Expression();

	if (Exp->m_Type == REGISTER_TYPE_LOGICAL)
	{
		fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

		Globals::PauseAndExit(SYNTACTIC_ERROR);
	}

	expressions.push_back(Exp);

	while (m_CurrentRegister->m_Token == TOKEN_COMMA)
	{
		MatchToken(TOKEN_COMMA);

		CG::g_TemporaryVars = 0;

		Exp = Expression();

		if (Exp->m_Type == REGISTER_TYPE_LOGICAL)
		{
			fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}

		expressions.push_back(Exp);
	}

	MatchToken(TOKEN_CLOSING_PARENTHESIS);

	MatchToken(TOKEN_SEMICOLON);

	for (std::vector<Function *>::iterator it = expressions.begin(); it != expressions.end(); ++it)
	{
		if (Exp->m_Type != REGISTER_TYPE_STRING)
		{
			std::string buffer = CG::NewTemp(REGISTER_TYPE_STRING);

			std::string Positive = CG::NewLabel();
			std::string Div = CG::NewLabel();
			std::string FillString = CG::NewLabel();

			std::string dollar = std::to_string((int)'$');

			CG::addLine("\tmov ah, 0");
			CG::addLine("\tmov ax, DS:[" + (*it)->m_Address + "]");
			CG::addLine("\tmov di, " + buffer);
			CG::addLine("\tmov cx, 0");
			CG::addLine("\tcmp ax, 0");
			CG::addLine("\tjge " + Positive);
			CG::addLine("\tmov bl, 45");
			CG::addLine("\tmov DS:[di], bl");
			CG::addLine("\tadd di, 1");
			CG::addLine("\tneg ax");
			CG::addLine(Positive + ":");
			CG::addLine("\tmov bx, 10");
			CG::addLine(Div + ":");
			CG::addLine("\tadd cx, 1");
			CG::addLine("\tmov dx, 0");
			CG::addLine("\tidiv bx");
			CG::addLine("\tpush dx");
			CG::addLine("\tcmp ax, 0");
			CG::addLine("\tjne " + Div);
			CG::addLine(FillString + ":");
			CG::addLine("\tpop dx");
			CG::addLine("\tadd dx, 48");
			CG::addLine("\tmov DS:[di], dl");
			CG::addLine("\tadd di, 1");
			CG::addLine("\tadd cx, -1");
			CG::addLine("\tcmp cx, 0");
			CG::addLine("\tjne " + FillString);
			CG::addLine("\tmov dl, " + dollar);
			CG::addLine("\tmov DS:[di], dl");
			CG::addLine("\tmov dx, " + buffer);
			CG::addLine("\tmov ah, 9");
			CG::addLine("\tint 33");
		}
		else
		{
			CG::addLine("\tmov dx, " + (*it)->m_Address);
			CG::addLine("\tmov ah, 9");
			CG::addLine("\tint 33");
		}
	}

	if (breakLine)
	{
		CG::addLine("\tmov ah, 2");
		CG::addLine("\tmov dl, 13");
		CG::addLine("\tint 33");
		CG::addLine("\tmov DL, 10");
		CG::addLine("\tint 33");
	}
}

Function *SyntacticAnalyser::Expression()
{
	Function *Exp1, *Exp2, *Exp;

	Exp = Exp1 = SubExpression();

	if (m_CurrentRegister->m_Type == TOKEN_TYPE_COMPARISSON)
	{
		Exp = new Function();

		MatchToken(TOKEN_TYPE_COMPARISSON);

		Token comparrison = m_PreviousRegister->m_Token;

		if (comparrison != TOKEN_EQUALITY)
		{
			if (Exp1->m_Type == REGISTER_TYPE_STRING)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			if (comparrison != TOKEN_DIFFERENT && Exp1->m_Type == REGISTER_TYPE_LOGICAL)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}
		}

		Exp2 = SubExpression();

		if (Exp1->m_Type != Exp2->m_Type)
		{
			if ((Exp2->m_Type != REGISTER_TYPE_INTEGER || Exp1->m_Type != REGISTER_TYPE_BYTE) && (Exp1->m_Type != REGISTER_TYPE_INTEGER || Exp2->m_Type != REGISTER_TYPE_BYTE))
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}
		}

		Exp->m_Type = REGISTER_TYPE_LOGICAL;
		Exp->m_Address = CG::NewTemp(Exp->m_Type);

		std::string tempRegister1, tempRegister2, Label1, Label2;

		if (Exp1->m_Type != Exp2->m_Type)
		{
			tempRegister1 = "ax";
			tempRegister2 = "bx";

			if (Exp1->m_Type == REGISTER_TYPE_BYTE)
			{
				CG::addLine("\tmov al, DS:[" + Exp1->m_Address + "]");
				CG::addLine("\tmov ah, 0");
			}
			else
			{
				CG::addLine("\tmov bl, DS:[" + Exp2->m_Address + "]");
				CG::addLine("\tmov bh, 0");
			}
		}
		else if (Exp1->m_Type != REGISTER_TYPE_STRING)
		{
			tempRegister1 = (Exp1->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "al";
			tempRegister2 = (Exp2->m_Type == REGISTER_TYPE_INTEGER) ? "bx" : "bl";

			CG::addLine("\tmov " + tempRegister1 + ", DS:[" + Exp1->m_Address + "]");
			CG::addLine("\tmov " + tempRegister2 + ", DS:[" + Exp2->m_Address + "]");
		}

		switch (comparrison)
		{
		case TOKEN_MORE_THAN:

			Label1 = CG::NewLabel();
			Label2 = CG::NewLabel();

			CG::addLine("\tcmp " + tempRegister1 + ", " + tempRegister2);
			CG::addLine("\tjg " + Label1);
			CG::addLine("\tmov al, 0");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine("\tjmp " + Label2);
			CG::addLine(Label1 + ":");
			CG::addLine("\tmov al, 255");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine(Label2 + ":");

			break;

		case TOKEN_LESS_THAN:

			Label1 = CG::NewLabel();
			Label2 = CG::NewLabel();

			CG::addLine("\tcmp " + tempRegister1 + ", " + tempRegister2);
			CG::addLine("\tjl " + Label1);
			CG::addLine("\tmov al, 0");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine("\tjmp " + Label2);
			CG::addLine(Label1 + ":");
			CG::addLine("\tmov al, 255");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine(Label2 + ":");

			break;

		case TOKEN_MORE_THAN_OR_EQUAL:

			Label1 = CG::NewLabel();
			Label2 = CG::NewLabel();

			CG::addLine("\tcmp " + tempRegister1 + ", " + tempRegister2);
			CG::addLine("\tjge " + Label1);
			CG::addLine("\tmov al, 0");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine("\tjmp " + Label2);
			CG::addLine(Label1 + ":");
			CG::addLine("\tmov al, 255");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine(Label2 + ":");

			break;

		case TOKEN_LESS_THAN_OR_EQUAL:

			Label1 = CG::NewLabel();
			Label2 = CG::NewLabel();

			CG::addLine("\tcmp " + tempRegister1 + ", " + tempRegister2);
			CG::addLine("\tjle " + Label1);
			CG::addLine("\tmov al, 0");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine("\tjmp " + Label2);
			CG::addLine(Label1 + ":");
			CG::addLine("\tmov al, 255");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine(Label2 + ":");

			break;

		case TOKEN_EQUALITY:

			if (Exp1->m_Type == REGISTER_TYPE_STRING)
			{
				std::string dollar = std::to_string((int)'$');

				std::string EqualStart = CG::NewLabel();
				std::string StringEnd = CG::NewLabel();
				std::string NotEqual = CG::NewLabel();
				std::string End = CG::NewLabel();

				CG::addLine("\tmov al, DS:[" + Exp1->m_Address + "]");
				CG::addLine("\tmov ah, 0");
				CG::addLine("\tmov bx, " + Exp1->m_Address);
				CG::addLine("\tmov cl, DS:[" + Exp2->m_Address + "]");
				CG::addLine("\tmov dx, " + Exp2->m_Address);
				CG::addLine(EqualStart + ":");
				CG::addLine("\tcmp al, cl");
				CG::addLine("\tjne " + NotEqual);
				CG::addLine("\tcmp ax, " + dollar);
				CG::addLine("\tje " + StringEnd);
				CG::addLine("\tadd bx, 1");
				CG::addLine("\tadd dx, 1");
				CG::addLine("\tjmp " + EqualStart);
				CG::addLine(NotEqual + ":");
				CG::addLine("\tmov al, 0");
				CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
				CG::addLine("\tjmp " + End);
				CG::addLine(StringEnd + ":");
				CG::addLine("\tmov al, 255");
				CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
				CG::addLine(End + ":");
			}
			else
			{
				Label1 = CG::NewLabel();
				Label2 = CG::NewLabel();

				CG::addLine("\tcmp " + tempRegister1 + ", " + tempRegister2);
				CG::addLine("\tje " + Label1);
				CG::addLine("\tmov al, 0");
				CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
				CG::addLine("\tjmp " + Label2);
				CG::addLine(Label1 + ":");
				CG::addLine("\tmov al, 255");
				CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
				CG::addLine(Label2 + ":");
			}

			break;

		case TOKEN_DIFFERENT:

			Label1 = CG::NewLabel();
			Label2 = CG::NewLabel();

			CG::addLine("\tcmp " + tempRegister1 + ", " + tempRegister2);
			CG::addLine("\tjne " + Label1);
			CG::addLine("\tmov al, 0");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine("\tjmp " + Label2);
			CG::addLine(Label1 + ":");
			CG::addLine("\tmov al, 255");
			CG::addLine("\tmov DS:[" + Exp->m_Address + "], al");
			CG::addLine(Label2 + ":");

			break;
		}
	}

	return Exp;
}

Function *SyntacticAnalyser::SubExpression()
{
	Function *Term1, *Term2, *ExpS;
	Token previousOperation;

	if (m_CurrentRegister->m_Token == TOKEN_MINUS)
	{
		MatchToken(m_CurrentRegister->m_Token);
	}

	previousOperation = m_PreviousRegister->m_Token;

	ExpS = Term1 = Term();

	if (previousOperation == TOKEN_MINUS)
	{
		if (Term1->m_Type != REGISTER_TYPE_BYTE && Term1->m_Type != REGISTER_TYPE_INTEGER)
		{
			fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

			Globals::PauseAndExit(SYNTACTIC_ERROR);
		}

		if (Term1->m_Type != REGISTER_TYPE_INTEGER)
		{
			CG::addLine("\tmov al, DS:[" + Term1->m_Address + "]");
			CG::addLine("\tmov ah, 0");
		}
		else
		{
			CG::addLine("\tmov ax, DS:[" + Term1->m_Address + "]");
		}

		CG::addLine("\tneg ax");

		Term1->m_Type = REGISTER_TYPE_INTEGER;
		Term1->m_Address = CG::NewTemp(Term1->m_Type);

		CG::addLine("\tmov DS:[" + Term1->m_Address + "], ax");
	}

	while (m_CurrentRegister->m_Token == TOKEN_PLUS || m_CurrentRegister->m_Token == TOKEN_MINUS || m_CurrentRegister->m_Token == TOKEN_OR)
	{
		ExpS = new Function();

		std::string tempRegister1, tempRegister2, tempRegister3;

		MatchToken(m_CurrentRegister->m_Token);

		previousOperation = m_PreviousRegister->m_Token;

		Term2 = Term();

		switch (previousOperation)
		{
		case TOKEN_PLUS:

			if (Term2->m_Type == REGISTER_TYPE_LOGICAL)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}
			else
			{
				if (Term1->m_Type != Term2->m_Type && (Term1->m_Type == REGISTER_TYPE_STRING || Term2->m_Type == REGISTER_TYPE_STRING))
				{
					fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

					Globals::PauseAndExit(SYNTACTIC_ERROR);
				}
			}

			if (Term1->m_Type == REGISTER_TYPE_STRING)
			{
				ExpS->m_Type = REGISTER_TYPE_STRING;
				ExpS->m_Address = CG::NewTemp(ExpS->m_Type);

				std::string dollar = std::to_string((int)'$');

				std::string ConcatStart = CG::NewLabel();
				std::string Concat2 = CG::NewLabel();
				std::string Concat3 = CG::NewLabel();
				std::string ConcatEnd = CG::NewLabel();

				CG::addLine("\tmov al, DS:[" + Term1->m_Address + "]");
				CG::addLine("\tmov ah, 0");
				CG::addLine("\tmov bx, " + Term1->m_Address);
				CG::addLine("\tmov di, " + ExpS->m_Address);
				CG::addLine(ConcatStart + ":");
				CG::addLine("\tcmp ax, " + dollar);
				CG::addLine("\tje " + Concat2);
				CG::addLine("\tmov DS:[di], al");
				CG::addLine("\tadd di, 1");
				CG::addLine("\tadd bx, 1");
				CG::addLine("\tmov al, DS:[bx]");
				CG::addLine("\tjmp " + ConcatStart);
				CG::addLine(Concat2 + ":");
				CG::addLine("\tmov al, DS:[" + Term2->m_Address + "]");
				CG::addLine("\tmov ah, 0");
				CG::addLine("\tmov bx, " + Term2->m_Address);
				CG::addLine(Concat3 + ":");
				CG::addLine("\tcmp ax, " + dollar);
				CG::addLine("\tje " + ConcatEnd);
				CG::addLine("\tmov DS:[di], al");
				CG::addLine("\tadd di, 1");
				CG::addLine("\tadd bx, 1");
				CG::addLine("\tmov al, DS:[bx]");
				CG::addLine("\tjmp " + Concat3);
				CG::addLine(ConcatEnd + ":");
				CG::addLine("\tmov ax, " + dollar);
				CG::addLine("\tmov DS:[di], ax");
			}
			else
			{
				tempRegister1 = (Term1->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "al";
				tempRegister2 = (Term2->m_Type == REGISTER_TYPE_INTEGER) ? "bx" : "bl";

				CG::addLine("\tmov " + tempRegister1 + ", DS:[" + Term1->m_Address + "]");
				CG::addLine("\tmov " + tempRegister2 + ", DS:[" + Term2->m_Address + "]");

				tempRegister3 = tempRegister1;
				tempRegister1 = tempRegister2;

				if (Term1->m_Type != Term2->m_Type)
				{
					ExpS->m_Type = REGISTER_TYPE_INTEGER;

					if (Term1->m_Type != REGISTER_TYPE_INTEGER)
					{
						tempRegister1 = tempRegister3;
						tempRegister3 = tempRegister2;
					}
				}

				CG::addLine("\tadd " + tempRegister3 + ", " + tempRegister1);

				ExpS->m_Type = Term1->m_Type;
				ExpS->m_Address = CG::NewTemp(ExpS->m_Type);

				CG::addLine("\tmov DS:[" + ExpS->m_Address + "], " + tempRegister3);
			}

			break;

		case TOKEN_MINUS:

			if ((Term1->m_Type != REGISTER_TYPE_INTEGER || Term1->m_Type != REGISTER_TYPE_BYTE) && (Term2->m_Type != REGISTER_TYPE_INTEGER || Term2->m_Type != REGISTER_TYPE_BYTE))
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			tempRegister1 = (Term1->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "al";
			tempRegister2 = (Term2->m_Type == REGISTER_TYPE_INTEGER) ? "bx" : "bl";

			CG::addLine("\tmov " + tempRegister1 + ", DS:[" + Term1->m_Address + "]");
			CG::addLine("\tmov " + tempRegister2 + ", DS:[" + Term2->m_Address + "]");

			tempRegister3 = tempRegister1;
			tempRegister1 = tempRegister2;

			if (Term1->m_Type != Term2->m_Type && Term1->m_Type != REGISTER_TYPE_INTEGER)
			{
				tempRegister1 = tempRegister3;
				tempRegister3 = tempRegister2;
			}

			CG::addLine("\tsub " + tempRegister3 + ", " + tempRegister1);

			ExpS->m_Type = REGISTER_TYPE_INTEGER;
			ExpS->m_Address = CG::NewTemp(ExpS->m_Type);

			CG::addLine("\tmov DS:[" + ExpS->m_Address + "], " + tempRegister3);

			break;

		case TOKEN_OR:

			if (Term1->m_Type != REGISTER_TYPE_LOGICAL || Term2->m_Type != REGISTER_TYPE_LOGICAL)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			CG::addLine("\tmov al, DS:[" + Term1->m_Address + "]");
			CG::addLine("\tmov bl, DS:[" + Term2->m_Address + "]");
			CG::addLine("\tor al, bl");

			ExpS->m_Type = REGISTER_TYPE_LOGICAL;
			ExpS->m_Address = CG::NewTemp(ExpS->m_Type);

			CG::addLine("\tmov DS:[" + ExpS->m_Address + "], al");


			break;
		}
	}

	return ExpS;
}

Function *SyntacticAnalyser::Term()
{
	Function *Factor1, *Factor2, *Term;

	Term = Factor1 = Factor();

	while (m_CurrentRegister->m_Token == TOKEN_MULTIPLY || m_CurrentRegister->m_Token == TOKEN_DIVIDE || m_CurrentRegister->m_Token == TOKEN_AND)
	{
		Term = new Function();

		std::string tempRegister1, tempRegister2;

		MatchToken(m_CurrentRegister->m_Token);

		Token previousOperation = m_PreviousRegister->m_Token;

		Factor2 = Factor();

		switch (previousOperation)
		{

		case TOKEN_MULTIPLY:

			if ((Factor2->m_Type != REGISTER_TYPE_INTEGER || Factor2->m_Type != REGISTER_TYPE_BYTE) && (Factor1->m_Type != REGISTER_TYPE_INTEGER || Factor1->m_Type != REGISTER_TYPE_BYTE))
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			if (Factor1->m_Type != Factor2->m_Type)
			{
				tempRegister1 = (Factor1->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "bl";
				tempRegister2 = (Factor2->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "bl";

				Term->m_Type = REGISTER_TYPE_INTEGER;
			}
			else
			{
				tempRegister1 = (Factor1->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "al";
				tempRegister2 = (Factor2->m_Type == REGISTER_TYPE_INTEGER) ? "bx" : "bl";

				Term->m_Type = Factor1->m_Type;
			}

			CG::addLine("\tmov " + tempRegister1 + ", DS:[" + Factor1->m_Address + "]");
			CG::addLine("\tmov " + tempRegister2 + ", DS:[" + Factor2->m_Address + "]");
			CG::addLine("\timul " + tempRegister1);

			Term->m_Address = CG::NewTemp(Term->m_Type);

			CG::addLine("\tmov DS:[" + Term->m_Address + "], " + tempRegister1);

			break;

		case TOKEN_DIVIDE:

			if ((Factor2->m_Type != REGISTER_TYPE_INTEGER || Factor2->m_Type != REGISTER_TYPE_BYTE) && (Factor1->m_Type != REGISTER_TYPE_INTEGER || Factor1->m_Type != REGISTER_TYPE_BYTE))
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			if (Factor1->m_Type != REGISTER_TYPE_INTEGER)
			{
				CG::addLine("\tmov al, DS:[" + Factor1->m_Address + "]");
				CG::addLine("\tmov ah, 0");
			}
			else
			{
				CG::addLine("\tmov ax, DS:[" + Factor1->m_Address + "]");
			}

			if (Factor2->m_Type != REGISTER_TYPE_INTEGER)
			{
				CG::addLine("\tmov bl, DS:[" + Factor2->m_Address + "]");
				CG::addLine("\tmov bh, 0");
			}
			else
			{
				CG::addLine("\tmov bx, DS:[" + Factor2->m_Address + "]");
			}

			CG::addLine("\tidiv bx");

			Term->m_Type = REGISTER_TYPE_INTEGER;
			Term->m_Address = CG::NewTemp(Term->m_Type);

			CG::addLine("\tmov DS:[" + Term->m_Address + "], ax");

			break;

		case TOKEN_AND:

			if (Factor1->m_Type != REGISTER_TYPE_LOGICAL || Factor2->m_Type != REGISTER_TYPE_LOGICAL)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			CG::addLine("\tmov al, DS:[" + Factor1->m_Address + "]");
			CG::addLine("\tmov bl, DS:[" + Factor2->m_Address + "]");
			CG::addLine("\tand al, bl");

			Term->m_Type = REGISTER_TYPE_LOGICAL;
			Term->m_Address = CG::NewTemp(Term->m_Type);

			CG::addLine("\tmov DS:[" + Term->m_Address + "], al");

			break;
		}
	}

	return Term;
}

Function *SyntacticAnalyser::Factor()
{
	Function *Factor1, *ReturnValue = new Function();
	Symbol *id;

	if (m_CurrentRegister->m_Type == TOKEN_TYPE_VALUE)
	{
		MatchToken(TOKEN_TYPE_VALUE);

		ReturnValue->m_Type = ExtractType(m_PreviousRegister->m_Token);

		if (ReturnValue->m_Type == REGISTER_TYPE_STRING)
		{
			ReturnValue->m_Address = CG::addConstantDeclaration(ReturnValue->m_Type, m_PreviousRegister->m_Lexeme);
		}
		else
		{
			if (ReturnValue->m_Type == REGISTER_TYPE_BYTE)
			{
				if (m_PreviousRegister->m_Lexeme.length() > 2 && (m_PreviousRegister->m_Lexeme[0] == '0' && m_PreviousRegister->m_Lexeme[1] == 'h'))
				{
					m_PreviousRegister->m_Lexeme.erase(0, 2);
					m_PreviousRegister->m_Lexeme = std::to_string(std::stoi(m_PreviousRegister->m_Lexeme, 0, 16));
				}

				int intValue = std::stoi(m_PreviousRegister->m_Lexeme);

				if (intValue > 255)
				{
					ReturnValue->m_Type = REGISTER_TYPE_INTEGER;
				}
			}
			else if (ReturnValue->m_Type == REGISTER_TYPE_LOGICAL)
			{
				if (m_PreviousRegister->m_Lexeme == "true" || m_PreviousRegister->m_Lexeme == "false")
				{
					m_PreviousRegister->m_Lexeme = (m_PreviousRegister->m_Lexeme == "true") ? "255" : "0";
				}
			}

			ReturnValue->m_Address = CG::NewTemp(ReturnValue->m_Type);

			std::string tempRegister = (ReturnValue->m_Type == REGISTER_TYPE_INTEGER) ? "ax" : "bl";

			CG::addLine("\tmov " + tempRegister + ", " + m_PreviousRegister->m_Lexeme);
			CG::addLine("\tmov DS:[" + ReturnValue->m_Address + "], " + tempRegister);
		}
	}
	else
	{
		switch (m_CurrentRegister->m_Token)
		{
		case TOKEN_OPENING_PARENTHESIS:

			MatchToken(TOKEN_OPENING_PARENTHESIS);

			ReturnValue = Expression();

			MatchToken(TOKEN_CLOSING_PARENTHESIS);

			break;

		case TOKEN_ID:

			MatchToken(TOKEN_ID);

			id = Symbols::SymbolsTable.at(m_PreviousRegister->m_Lexeme).get();

			if (id->m_Class == ID_CLASS_NA)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:identificador não declarado %s.\r\n", m_CurrentRegister->m_LineNumber, m_PreviousRegister->m_Lexeme.c_str());

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			ReturnValue->m_Type = id->m_Type;

			ReturnValue->m_Address = std::to_string(id->m_MemoryAddress);

			break;

		case TOKEN_NOT:

			MatchToken(TOKEN_NOT);

			Factor1 = Factor();

			if (ReturnValue->m_Type != REGISTER_TYPE_LOGICAL)
			{
				fprintf(stderr, "[Semantic Analyser] - %d:tipos incompatíveis.\r\n", m_CurrentRegister->m_LineNumber);

				Globals::PauseAndExit(SYNTACTIC_ERROR);
			}

			ReturnValue->m_Type = REGISTER_TYPE_LOGICAL;
			ReturnValue->m_Address = CG::NewTemp(ReturnValue->m_Type);

			CG::addLine("\tmov al, DS:[" + Factor1->m_Address + "]");
			CG::addLine("\tnot al");
			CG::addLine("\tmov DS:[" + ReturnValue->m_Address + "], al");

			break;

		default:
			MatchToken(TOKEN_TYPE_VALUE);
			break;
		}
	}

	return ReturnValue;
}

#pragma endregion