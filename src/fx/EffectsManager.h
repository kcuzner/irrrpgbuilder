#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H

#include "PostProcess/PostProcessManager.h"

class EffectsManager
{
    public:
        static EffectsManager* getInstance();

        void setWeather(float fogFactor, float rainFactor);
        void setTimeOfDay(int newTime);
		void update();
		void preparePostFX(bool depth);

        virtual ~EffectsManager();
    protected:
    private:
        EffectsManager();
		int postProcessMode;
		CPostProcessManager* postProcessManager;
		E_POSTPROCESS_EFFECT sampleEffect;
};

#endif // EFFECTSMANAGER_H
