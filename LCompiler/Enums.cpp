#include "Includes.hpp"

TokenType TokenToType(Token token)
{
	TokenType type = TOKEN_TYPE_DEFAULT;

	switch (token)
	{
	case TOKEN_BYTE:
	case TOKEN_INTEGER:
	case TOKEN_BOOLEAN:
	case TOKEN_STRING:
	case TOKEN_CONSTANT:

		type = TOKEN_TYPE_TYPE;

		break;

	case TOKEN_WHILE:
	case TOKEN_IF:
	case TOKEN_READLN:
	case TOKEN_WRITE:
	case TOKEN_WRITELN:
	case TOKEN_ID:

		type = TOKEN_TYPE_COMMAND;

		break;

	case TOKEN_STRING_VALUE:
	case TOKEN_INTEGER_VALUE:
	case TOKEN_HEXADECIMAL_VALUE:
	case TOKEN_TRUE:
	case TOKEN_FALSE:

		type = TOKEN_TYPE_VALUE;

		break;

	case TOKEN_EQUALITY:
	case TOKEN_LESS_THAN_OR_EQUAL:
	case TOKEN_MORE_THAN_OR_EQUAL:
	case TOKEN_DIFFERENT:
	case TOKEN_LESS_THAN:
	case TOKEN_MORE_THAN:

		type = TOKEN_TYPE_COMPARISSON;

		break;
	}

	return type;
}

std::string TokenDescription(Token token)
{
	std::string value = "";

	switch (token)
	{
	case TOKEN_BYTE:
	case TOKEN_INTEGER:
	case TOKEN_BOOLEAN:
	case TOKEN_STRING:
	case TOKEN_CONSTANT:

		value = "Tipo";

		break;

	case TOKEN_WHILE:

		value = "Commando de repeticao";

		break;
	case TOKEN_IF:
	case TOKEN_ELSE:

		value = "Desvio";

		break;

	case TOKEN_AND:
	case TOKEN_OR:
	case TOKEN_NOT:

		value = "Logico";

		break;

	case TOKEN_ATTRIBUTION:

		value = "Atribuicao";

		break;

	case TOKEN_EQUALITY:
	case TOKEN_MORE_THAN_OR_EQUAL:
	case TOKEN_LESS_THAN_OR_EQUAL:
	case TOKEN_DIFFERENT:
	case TOKEN_LESS_THAN:
	case TOKEN_MORE_THAN:

		value = "Comparacao";

		break;

	case TOKEN_OPENING_PARENTHESIS:
	case TOKEN_CLOSING_PARENTHESIS:

		value = "Precedencia";

		break;

	case TOKEN_COMMA:
	case TOKEN_SEMICOLON:
	case TOKEN_BEGIN:
	case TOKEN_END:

		value = "Delimitador";

		break;

	case TOKEN_PLUS:
	case TOKEN_MINUS:
	case TOKEN_MULTIPLY:
	case TOKEN_DIVIDE:

		value = "Simbolo aritimetico";

		break;

	case TOKEN_MAIN:

		value = "Palavra reservada";

		break;

	case TOKEN_THEN:
	case TOKEN_READLN:
	case TOKEN_WRITE:
	case TOKEN_WRITELN:

		value = "Comandos";

		break;

	case TOKEN_TRUE:

		value = "Valor constante true";

		break;

	case TOKEN_FALSE:

		value = "Valor constante falso";

		break;

	case TOKEN_INTEGER_VALUE:

		value = "Valor constante inteiro";

		break;

	case TOKEN_HEXADECIMAL_VALUE:

		value = "Valor constante hexadecimal";

		break;

	case TOKEN_STRING_VALUE:

		value = "Valor constante string";

		break;

	case TOKEN_ID:

		value = "Identificador";

		break;
	}

	return value;
}