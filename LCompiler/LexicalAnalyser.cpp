#include "Includes.hpp"

//TODO: verificar tamanho do identificador

LexicalAnalyser::LexicalAnalyser()
{
	Symbols::Initialize();

	m_State = STATE_INITIAL;
	m_Lexeme = "";
	m_Token = (Token)-1;
	m_LineNumber = 1;
	m_ReturnChar = '\0';

	m_Error = false;
}

CharacterType LexicalAnalyser::getCharType(char character)
{
	int charValue = (int)character;

	if (charValue == EOF)
	{
		return CHAR_EOF;
	}

	if (charValue >= (int)'a' && charValue <= (int)'z')
	{
		return CHAR_LOWERCASE_LETTER;
	}

	if (charValue >= (int)'A' && charValue <= (int)'Z')
	{
		return CHAR_UPPERCASE_LETTER;
	}

	if (charValue >= (int)'0' && charValue <= (int)'9')
	{
		return CHAR_DIGIT;
	}

	if (character == '\n' || character == '\t' || character == '_' || character == '+' ||
		character == '-' || character == '.' || character == ',' || character == ';' ||
		character == ':' || character == '(' || character == ')' || character == '>' ||
		character == '<' || character == '*' || character == ' ' || character == '/' ||
		character == '=' || character == '!' || character == '\'' || character == '\r' ||
		character == ' ')
	{
		return CHAR_SYMBOL;
	}

	return CHAR_INVALID;
}

void LexicalAnalyser::TestAnalyser()
{
	while (m_Error == false)
	{
		//retorna o último lexema encontra, cujo token está guardado no registro léxico(LexicalRegisters)
		RunAnalysis();
	}
}

LexicalRegister *LexicalAnalyser::RunAnalysis()
{
	char character;

	while (m_State != STATE_FINAL)
	{
		if (m_ReturnChar == '\0')
		{
			character = fgetc(Globals::g_FileHandle);
		}
		else
		{
			character = m_ReturnChar;

			m_ReturnChar = '\0';
		}

		CharacterType charType = getCharType(character);

		if (charType == CHAR_INVALID)
		{
			fprintf(stderr, "[Lexical Analyser] - %d:caractere invalido.\r\n", m_LineNumber);

			m_Error = true;

			Globals::PauseAndExit(LEXICAL_ERROR);
		}

		switch (m_State)
		{

		case STATE_INITIAL:

			m_Lexeme = "";

			if (character == '<' || character == '>' || character == '=')
			{
				m_Lexeme += character;
				m_State = STATE_SYMBOLS;
			}
			else if (character == ',' || character == ';' || character == '(' || character == ')' || character == '+' || character == '-' || character == '*')
			{
				m_Lexeme += character;
				m_State = STATE_FINAL;
			}
			else if (charType == CHAR_LOWERCASE_LETTER || charType == CHAR_UPPERCASE_LETTER)
			{
				m_Lexeme += character;
				m_State = STATE_IDL;
			}
			else if (character == '_')
			{
				m_Lexeme += character;
				m_State = STATE_ID_;
			}
			else if (character == '\'')
			{
				m_Lexeme += character;
				m_State = STATE_STRING1;
			}
			else if (getCharType(character) == CHAR_DIGIT && character != '0')
			{
				m_Lexeme += character;
				m_State = STATE_INTEGER;
			}
			else if (character == '0')
			{
				m_Lexeme += character;
				m_State = STATE_HEX1;
			}
			else if (character == '/')
			{
				m_State = STATE_COMMENT1;
			}
			else if (character == '\n')
			{
				++m_LineNumber;
				m_State = STATE_INITIAL;
			}
			else if (character == ' ' || character == '\t' || character == '\r')
			{
				m_State = STATE_INITIAL;
			}
			else if (character == '!')
			{
				m_Lexeme += character;

				m_State = STATE_DIFFERENT;
			}
			else if (charType == CHAR_EOF)
			{
				return nullptr;
			}

			break;

		case STATE_DIFFERENT:

			m_Lexeme += character;

			if (character == '=')
			{
				m_State = STATE_FINAL;
			}
			else
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_IDL:

			if (charType == CHAR_LOWERCASE_LETTER || charType == CHAR_UPPERCASE_LETTER || charType == CHAR_DIGIT || character == '_')
			{
				m_Lexeme += character;
			}
			else if (charType != CHAR_LOWERCASE_LETTER && charType != CHAR_UPPERCASE_LETTER && charType != CHAR_DIGIT && character != '_')
			{
				m_ReturnChar = character;
				m_State = STATE_FINAL;

				m_Token = TOKEN_ID;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_ID_:

			m_Lexeme += character;

			if (charType == CHAR_LOWERCASE_LETTER || charType == CHAR_UPPERCASE_LETTER || charType == CHAR_DIGIT)
			{
				m_State = STATE_IDL;
			}
			else if (character == '_')
			{
			}
			else
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_SYMBOLS:

			if (character == '=')
			{
				m_Lexeme += character;

				m_State = STATE_FINAL;
			}
			else if (character != '=')
			{
				m_ReturnChar = character;

				m_State = STATE_FINAL;
			}
			else
			{
				m_ReturnChar = character;

				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_STRING1:

			m_Lexeme += character;

			if (character == '\'')
			{
				m_State = STATE_STRING2;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_STRING2:

			if (character == '\'')
			{
				m_Lexeme += character;
				m_State = STATE_STRING1;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}
			else if (character != '\'')
			{
				m_ReturnChar = character;
				m_State = STATE_FINAL;

				m_Token = TOKEN_STRING_VALUE;
			}

			break;

		case STATE_INTEGER:

			if (charType == CHAR_DIGIT)
			{
				m_Lexeme += character;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}
			else if (charType != CHAR_DIGIT)
			{
				m_ReturnChar = character;
				m_State = STATE_FINAL;

				m_Token = TOKEN_INTEGER_VALUE;
			}

			break;

		case STATE_HEX1:

			if (charType == CHAR_DIGIT)
			{
				m_Lexeme += character;
				m_State = STATE_INTEGER;
			}
			else if (character == 'h')
			{
				m_Lexeme += character;
				m_State = STATE_HEX2;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}
			else
			{
				m_ReturnChar = character;
				m_State = STATE_FINAL;

				m_Token = TOKEN_INTEGER_VALUE;
			}

			break;

		case STATE_HEX2:

			m_Lexeme += character;

			if (charType == CHAR_DIGIT || charType == CHAR_UPPERCASE_LETTER)
			{
				m_State = STATE_HEX3;
			}
			else
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_HEX3:

			m_Lexeme += character;

			if (charType == CHAR_DIGIT || charType == CHAR_UPPERCASE_LETTER)
			{
				m_State = STATE_FINAL;

				m_Token = TOKEN_HEXADECIMAL_VALUE;
			}
			else
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_COMMENT1:

			if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}
			else if (character != '*')
			{
				m_ReturnChar = character;
				m_State = STATE_FINAL;

				m_Token = TOKEN_DIVIDE;
			}
			else if (character == '*')
			{
				m_State = STATE_COMMENT2;
			}

			break;

		case STATE_COMMENT2: //não intefere na formação do lexema, não vale a pena concatenar

			if (character == '*')
			{
				m_State = STATE_COMMENT3;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_COMMENT3: //não intefere na formação do lexema, não vale a pena concatenar

			if (character == '/')
			{
				m_State = STATE_INITIAL;
			}
			else if(charType == CHAR_EOF)
			{
				m_ReturnChar = character;
				m_State = STATE_INVALID_LEX;
			}

			break;

		case STATE_INVALID_LEX:

			if (character == EOF)
			{
				fprintf(stderr, "[Lexical Analyser] - %d:fim de arquivo nao esperado.\r\n", m_LineNumber);

				m_Error = true;

				Globals::PauseAndExit(LEXICAL_ERROR);
			}
			else
			{

				fprintf(stderr, "[Lexical Analyser] - %d:lexema nao identificado %s.\r\n", m_LineNumber, m_Lexeme.c_str());

				m_Error = true;

				Globals::PauseAndExit(LEXICAL_ERROR);
			}
		}
	}

	//STATE_FINAL

	if (Symbols::SymbolsTable.emplace(m_Lexeme, std::unique_ptr<Symbol>(new Symbol(m_Token, m_Lexeme))).second == true)
	{
		//teste
		//fprintf(stdout, "%d - Lexema (%s) incluido com id (%d) e tipo (%s).\r\n", m_LineNumber, m_Lexeme.c_str(), m_Token, TokenDescription(m_Token).c_str());
	}

	LexicalRegister *foundRegister = new LexicalRegister(m_Token, m_Lexeme, m_LineNumber);

	m_State = STATE_INITIAL;

	return foundRegister;
}
