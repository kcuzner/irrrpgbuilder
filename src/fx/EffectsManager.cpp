#include "EffectsManager.h"
#include <iostream>
#include "postprocess/PostProcessManager.h"

#include "../App.h"
#include "../CameraSystem.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

EffectsManager::EffectsManager()
{
	// Init the postProcess FX manager
	postProcessManager = new CPostProcessManager(App::getInstance()->getDevice());
	if (postProcessManager == 0)
		printf ("Success initialized the postprocess manager.\n");
	//postProcessManager->prepare(false);
	postProcessMode = 0;

    //get main app scenemanager pointer
    ISceneManager* smgr=App::getInstance()->getDevice()->getSceneManager();
    //this is the main particle system, it is used to rain and snow effects
    mainParticleSystem = smgr->addParticleSystemSceneNode(false,//use default emitter
                                                          CameraSystem::getInstance()->getNode(),
                                                          -1,
                                                          vector3df(0,0,250));

    f32 emitterSize = 500;
    emitter = mainParticleSystem->createBoxEmitter(
                                         aabbox3df(vector3df(-emitterSize,-1,-emitterSize),vector3df(emitterSize,1,emitterSize)),
                                         vector3df(0,-1,0.5),
                                         5,
                                         10,
                                         SColor(255,0,0,0),
                                         SColor(255,255,255,255),
                                         5000,
                                         10000);


    mainParticleSystem->setEmitter(emitter);
    mainParticleSystem->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);

    //default particle texture -> media/rain.png
    //mainParticleSystem->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture("../media/rain.png"));

    mainParticleSystem->setVisible(false);
}

EffectsManager::~EffectsManager()
{
	delete postProcessManager;

    emitter->drop();
}

EffectsManager* EffectsManager::getInstance()
{
    static EffectsManager *instance = 0;
    if (!instance) instance = new EffectsManager();
    return instance;
}

void EffectsManager::setWeather(int maxParticles, float particlesSpeed, stringc textureFile="")
{
    //hide effect when user sets zero particles
    if(maxParticles == 0)
        mainParticleSystem->setVisible(false);
    else
        mainParticleSystem->setVisible(true);

    emitter->setMaxParticlesPerSecond(maxParticles);
    emitter->setMinParticlesPerSecond(maxParticles);
    emitter->setDirection(vector3df(0,-particlesSpeed,particlesSpeed*0.5));

    if(textureFile!="")
        mainParticleSystem->setMaterialTexture(0,App::getInstance()->getDevice()->getVideoDriver()->getTexture(textureFile));
}

///TODO: define this function in Lua by setting AmbinetLight(COLOR_NAME)
void EffectsManager::setTimeOfDay(int newTime)
{
    newTime = newTime%24;

    int r,g,b;

    switch(newTime)
    {
        case 0:
            r=19;
            g=14;
            b=134;
            break;
        case 1:
            r=31;
            g=42;
            b=134;
            break;
        case 2:
            r=43;
            g=69;
            b=134;
            break;
        case 3:
            r=54;
            g=97;
            b=134;
            break;
        case 4:
            r=66;
            g=125;
            b=134;
            break;
        case 5:
            r=78;
            g=153;
            b=134;
            break;
        case 6:
            r=94;
            g=177;
            b=139;
            break;
        case 7:
            r=121;
            g=190;
            b=158;
            break;
        case 8:
            r=148;
            g=203;
            b=178;
            break;
        case 9:
            r=175;
            g=216;
            b=197;
            break;
        case 10:
            r=201;
            g=229;
            b=216;
            break;
        case 11:
            r=228;
            g=242;
            b=236;
            break;
        case 12:
            r=255;
            g=255;
            b=255;
            break;
        case 13:
            r=228;
            g=246;
            b=225;
            break;
        case 14:
            r=255;
            g=237;
            b=194;
            break;
        case 15:
            r=228;
            g=227;
            b=167;
            break;
        case 16:
            r=255;
            g=218;
            b=134;
            break;
        case 17:
            r=228;
            g=209;
            b=103;
            break;
        case 18:
            r=255;
            g=199;
            b=73;
            break;
        case 19:
            r=226;
            g=173;
            b=73;
            break;
        case 20:
            r=187;
            g=142;
            b=84;
            break;
        case 21:
            r=148;
            g=110;
            b=95;
            break;
        case 22:
            r=110;
            g=79;
            b=106;
            break;
        case 23:
            r=71;
            g=47;
            b=118;
            break;
    }

    App::getInstance()->getDevice()->getSceneManager()->setAmbientLight(SColorf(r/255.0f,g/255.0f,b/255.0f,1));
}

void EffectsManager::update()
{
	// Update FX's
	// Will update the postprocess shader. Need to be called for every frame
	// There is also a "prepare function" needed to be called before rendering all the scene.
	// This is all in the "UPDATE" function in the APP class.

	// To check the framework, just set the postProcessMode to 1.
	//postProcessMode=1;
	if (postProcessMode>0)
	{
		//postProcessManager->render(EPPE_ADAPTIVE_BLOOM);
		postProcessManager->render(postEffect);
		postProcessManager->update();
	}
}

void EffectsManager::preparePostFX(bool depth)
{
	if (postProcessMode>0)
		postProcessManager->prepare(false);

}

void EffectsManager::DOFaddObject(ISceneNode * DOFNode)
{
	postProcessManager->addNodeToDepthPass(DOFNode);
}

void EffectsManager::DOFclearObjects()
{
	postProcessManager->clearDepthPass();
}

void EffectsManager::SetPostFX(stringc fxname)
{
	if (fxname=="invert")
	{
		postEffect = EPPE_INVERT;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="sepia")
	{
		postEffect = EPPE_SEPIA;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="simple_bloom")
	{
		postEffect = EPPE_SIMPLE_BLOOM;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="adaptive_bloom")
	{
		postEffect = EPPE_ADAPTIVE_BLOOM;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="motion_blur")
	{
		postEffect = EPPE_MOTION_BLUR;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="night_vision")
	{
		postEffect = EPPE_NIGHT_VISION;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="dream_vision")
	{
		postEffect = EPPE_DREAM_VISION;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="posterize")
	{
		postEffect = EPPE_POSTERIZE;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="sharpen")
	{
		postEffect = EPPE_SHARPEN;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="embossed")
	{
		postEffect = EPPE_EMBOSSED;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="tiling")
	{
		postEffect = EPPE_TILING;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="displacement")
	{
		postEffect = EPPE_DISPLACEMENT;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="scratched")
	{
		postEffect = EPPE_SCRATCHED;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="pencil")
	{
		postEffect = EPPE_PENCIL;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="blur")
	{
		postEffect = EPPE_BLUR;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="water")
	{
		postEffect = EPPE_WATER;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="color")
	{
		postEffect = EPPE_COLOR;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="pulsing")
	{
		postEffect = EPPE_PULSING;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="shake")
	{
		postEffect = EPPE_SHAKE;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="desaturate")
	{
		postEffect = EPPE_DESATURATE;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="radial_blur")
	{
		postEffect = EPPE_RADIAL_BLUR;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="depth_of_field")
	{
		postEffect = EPPE_DEPTH_OF_FIELD;
		postProcessMode = 1;
		return;
	} else
	if (fxname=="vignette")
	{
		postEffect = EPPE_VIGNETTE;
		postProcessMode = 1;
		return;
	} else
	postProcessMode = 0;
}
