#include "Includes.hpp"

namespace CG
{
	std::string m_MASM = "";
	int g_DSAddress = 16384;
	int g_TemporaryVars = 0;
	int g_Labels = 0;

	void FlushToFile()
	{
		Globals::g_ASMFile << m_MASM;
		Globals::g_ASMFile.close();
	}

	int CalculateSize(RegisterType type, std::string value = "")
	{
		int size = 0;

		switch (type)
		{
		case REGISTER_TYPE_INTEGER:
			size = 2;
			break;
		case REGISTER_TYPE_LOGICAL:
		case REGISTER_TYPE_BYTE:
			size = 1;
			break;
		case REGISTER_TYPE_STRING:
			if (value != "")
			{
				size = value.length() -1;
			}
			else
			{
				size = 256;
			}
			break;
		}

		return size;
	}

	std::string CreateDeclaration(int size, std::string value)
	{
		std::string allocation = "\t";

		if (size == 1)
		{
			if (value == "true" || value == "false")
			{
				value = (value == "true") ? "255" : "0";
			}

			allocation += "byte " + value;
		}
		else if (size == 2)
		{
			allocation += "sword " + value;
		}
		else if (size > 2)
		{
			if (value != "?")
			{
				allocation += "byte \"" + value + "$\"";
				allocation.erase(std::remove(allocation.begin(), allocation.end(), '\''), allocation.end());
			}
			else
			{
				allocation += "byte 256 DUP(?)";
			}
		}

		return allocation;
	}

	std::string GetTypeDescription(RegisterType type)
	{
		std::string description;

		switch (type)
		{
		case REGISTER_TYPE_INTEGER:
			description = "int";
			break;
		case REGISTER_TYPE_BYTE:
			description = "byte";
			break;
		case REGISTER_TYPE_LOGICAL:
			description = "boolean";
			break;
		case REGISTER_TYPE_STRING:
			description = "string";
			break;
		}

		return description;
	}

	std::string NewTemp(RegisterType type, bool readln)
	{
		int address = g_TemporaryVars;

		g_TemporaryVars += CalculateSize(type);

		if(readln)
		{
			g_TemporaryVars += 3;
		}

		return std::to_string(address);
	}

	std::string NewLabel()
	{
		std::string label = "Label" + std::to_string(g_Labels);

		++g_Labels;

		return label;
	}

	void Initialize()
	{
		addLine("sseg SEGMENT STACK", "início seg. pilha");
		addLine("\tbyte 16384 DUP(?)", "dimensiona pilha");
		addLine("sseg ENDS", "fim seg. pilha");
		addLine("\ndseg SEGMENT PUBLIC", "início seg. dados");
		addLine("\tbyte 16384 DUP(?)", "temporários");
	}

	void FinalizeDeclaration()
	{
		addLine("dseg ENDS", "fim seg. dados");
		addLine("\ncseg SEGMENT PUBLIC", "inicio do seg. código");
		addLine("\tASSUME CS:cseg, DS:dseg");
		addLine("\nStart:");
		addLine("\tmov ax, dseg");
		addLine("\tmov ds, ax");
	}

	void EndProgram()
	{
		addLine("\n\tmov ah, 4Ch", "termina o programa");
		addLine("\tint 21h");
		addLine("cseg ENDS", "fim seg.código");
		addLine("\nEND Start");

		FlushToFile();
	}

	void addLine(std::string line)
	{
		m_MASM += line + "\n";
	}

	void addLine(std::string line, std::string comment)
	{
		m_MASM += line + "\t; " + comment + "\n";
	}

	void addDeclaration(Symbol *id, std::string value)
	{
		if (id->m_Type == REGISTER_TYPE_INTEGER && id->m_Class == ID_CLASS_CONST)
		{
			int intValue = std::stoi(value);

			if (intValue < 255)
			{
				id->m_Type = REGISTER_TYPE_BYTE;
			}
		}

		int size = CalculateSize(id->m_Type);

		if (size != 0)
		{
			if (value.length() > 2 && (value[0] == '0' && value[1] == 'h'))
			{
				value.erase(0, 2);
				value = std::to_string(std::stoi(value, 0, 16));
			}

			std::string allocation = CreateDeclaration(size, value);

			std::string idClass = (id->m_Class == ID_CLASS_VAR) ? "var" : "const";

			addLine(allocation, idClass + " " + id->m_Lexeme + " " + GetTypeDescription(id->m_Type) + " em " + std::to_string(g_DSAddress));

			id->m_MemoryAddress = g_DSAddress;

			g_DSAddress += size;
		}
	}

	std::string addConstantDeclaration(RegisterType type, std::string value)
	{
		if (type == REGISTER_TYPE_BYTE)
		{
			int intValue = std::stoi(value);

			if (intValue > 255)
			{
				type = REGISTER_TYPE_INTEGER;
			}
		}

		int size = CalculateSize(type, value);

		int address = g_DSAddress;

		if (value.length() > 2 && (value[0] == '0' && value[1] == 'h'))
		{
			value.erase(0, 2);
			value = std::to_string(std::stoi(value, 0, 16));
		}

		addLine("dseg SEGMENT PUBLIC", "inicio seg.dados");
		addLine(CreateDeclaration(size, value), +"const " + GetTypeDescription(type) + " em " + std::to_string(g_DSAddress));
		addLine("dseg ENDS", "fim seg.dados");

		g_DSAddress += size;

		return std::to_string(address);
	}
}