#pragma once

class LexicalAnalyser
{
private:

	LexicalState m_State;
	std::string m_Lexeme;
	Token m_Token;
	int m_LineNumber;
	char m_ReturnChar;

public:

	bool m_Error;

	CharacterType getCharType(char character);

	void TestAnalyser();

	LexicalRegister *RunAnalysis();

	LexicalAnalyser();
};