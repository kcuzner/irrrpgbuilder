#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <irrlicht.h>
#include "../App.h" //  to be able to communicate with this class
#include "DynamicObject.h"// to be able to communicate with this class
#include <vector>


using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class Projectile
{
	public:
		
		//This structure has all the information needed for a projectile
		struct itemdata
		{
			irr::u32 id;        // A id 
			bool alive;         // A check to see when the projectile is no longer needed
			ISceneNode* node;   // Representation of the object (mesh or billboard)
			vector3df position; // Start position of the projectile
			vector3df rotation; // Start angle of the projectile
			vector3df endposition; //End position of the projectile
			f32 velocity;

		};

		// Functions and commands
		void update();
		static Projectile* getInstance();

		void createProjectile(itemdata item); //Create a projectile for the manager
		itemdata reset(itemdata data); //Helper function to reset the data in the reference projectile data
		itemdata setProjectileRepresentation(itemdata data, core::stringc filename, bool mesh, f32 size=0); //Helper function to define the object representation
		itemdata setProjectileTrajectory(itemdata data, vector3df startpos, vector3df angle, f32 range, f32 velocity); //Find the endpoint for the projectile
		itemdata setProjectileTrajectory(itemdata data, vector3df startpos, vector3df endpos, f32 velocity); //Find the endpoint for the projectile
		vector3df rayTest(vector3df pos, vector3df pos1); // Check for a ray test from point 1 to point 2 (utility)

		// Data and variables
		vector<itemdata> projectiles;

	private:
		Projectile();
		virtual ~Projectile();

		void updateMovement(ISceneNode* node, f32 velocity);

		//To access Irrlicht objects
		IrrlichtDevice *device;
        IVideoDriver* driver;
        ISceneManager* smgr;
        IGUIEnvironment* guienv;
		ISceneCollisionManager* collman;
		ISceneNode* collisionnode;

		u32 projectilecounter;

};
#endif //PROJECTILE_H