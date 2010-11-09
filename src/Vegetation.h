#ifndef VEGETATION_H
#define VEGETATION_H

#include <irrlicht.h>
#include <vector>
#include "App.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class Vegetation
{
    public:
        Vegetation();
        virtual ~Vegetation();

        vector3df getPosition();
        void setPosition(vector3df pos);
        void setScale(vector3df scale);

        void showDebugData(bool show);

    protected:
    private:
        IMeshSceneNode* trunk;
        vector<IBillboardSceneNode*> leafs;

        //TODO: remove fake shadow when shadowmaps are available
        IMeshSceneNode* fakeShadow;

        f32 size;
};

#endif // VEGETATION_H
