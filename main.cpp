#include "C:\\MyDirC\\Code\\cpp\\usetool\\usetool7.1\\usetool.h"
#include "MainWindow.h"
int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	if (!NOther::CoInit()) return -1;
	std::string exeDir{ NSystem::ExeDir() };
	//NSystem::DPIScale();
	std::string cmdline{ lpCmdLine };
	cmdline.erase(0, cmdline.find_first_not_of(' '));
	cmdline.erase(cmdline.find_last_not_of(' ') + 1);

	NConsole::Create();

	{
		MainWindow mainWindow{};
		if (!mainWindow.Start())
		{
			TESTB("fail");
			return -1;
		}
	}
	NOther::UnCoInit();
	TESTB("ok");
	return 0;
}