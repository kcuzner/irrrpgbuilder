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
	if (engine)
		return engine->play2D(file.c_str(),looped);
	else
		return NULL;
}

ISound* SoundManager::playSound3D(std::string file, vec3df pos, bool looped)
{
	if (engine)
	{
		ISound* s = engine->play3D(file.c_str(), pos, looped);
		if(s)
		{
			s->setMinDistance(150);
			s->setPosition(pos);
			return s;
		}
	}
	return 0;
}

void SoundManager::setListenerPosition(vec3df pos, vec3df lookDir)
{
	if (engine)
    engine->setListenerPosition(pos,lookDir);
}

void SoundManager::setVolume(float volume)
{
	if (engine)
    engine->setSoundVolume(volume);
}

void SoundManager::stopSounds()
{
	if (engine)
    engine->stopAllSounds();
}

void SoundManager::stopEngine()
{
	if (engine)
	engine->drop();
}
