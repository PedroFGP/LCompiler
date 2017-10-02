#include "Includes.hpp"

namespace Symbols
{
	std::unordered_map<std::string, std::unique_ptr<Symbol>> SymbolsTable;

	void Initialize()
	{
		SymbolsTable.emplace("byte", std::unique_ptr<Symbol>(new Symbol(TOKEN_BYTE, "byte")));
		SymbolsTable.emplace("integer", std::unique_ptr<Symbol>(new Symbol(TOKEN_INTEGER, "integer")));
		SymbolsTable.emplace("boolean", std::unique_ptr<Symbol>(new Symbol(TOKEN_BOOLEAN, "boolean")));
		SymbolsTable.emplace("string", std::unique_ptr<Symbol>(new Symbol(TOKEN_STRING, "string")));
		SymbolsTable.emplace("const", std::unique_ptr<Symbol>(new Symbol(TOKEN_CONSTANT, "const")));
		SymbolsTable.emplace("while", std::unique_ptr<Symbol>(new Symbol(TOKEN_WHILE, "while")));
		SymbolsTable.emplace("if", std::unique_ptr<Symbol>(new Symbol(TOKEN_IF, "if")));
		SymbolsTable.emplace("else", std::unique_ptr<Symbol>(new Symbol(TOKEN_ELSE, "else")));
		SymbolsTable.emplace("and", std::unique_ptr<Symbol>(new Symbol(TOKEN_AND, "and")));
		SymbolsTable.emplace("or", std::unique_ptr<Symbol>(new Symbol(TOKEN_OR, "or")));
		SymbolsTable.emplace("not", std::unique_ptr<Symbol>(new Symbol(TOKEN_NOT, "not")));
		SymbolsTable.emplace("=", std::unique_ptr<Symbol>(new Symbol(TOKEN_ATTRIBUTION, "=")));
		SymbolsTable.emplace("==", std::unique_ptr<Symbol>(new Symbol(TOKEN_EQUALITY, "==")));
		SymbolsTable.emplace("(", std::unique_ptr<Symbol>(new Symbol(TOKEN_OPENING_PARENTHESIS, "(")));
		SymbolsTable.emplace(")", std::unique_ptr<Symbol>(new Symbol(TOKEN_CLOSING_PARENTHESIS, ")")));
		SymbolsTable.emplace("<", std::unique_ptr<Symbol>(new Symbol(TOKEN_LESS_THAN, "<")));
		SymbolsTable.emplace(">", std::unique_ptr<Symbol>(new Symbol(TOKEN_MORE_THAN, ">")));
		SymbolsTable.emplace("!=", std::unique_ptr<Symbol>(new Symbol(TOKEN_DIFFERENT, "!=")));
		SymbolsTable.emplace(">=", std::unique_ptr<Symbol>(new Symbol(TOKEN_MORE_THAN_OR_EQUAL, ">=")));
		SymbolsTable.emplace("<=", std::unique_ptr<Symbol>(new Symbol(TOKEN_LESS_THAN_OR_EQUAL, "<=")));
		SymbolsTable.emplace(",", std::unique_ptr<Symbol>(new Symbol(TOKEN_COMMA, ",")));
		SymbolsTable.emplace("+", std::unique_ptr<Symbol>(new Symbol(TOKEN_PLUS, "+")));
		SymbolsTable.emplace("-", std::unique_ptr<Symbol>(new Symbol(TOKEN_MINUS, "-")));
		SymbolsTable.emplace("*", std::unique_ptr<Symbol>(new Symbol(TOKEN_MULTIPLY, "*")));
		SymbolsTable.emplace("/", std::unique_ptr<Symbol>(new Symbol(TOKEN_DIVIDE, "/")));
		SymbolsTable.emplace(";", std::unique_ptr<Symbol>(new Symbol(TOKEN_SEMICOLON, ";")));
		SymbolsTable.emplace("begin", std::unique_ptr<Symbol>(new Symbol(TOKEN_BEGIN, "begin")));
		SymbolsTable.emplace("end", std::unique_ptr<Symbol>(new Symbol(TOKEN_END, "end")));
		SymbolsTable.emplace("then", std::unique_ptr<Symbol>(new Symbol(TOKEN_THEN, "then")));
		SymbolsTable.emplace("readln", std::unique_ptr<Symbol>(new Symbol(TOKEN_READLN, "readln")));
		SymbolsTable.emplace("main", std::unique_ptr<Symbol>(new Symbol(TOKEN_MAIN, "main")));
		SymbolsTable.emplace("write", std::unique_ptr<Symbol>(new Symbol(TOKEN_WRITE, "write")));
		SymbolsTable.emplace("writeln", std::unique_ptr<Symbol>(new Symbol(TOKEN_WRITELN, "writeln")));
		SymbolsTable.emplace("true", std::unique_ptr<Symbol>(new Symbol(TOKEN_TRUE, "true")));
		SymbolsTable.emplace("false", std::unique_ptr<Symbol>(new Symbol(TOKEN_FALSE, "false")));
	}
}