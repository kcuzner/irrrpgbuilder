#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <irrlicht.h>
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

//! This data class is used for the dynamic objects and the projectiles
// Might be also used for other types of object.
// It's inherited from the the dynamic object class and the projectile data class
class cproperty{
public:
	u32 life;
	u32 mana;
	u32 maxlife;
	u32 maxmana;
	u32 regenlife;
	u32 regenmana;
	u32 money;
	u32 level;
	u32 experience;
	u32 mindamage;
	u32 maxdamage;
	u32 armor;
	u32 magic_armor;
	u32 hurt_resist;
	u32 dotduration;
	f32 hit_prob;
	f32 dodge_prop;
	u32 attackdelay;
	u32 mindefense;
	u32 maxdefense;
	u32 weight;
	u32 maxweight;
	u32 currentweight;
};


#endif
