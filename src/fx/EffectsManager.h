#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H

#include <irrlicht.h>
#include "XEffects/XEffects.h"
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

		inline void skydomeVisible(bool visible) {skydomestatus=visible; skydome->setVisible(visible);}
		inline void updateSkydome() {skydome->setVisible(skydomestatus);}
		inline void turnOffSkydome() {skydome->setVisible(false);}
		void skydomeTexture(core::stringc file); 
		void setBackgroundColor(const video::SColor color);

		//Add commands from XEffect available here
		inline void addShadowToNode(ISceneNode* node) {effect->addShadowToNode(node);}
		inline void addShadowLight(vector3df position, vector3df target, SColor color) {
			effect->addShadowLight(SShadowLight(4096, vector3df(35, 480, -50), vector3df(0, 0, 0), SColor(255, 255, 255, 255), 20.0f, 5000.0f, 220000.0f * DEGTORAD, true));}
		
		inline void removeShadowFromNode(ISceneNode* node) {effect->removeShadowFromNode(node);}
	
		inline void setClearColor(SColor color) {effect->setClearColour(color);}
		

		inline void enableXEffects(bool enable) {xeffects = enable;}
		inline bool isXEffectsEnabled() {return xeffects;}


		

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
		EffectHandler* effect;
		bool xeffects;
};

#endif // EFFECTSMANAGER_H
