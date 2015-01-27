#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <irrlicht.h>
#include <vector>
#include "projectileItem.h" // Access to projectile datas

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

using namespace std;

class Projectile : public projectileitem{
	public:
		
		// Functions and commands
		void update();
		static Projectile* getInstance();

		void createProjectile(projectileitem item); //Create a projectile for the manager
		projectileitem reset(projectileitem data); //Helper function to reset the data in the reference projectile data
		projectileitem setProjectileRepresentation(projectileitem data, core::stringc filename, bool mesh, f32 size=0); //Helper function to define the object representation
		projectileitem setProjectileTrajectory(projectileitem data, vector3df startpos, vector3df angle, f32 range, f32 velocity); //Find the endpoint for the projectile
		projectileitem setProjectileTrajectory(projectileitem data, vector3df startpos, vector3df endpos, f32 velocity); //Find the endpoint for the projectile
		vector3df rayTest(vector3df pos, vector3df pos1); // Check for a ray test from point 1 to point 2 (utility)

		// Data and variables
		vector<projectileitem> projectiles;

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