#ifndef _SHADERCB_
#define _SHADERCB_

#include <irrlicht.h>
//#include "HardwareSkinCallback.h"


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class ShaderCallBack : public video::IShaderConstantSetCallBack
{
private:
    IrrlichtDevice *device;
    int layer;
    int editingTerrain;


public:

    ShaderCallBack();

    static ShaderCallBack* getInstance();

    void setFlagEditingTerrain(bool edit);

	virtual void OnSetConstants(video::IMaterialRendererServices* services, s32 userData);
};

#endif
