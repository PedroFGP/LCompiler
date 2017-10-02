#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

//Custom headers

#include "Enums.hpp"
#include "Symbol.hpp"
#include "SymbolsHash.hpp"
#include "Function.hpp"
#include "LexicalRegister.hpp"
#include "LexicalAnalyser.hpp"
#include "SyntacticAnalyser.hpp"
#include "CodeGeneration.hpp"

namespace Globals 
{
	extern FILE *g_FileHandle;
	extern std::ofstream g_ASMFile;

	void PauseAndExit(int errorCode);
}
