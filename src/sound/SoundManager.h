#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <irrKlang.h>
#include <iostream>

using namespace irrklang;

class SoundManager
{
    public:
        static SoundManager* getInstance();

        ISound* playSound2D(std::string file, bool looped = false);

        ISound* playSound3D(std::string file, vec3df pos, bool looped = false);

        void setListenerPosition(vec3df pos, vec3df lookDir);

        //Master Volume, affects all sounds
        void setVolume(float volume);

        void stopSounds();

		void stopEngine();

        virtual ~SoundManager();
    protected:
    private:
        SoundManager();

        ISoundEngine* engine;
};

#endif // SOUNDMANAGER_H
