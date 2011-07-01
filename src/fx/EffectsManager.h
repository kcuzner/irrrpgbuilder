#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H

#include <irrlicht.h>
#include "postprocess/PostProcessManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class EffectsManager
{
    public:
        static EffectsManager* getInstance();

        void setWeather(int maxParticles, float particlesSpeed, stringc textureFile);
        void setTimeOfDay(int newTime);
		void update();
		void preparePostFX(bool depth);
		void SetPostFX(stringc fxname);
		void DOFaddObject(ISceneNode * DOFNode);
		void DOFclearObjects();

        virtual ~EffectsManager();
    protected:
    private:
        EffectsManager();
		int postProcessMode;
		CPostProcessManager* postProcessManager;
		E_POSTPROCESS_EFFECT postEffect;

		IParticleSystemSceneNode* mainParticleSystem;
		IParticleBoxEmitter* emitter;
};

#endif // EFFECTSMANAGER_H
