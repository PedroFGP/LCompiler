#include "Includes.hpp"

namespace Globals
{
	FILE *g_FileHandle;
	std::ofstream g_ASMFile;

	void PauseAndExit(int errorCode)
	{
		system("pause");

		exit(errorCode);
	}
}
