#include "src/App.h"

#include <iostream>
#include <fstream>

using namespace std;

#if NDEBUG
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int main(int argc, char** argv)
{
    ///TODO: Create a Window to choose resolution
    int w = 1024;
    int h = 768;
    int f = 0;

    std::string line;
    ifstream myfile ("config.txt");
    if (myfile.is_open())
    {
        myfile >> w;
        myfile >> h;
        myfile >> f;
        myfile.close();
    }

    App::getInstance()->setupDevice(w,h,f);
    App::getInstance()->initialize();
    App::getInstance()->run();

    return 0;
}
