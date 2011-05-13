#include "App.h"

#include <iostream>
#include <fstream>

using namespace std;

#if NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

// use the define for Linux. The defines for a win32 project are assigned into the project itself. not in code
#ifndef WIN32
	#define EDITOR
#endif

int main(int argc, char** argv)
{
    ///TODO: Create a Window to choose resolution

    App::getInstance()->setupDevice(NULL);
    App::getInstance()->initialize();
    App::getInstance()->run();

    return 0;
}
