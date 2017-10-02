#include "Includes.hpp"

#pragma once

namespace CG
{
	extern std::string m_MASM;
	extern int g_DSAddress; 
	extern int g_TemporaryVars; 
	extern int g_Labels;

	void Initialize();

	std::string NewTemp(RegisterType type, bool readln = false);

	std::string NewLabel();

	void FinalizeDeclaration();

	void EndProgram();

	void addLine(std::string line);

	void addLine(std::string line, std::string comment);

	void addDeclaration(Symbol *id, std::string value = "?");

	std::string addConstantDeclaration(RegisterType type, std::string value);
}