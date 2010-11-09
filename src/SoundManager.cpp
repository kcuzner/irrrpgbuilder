#include "SoundManager.h"

using namespace irrklang;

SoundManager::SoundManager()
{
    engine = createIrrKlangDevice();
}

SoundManager::~SoundManager()
{
    //dtor
}

SoundManager* SoundManager::getInstance()
{
    static SoundManager* instance = 0;
    if(instance == 0) instance = new SoundManager();
    return instance;
}

ISound* SoundManager::playSound2D(std::string file, bool looped)
{
    return engine->play2D(file.c_str(),looped);
}

void SoundManager::playSound3D(std::string file, vec3df pos, bool looped)
{
    ISound* s = engine->play3D(file.c_str(), pos, looped);
    if(s)
    {
        s->setMinDistance(10);
        s->setPosition(pos);
    }
}

void SoundManager::setListenerPosition(vec3df pos, vec3df lookDir)
{
    engine->setListenerPosition(pos,lookDir);
}

void SoundManager::setVolume(float volume)
{
    engine->setSoundVolume(volume);
}

void SoundManager::stopSounds()
{
    engine->stopAllSounds();
}

void SoundManager::stopEngine()
{
	engine->drop();
}
