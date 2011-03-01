#include <iostream>

#include "src/App.h"

using namespace std;

int main()
{
    App* app = new App();
    app->run();
    delete app;

    return 0;
}
