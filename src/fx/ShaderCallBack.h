#ifndef _SHADERCB_
#define _SHADERCB_

#include <vector>
#include <irrlicht.h>
#include "../objects/DynamicObject.h" //This is for the template class to assign them shaders

using namespace std; // for the vector class
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

	void setMaterials(ISceneNode * node, vector<DynamicObject::DynamicObject_material> mat);
};

#endif
