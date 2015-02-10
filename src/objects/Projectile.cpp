#include "Projectile.h"
#include "../App.h" //  to be able to communicate with this class
#include "DynamicObject.h" // to be able to communicate with this class

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

Projectile::Projectile()
{
	projectiles.clear();
	projectilecounter=0;

	//Get all needed data from the application class
	this->device=App::getInstance()->getDevice();
	this->driver=App::getInstance()->getDevice()->getVideoDriver();
	this->smgr=App::getInstance()->getDevice()->getSceneManager();
	this->guienv=App::getInstance()->getDevice()->getGUIEnvironment();
	this->collman=App::getInstance()->getDevice()->getSceneManager()->getSceneCollisionManager();

	this->collisionnode=NULL;

}

Projectile::~Projectile()
{
}

//Get instance keep a pointer reference on it's own object
//Easier to get back the pointer of the class
//As this class manage all the projectiles
//Used like in a singleton
Projectile* Projectile::getInstance()
{
    static Projectile *instance = 0;
    if (!instance) instance = new Projectile();
    return instance;
}

//Update the movement of the projectile based on delta time
void Projectile::updateMovement(ISceneNode* node, f32 velocity)
{
	if (!node)
		return;

	//The "DELTA" is the difference in MS between the last and current frame render.
	//The delta offset will get the velocity per second and return the proper distance with the current render time
	//If the FPS is low, the result will be a longer distance
	//If the FPS is high, the result will be a smaller distance (smoother)
	//The objective is to have the same distance travelled, non dependant on the rendering performance
	f32 deltaoffset = (velocity*(f32)App::getInstance()->getDelta())/1000; //(based on seconds (1000 for 1000ms)

	vector3df pos=node->getPosition();
    pos.Z -= cos((node->getRotation().Y)*PI/180)*deltaoffset;
	pos.X -= sin((node->getRotation().Y)*PI/180)*deltaoffset;

	node->setPosition(pos);
	node->updateAbsolutePosition();
}

//! Update all of the projectiles
void Projectile::update()
{
	for(u32 i=0; i<projectiles.size(); i++)
	{
		//Get the projectile informations and update the trajectory
		ISceneNode* node = projectiles[i].node;
		f32 vel = projectiles[i].velocity;

		if (projectiles[i].node->getPosition()==vector3df(0,0,0))
			printf("Bugged coords for the projectile, it should take the position of the object and it has none!!!\n");

		this->updateMovement(node, vel);

		//Test if the distance of the projectile reach the endpoint then disable the projectile
		if (projectiles[i].node->getAbsolutePosition().getDistanceFrom(projectiles[i].endposition)<1.0f)
		{
			projectiles[i].alive=false;
		}

		//If the projectile lifetime is completed, then remove the object and the projectile data.
		if (!projectiles[i].alive)
		{
			if (projectiles[i].node)
				projectiles[i].node->remove();
			projectiles.erase(projectiles.begin()+i);
			//Would need to return something to the dynamic object class to tell where the projectile stopped
			//So it could define the position to had a FX
			//Also need to check if it touched a node
			//And report for the damage
		}
	}
}

//! Will create a new projectile for the manager
void Projectile::createProjectile(projectileitem item)
{
	//item=data1; //check if we can keep data in
	item.alive = true;

	//Item cloning is required
	if (!item.node)
		return;

	ISceneNode* clone = item.node->clone();
	item.node = clone;
	item.node->setName("projectile");
	item.node->setPosition(item.position);

	//printf("Here is the data for this projectile: pos=%f,%f,%f\n",item.position.X,item.position.Y,item.position.Z);
	//printf("Here is the data for this projectile: endpos=%f,%f,%f\n",item.endposition.X,item.endposition.Y,item.endposition.Z);

	if (!item.node)
	{
		printf("A projectile failed to be created, node is not created!\n");
		return;
	}

	//Do a check to see if the projectile data is created correctly
	if (item.node && item.alive && item.velocity>0.0f)
	{
		item.id = projectilecounter+1; //Add another id to each instanced projectile
		if (!item.node->isVisible()) //Put back the node visible once it used by the manager
				item.node->setVisible(true);
		item.node->setPosition(item.position);
		item.node->setRotation(item.rotation);
		projectiles.push_back(item);


		//printf("A projectile has been created!\n");
	} else
	{
		printf("A projectile failed to be created, ");
		if (!item.node)
			printf("no node was created!\n");
		if (!item.alive)
			printf("The created projectile is already dead!\n");
		if (item.velocity==0.0f)
			printf("no velocity!\n");
	}

}

//! Helper function. Reset the projectile data. Should be used prior to define a new projectile
void Projectile::reset(projectileitem& data)
{
	data.alive = true;
	data.id = 0;
	data.node = NULL;
	data.position = vector3df(0.0f,0.0f,0.0f);
	data.rotation = vector3df(0.0f,0.0f,0.0f);
	data.endposition = vector3df(0.0f,0.0f,0.0f);
	data.velocity = 0.0f;
}

//! Helper function, will add a projectile representation to the projectile data
void Projectile::setProjectileRepresentation(projectileitem& data, irr::core::stringc filename, bool usemesh, irr::f32 size)
{
	IAnimatedMesh* mesh=NULL;
	IBillboardSceneNode* bill=NULL;
	ISceneNode* node=NULL;

	if (usemesh)
	{
		mesh = smgr->getMesh(filename.c_str());
		node = smgr->addAnimatedMeshSceneNode(mesh,0,0x0010);
		if (node)
		{
			if (size>0) // User defined a size, so it mean that it want this projectile scaled
				node->setScale(vector3df(size,size,size));

			node->setVisible(false); // The node has been created, but not yet used. Hide it for the moment.
			data.node = node;
		}

	} else
	{
		if (size==0) // If the size is not provided, use the default size
			size=10.0f;

		bill = smgr->addBillboardSceneNode(0,core::dimension2d<f32>(size,size));
		bill->setMaterialFlag(video::EMF_LIGHTING, false);
		bill->setMaterialTexture(0, driver->getTexture(filename.c_str()));
		bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		bill->setVisible(false); // The node has been created, but not yet used. Hide it for the moment.

		if (bill)
			data.node = (ISceneNode*)bill;
		else
			data.node = NULL;


	}

}

// Set the trajectory of the projectile into the data
void Projectile::setProjectileTrajectory(projectileitem& data, irr::core::vector3df startpos, irr::core::vector3df angle, irr::f32 range, f32 velocity)
{
	data.position = startpos;
	data.rotation = angle;
	data.velocity = velocity;

	//Front of the player 30 units in front
	vector3df tas = vector3df (0,0,-range); //-Z Axis
	tas.rotateXZBy(-angle.Y);

	//put the result back in the data
	data.endposition = startpos+tas;

	// Do a raytest to check if the ray find anything. If it does then the endpoint will stop when it touch.
	vector3df testpos = rayTest(data.position,data.endposition);
	if (this->collisionnode && testpos!=vector3df(-1000.0f,-1000.0f,-1000.0f))
	{
		printf("The projectile collided with %s! end position is: %f,%f,%f\n",stringc(collisionnode->getName()).c_str(),testpos.X,testpos.Y,testpos.Z);
		printf("Old end Position is: %f,%f,%f\n",data.endposition.X,data.endposition.Y,data.endposition.Z);
		data.endposition = testpos;
	}

}

// Set the trajectory of the projectile into the data
// The user know the endposition but a collision test is done to be sure it reach
// If something get between the target it will stop the projectile
void Projectile::setProjectileTrajectory(projectileitem& data, irr::core::vector3df startpos, irr::core::vector3df endpos, f32 velocity)
{
	data.endposition = endpos;
	data.velocity = velocity;

	// Do a raytest to check if the ray find anything. If it does then the endpoint will stop when it touch.
	vector3df testpos = rayTest(data.position,data.endposition);
	if (this->collisionnode && testpos!=vector3df(-1000.0f,-1000.0f,-1000.0f))
		data.endposition = testpos;

}

vector3df Projectile::rayTest(vector3df pos, vector3df pos1)
{

	//Create a ray from the function input
	core::line3d<f32> ray;
    ray.start = pos;
	ray.end = pos1;
	this->collisionnode = NULL;

	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
    // Used to show with triangle has been hit. Required but not used in IRB
    core::triangle3df hitTriangle;


	scene::ISceneNode * selectedSceneNode =
    collman->getSceneNodeAndCollisionPointFromRay(
		ray,
		intersection,
		hitTriangle,
		100, //100 is the default ID for walkable (ground obj + props)
		0); // Check the entire scene (this is actually the implicit default)

	this->collisionnode=selectedSceneNode;

	if (selectedSceneNode)
	{
		return intersection;
	}
	else
		// if not return 0
		return vector3df(-1000.0f,-1000.0f,-1000.0f); //Ray did not touch anything
}


