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

		void initPostProcess();
        void setWeather(int maxParticles, float particlesSpeed, stringc textureFile);
        void setTimeOfDay(int newTime);
		void update();
		void preparePostFX(bool depth);
		void SetPostFX(stringc fxname);
		void DOFaddObject(ISceneNode * DOFNode);
		void DOFclearObjects();

		inline void skydomeVisible(bool visible) {skydomestatus=visible;}
		inline void updateSkydome() {skydome->setVisible(skydomestatus);}
		inline void turnOffSkydome() {skydome->setVisible(false);}
		void skydomeTexture(core::stringc file); 
		void setBackgroundColor(const video::SColor color);

		

        virtual ~EffectsManager();
    protected:
    private:
        EffectsManager();
		int postProcessMode;
		CPostProcessManager* postProcessManager;
		E_POSTPROCESS_EFFECT postEffect;

		bool skydomestatus;

		IParticleSystemSceneNode* mainParticleSystem;
		IParticleBoxEmitter* emitter;

		scene::ISceneNode* skydome;
		scene::ISceneNode* skybox;

		video::IVideoDriver* driver;
};

#endif // EFFECTSMANAGER_H
