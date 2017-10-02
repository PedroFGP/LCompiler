#pragma once

#include "Includes.hpp"

namespace Symbols
{
	extern std::unordered_map<std::string, std::unique_ptr<Symbol>> SymbolsTable;

	void Initialize();
}