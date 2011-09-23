#include "pch.h"
#include "BlockOut.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR commandLine, int showCommand)
{
	// Enable run-time memory check for debug builds.
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	BlockOut theApp(hInstance);
	
	theApp.InitApplication();

	return theApp.Run();
}
