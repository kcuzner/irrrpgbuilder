#ifndef PROJECTILEITEM_H
#define PROJECTILEITEM_H

#include <irrlicht.h>
#include <vector>

#include "properties.h"
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class projectileitem : public cproperty{
	public:
		irr::u32 id;        // A id 
		bool alive;         // A check to see when the projectile is no longer needed
		ISceneNode* node;   // Representation of the object (mesh or billboard)
		vector3df position; // Start position of the projectile
		vector3df rotation; // Start angle of the projectile
		vector3df endposition; //End position of the projectile
		f32 velocity;
		cproperty properties; //data properties assigned to the projectiles


};
#endif