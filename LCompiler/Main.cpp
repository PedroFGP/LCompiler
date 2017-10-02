#include "Includes.hpp"

void OpenSourceFile(const std::string &path)
{
	Globals::g_FileHandle = fopen(path.c_str(), "r");

	if (Globals::g_FileHandle == NULL)
	{
		fprintf(stderr, "Erro ao tentar ler o arquivo: %s.\r\n", path.c_str());
	}
}

void CloseSourceFile()
{
	if (fclose(Globals::g_FileHandle) == EOF)
	{
		fprintf(stderr, "Falha ao fechar o arquivo!\r\n");
	}
}

int main(int argumentCount, char *argumentValues[])
{
	if (argumentCount != 3)
	{
		fprintf(stderr, "Uso correto: %s <codigofonte>.l <programaassembly>.asm.\r\n", argumentValues[0]);

		system("pause");

		return INVALID_ARGUMENTS;
	}

	OpenSourceFile(argumentValues[1]);

	Globals::g_ASMFile = std::ofstream(argumentValues[2]);

	//Globals::g_ASMFile.open("D:\\Visual Studio 2017\\LCompiler\\Debug\\out.asm");

	//OpenSourceFile("D:\\Visual Studio 2017\\LCompiler\\Debug\\exemplo.l");

	SyntacticAnalyser *syntacticAnalyser = new SyntacticAnalyser();

	syntacticAnalyser->RunAnalysis();

	CloseSourceFile();

	system("pause");

	return NORMAL;
}