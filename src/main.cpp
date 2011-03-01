#include "App.h"

#include <iostream>
#include <fstream>

using namespace std;

#if NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char** argv)
{
    ///TODO: Create a Window to choose resolution

    App::getInstance()->setupDevice();
    App::getInstance()->initialize();
    App::getInstance()->run();

    return 0;
}
