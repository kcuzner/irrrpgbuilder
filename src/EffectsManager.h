#ifndef EFFECTSMANAGER_H
#define EFFECTSMANAGER_H


class EffectsManager
{
    public:
        static EffectsManager* getInstance();

        void setWeather(float fogFactor, float rainFactor);
        void setTimeOfDay(int newTime);

        virtual ~EffectsManager();
    protected:
    private:
        EffectsManager();
};

#endif // EFFECTSMANAGER_H
