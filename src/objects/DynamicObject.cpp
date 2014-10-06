// IRR RPG BUILDER
// DYNAMIC OBJECT CLASS
//
//
//
#include "../App.h"
#include "DynamicObjectsManager.h"
#include "combat.h"
//#include "../LuaGlobalCaller.h"
#include "Player.h"
#include "../terrain/TerrainManager.h"
#include "../camera/CameraSystem.h"

#include "DynamicObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


DynamicObject::DynamicObject(irr::core::stringc name, irr::core::stringc meshFile, vector<DynamicObject_Animation> animations, bool directpath)
{
	// This is done when an dynamic object is initialised (template && player)
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

	properties=initProperties();
	prop_base=initProperties();
	prop_level=initProperties();

	stringc realFile = "../media/dynamic_objects/";
	realFile += meshFile;   

	fileName=meshFile; // Store the original filename of the dynamic object for verification
	thumbnail=""; //Store the filename for the thumbnail. Used in the GUI Manager. Thumbnail filename come from the template definition
	description=L""; //Store the description of the object.

	error=false;
	//printf("Here is the object: %s \n",realFile.c_str());

	mesh = NULL;
	Tmesh = NULL;
	//Attempt to create different meshbuffer when getting instances from the meshbuffer.
	bool result=smgr->getMeshCache()->isMeshLoaded(realFile);
	if (result)
	{
		IAnimatedMesh* oldmesh = smgr->getMeshCache()->getMeshByName(realFile);
		
		if (oldmesh->getFrameCount()>1) //Will only rename animated models
		{
			stringc newname = DynamicObjectsManager::getInstance()->createUniqueName(this->getType());
			smgr->getMeshCache()->renameMesh(oldmesh,newname);
			printf("This mesh %s is already loaded and in the mesh cache\n",realFile.c_str());
		}
	}

	//Report if the file is not found
	if (!App::getInstance()->getDevice()->getFileSystem()->existFile(realFile.c_str()))
	{
		//Put the information in the logger (debugging purpose)
		core::stringc log = "Model was not found: ";
		log.append(realFile.c_str());
		log.append("!!!");
		App::getInstance()->getDevice()->getLogger()->log(log.c_str());
		GUIManager::getInstance()->setConsoleText((core::stringw)log,video::SColor(255,200,0,0));
	}


	if (!directpath)
		mesh = smgr->getMesh(realFile); //Loading from the Dynamic objects template library
										//Objects in the library should be stored in the
										//" media/dynamic_objects" folder in IRB
	else
		mesh = smgr->getMesh(meshFile); //Loading the file directly from a path (directpath)
										// Object can be stored anywhere
										// Used mostly for testing models

	
	//meshName = meshFile;
    this->animations = animations;

	// Check if mesh is really valid
	// if not, then use the "error" mesh as temp to tell the user the object is not ok.
	if (!mesh)
	{
		mesh = smgr->getMesh("../media/editor/error.obj");
		error=true;
	}

	//Try to create a version of the mesh with the tangents for the normal mapping
	Tmesh = smgr->getMeshManipulator()->createMeshWithTangents(mesh);
	
	setupObj(name, mesh);

	// When enabled, the LUA will update even if the node is culled.
	this->nodeLuaCulling = false;
	this->templateobject = false;
	lastframe=0;
	enabled=true;
	enemyUnderAttack=NULL;
	namecollide="";

	diePresent=true;
	despawnPresent = true;
	runningMode = false;
	soundActivated.push_back(false);
	attackActivated.push_back(false);
	stunstate=false;
	attackdelaystate=false;
	reached=false;
	rotationupdater=false;

	isEnemy=false;
	isInBag=false;
	isDestroyedAfterUse=false;	//Default value, mostly used for consumable. For scrolls, key, and other "resellable", will need to be set to false;
	isGenerated=false; //Default state, was generated inside the editor and not by LUA. LUA object must be removed after the game is complete. (STOPGAME)

	attackresult=0;
	originalscale=vector3df(1.0f,1.0f,1.0f);
	lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();

	timerAnimation = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timerLUA = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timer_attackdelay = App::getInstance()->getDevice()->getTimer()->getRealTime();
	// Set the animation and AI state to idle (default)
	this->setAnimation("idle");
	this->walkTarget=this->getPosition();
	this->AI_State=AI_STATE_IDLE;
	// Init the timedelay taken for a loop
	lastTime=0;
	soundfx = NULL;

	smgr = App::getInstance()->getDevice()->getSceneManager();
	driver = App::getInstance()->getDevice()->getVideoDriver();
}

DynamicObject::DynamicObject(stringc name, IAnimatedMesh* mesh, vector<DynamicObject_Animation> animations)
{

	smgr = App::getInstance()->getDevice()->getSceneManager();
	driver = App::getInstance()->getDevice()->getVideoDriver();

	// This is done when a new character is created from the template
	properties=initProperties();
	prop_base=initProperties();
	prop_level=initProperties();

    this->animations = animations;

    setupObj(name, mesh);

	enemyUnderAttack=NULL;
	oldparent = smgr->getRootSceneNode(); //Initial "old parent" (Normally for loot object that are parented to the player)

	// initialize the timers
	timerAnimation = App::getInstance()->getDevice()->getTimer()->getRealTime();
	timerLUA = timerAnimation;
	timer_display = timerAnimation;

	// Flags initialisation
	diePresent=true;
	despawnPresent = true;
	runningMode = false;
	stunstate=false;
	attackdelaystate=false;

	oldpos=vector3df(0,0,0);

	currentAnimation=OBJECT_ANIMATION_CUSTOM;
	oldAnimation=OBJECT_ANIMATION_CUSTOM;
	this->setAnimation("prespawn");
	lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();

}

DynamicObject::~DynamicObject()
{
	if (selector)
		selector->drop();

	if (Healthbar)
		Healthbar->remove();

	if (node)
		node->remove();

//	if (fakeShadow)
//		fakeShadow->drop();


}

DynamicObject::cproperty DynamicObject::initProperties()
{
	DynamicObject::cproperty prop;
	// Initialize the property
	prop.armor=0;
	prop.dodge_prop=0;
	prop.dotduration=0;
	prop.experience=0;
	prop.hurt_resist=0;
	prop.hit_prob=0;
	prop.level=0;
	prop.life=100;
	prop.magic_armor=0;
	prop.mana=0;
	prop.maxdamage=0;
	prop.maxdefense=0;
	prop.maxlife=0;
	prop.maxmana=0;
	prop.mindamage=0;
	prop.mindefense=0;
	prop.attackdelay=2000; //wait 1 second after the initial attack (default)
	prop.money=0;
	prop.regenlife=0;
	prop.regenmana=0;

	return prop;

}

void DynamicObject::setupObj(stringc name, IAnimatedMesh* mesh)
{
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

    this->mesh = mesh;
    this->name = name;

	displayName=(core::stringw)name;
	internalname=displayName;

	//initialise stuff
	selector=NULL;
	node=NULL;

	if(hasAnimation() && mesh && mesh->getFrameCount()>1)
	{
		this->mesh->setHardwareMappingHint(EHM_DYNAMIC);

        nodeAnim = smgr->addAnimatedMeshSceneNode(mesh,0,0x0010);
		this->node = nodeAnim;
		if (node)
		{
			this->selector = smgr->createTriangleSelector(mesh, node);
			this->node->setTriangleSelector(selector);
		}

	}
    else
	{
		this->mesh->setHardwareMappingHint(EHM_STATIC);
		//Keep this for reference.
        //this->node = smgr->addMeshSceneNode((IAnimatedMesh*)mesh,0,0x0010);

		// Check the size of the mesh and create it as an octree if it's big. (200 unit min)
		if (mesh->getBoundingBox().getExtent().X>200 && mesh->getBoundingBox().getExtent().Y>200 && mesh->getBoundingBox().getExtent().Z>200)
			this->node = smgr->addOctreeSceneNode (Tmesh,0,100);
		else
			this->node = smgr->addMeshSceneNode(Tmesh,0,100);
		if (node)
		{
			// Select the triangle selector for more precision
			this->selector = smgr->createTriangleSelector((IMesh*)Tmesh,node);
			//this->selector = smgr->createOctreeTriangleSelector((IMesh*)mesh, node);
			//this->selector = smgr->createTriangleSelectorFromBoundingBox(node);
			this->node->setTriangleSelector(selector);
		}
	}
	if (!node) //If a node fail to create. Create this to help find the problem
	{
		node=smgr->addEmptySceneNode();
		GUIManager::getInstance()->setConsoleText(L"Failed to create a node!");
		//printf ("Failed to create a node! Node name should be: %s\n",name.c_str());
	}

	if (node)
	{
		// Setup the animations
		this->animator = NULL;

		node->setName(name);

		f32 meshSize = this->getNode()->getBoundingBox().getExtent().Y;
	    f32 meshScale = this->getNode()->getScale().Y;

		if (objectType != OBJECT_TYPE_EDITOR)
		{
			// Editor objects don't have the fake shadow.
			//node->setDebugDataVisible(EDS_BBOX | EDS_SKELETON);
			//Fake Shadow
			fakeShadow = smgr->addMeshSceneNode(smgr->getMesh("../media/dynamic_objects/shadow.obj"),node);
			fakeShadow->setScale(vector3df(0.75f,0.75f,0.75f));
			fakeShadow->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL);
			fakeShadow->setMaterialFlag(EMF_BLEND_OPERATION,true);
			fakeShadow->setPosition(vector3df(0,0.03f + (rand()%5)*0.01f ,0));

			fakeShadow->setMaterialFlag(EMF_FOG_ENABLE,true);

			// This set the frameloop to the static pose, we could use a flag if the user decided this
			//if(hasAnimation()) this->setFrameLoop(0,0);
		}

		//printf ("Scaling for node: %s, is meshSize %f, meshScale: %f, final scale: %f\n",this->getName().c_str(),meshSize,meshScale,meshSize*meshScale);
		script = "";
		this->setEnabled(true);
		node->setMaterialFlag(EMF_FOG_ENABLE,true);
		node->setMaterialFlag(EMF_LIGHTING, true);
		node->setMaterialFlag(EMF_ANTI_ALIASING,true);

		objLabel = smgr->addTextSceneNode(GUIManager::getInstance()->getFont(GUIManager::FONT_ARIAL),L"",SColor(255,255,255,0),node,vector3df(0,meshSize*meshScale*1.1f,0));
		objLabel->setVisible(false);
		scene::ISceneCollisionManager* coll = smgr->getSceneCollisionManager();
		Healthbar = new scene::HealthSceneNode(this->node,smgr,-1,coll,50,5,vector3df(0,meshSize*meshScale*1.05f,0),video::SColor(255,192,0,0),video::SColor(255,0,0,0),video::SColor(255,128,128,128));
		Healthbar->setVisible(false);

		// Set the object animation as prespawn for the NPC`s			setAnimation("prespawn");
		//node->setDebugDataVisible(EDS_BBOX_ALL);

	} 
		
}

DynamicObject* DynamicObject::clone()
{
    DynamicObject* newObj = new DynamicObject(name,mesh,animations);

	if (!error)
		newObj->setScale(this->getScale());
    newObj->setMaterialType(this->getMaterialType());
	newObj->setType(typeText);
    newObj->templateObjectName = this->templateObjectName;///TODO: scale and material can be protected too, then we does not need get and set for them.
	newObj->setTemplate(false);
	// use a temporary state to define animation, will set the idle animation, but with a random initial frame.
	
	fakeShadow->setVisible(false);
	// Preset telling the die animation is present (will be tested for this)
    return newObj;
}
//-----------------------------------------------------------------------
// Return node info
//-----------------------------------------------------------------------
ISceneNode* DynamicObject::getNode()
{
    return node;
}

ISceneNode* DynamicObject::getShadow()
{
	return fakeShadow;
}

stringc DynamicObject::getTemplateObjectName()
{
    return templateObjectName;
}

void DynamicObject::setTemplateObjectName(stringc newName)
{
    templateObjectName = newName;
}

//-----------------------------------------------------------------------
// Character Movement
//-----------------------------------------------------------------------
void DynamicObject::lookAt(vector3df pos)
{
    vector3df offsetVector = pos - node->getPosition();

    vector3df rot = (-offsetVector).getHorizontalAngle();

	if (rot.Y>361)
		rot.Y=rot.Y-360;

	if (rot.Y<-360)
		rot.Y=rot.Y+360;

    rot.X=0;
    rot.Z=0;

	// Will rotate the node only if it still "alive"
	if (properties.life>0)
		node->setRotation(rot);
}

void DynamicObject::rotateObject(core::vector3df from, core::vector3df to, u32 time)
{
	rotationcounter = App::getInstance()->getDevice()->getTimer()->getRealTime();
	rotationupdater=true;
	rotfrom=from;
	rotto=to;
	rotationtime=time;
}

void DynamicObject::setPosition(vector3df pos)
{

	node->setPosition(pos);
	//node->updateAbsolutePosition();
}

void DynamicObject::setOldPos()
{
	node->setPosition(oldpos);
	node->updateAbsolutePosition();
}

vector3df DynamicObject::getPosition()
{
	//Fakeshadow seem to cause problems... Let put this off while we investigate
	/*if (fakeShadow)
	{
		vector3df pos = fakeShadow->getAbsolutePosition();
		return pos;
	}
	else
	*/
	{
		if (node)
			return node->getAbsolutePosition();
		else
			return vector3df(0,0,0);
	}
}

void DynamicObject::setRotation(vector3df rot)
{
    node->setRotation(rot);
}

vector3df DynamicObject::getRotation()
{
    return node->getRotation();
}

void DynamicObject::moveObject(f32 speed)
{

	vector3df pos=this->getPosition();
	pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;
    pos.Y = 0;///TODO: fixar no Y da terrain (gravidade)

	this->setPosition(pos);
	currentObject=this;
	currentSpeed=speed;
}

void DynamicObject::walkTo(vector3df targetPos)
{
	// Will have the object walk to the targetposition at the current speed.
	// Walk can be interrupted by:
	// - A collision with another object (need to be updated as collision is down)
	// - Moving into a part of the terrain that is not reachable (based on height of terrain)

	collided=false;
	if (getType()==OBJECT_TYPE_PLAYER && Player::getInstance()->getTaggedTarget())
		targetPos = Player::getInstance()->getTaggedTarget()->getPosition();
	else
		targetPos = vector3df((f32)round32(targetPos.X),(f32)round32(targetPos.Y),(f32)round32(targetPos.Z));

	this->lookAt(targetPos);

	//Attemp to calculate the proper time/distance interval
	u32 delay=App::getInstance()->getTimer()-lastTime; //The delay reference MUST come from the main REFRESH loop in APP.
	lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();

	f32 newspeed = 1.0f;
	// Calculate the distance based on time (delay)
	f32 speed = (currentAnim.walkspeed*(f32)delay)/1000; //(based on seconds (1000 for 1000ms)
	//change the speed of the object if the scale is changed.
	if (node->getScale()!=originalscale)
	{
		//printf("Original scale is: %f - new scale is: %f\n",originalscale.Y,getScale().Y);
		newspeed=currentAnim.walkspeed*(node->getScale().Y/originalscale.Y);
		speed = (newspeed*(f32)delay)/1000;
	}
	if (speed == 0)
		speed=1.0f;

	
	vector3df pos=this->getPosition();
	oldpos=pos;

	pos.Z -= cos((this->getRotation().Y)*PI/180)*speed;
    pos.X -= sin((this->getRotation().Y)*PI/180)*speed;


	// Sampling points on the ground
	// TODO: The sampling point should be spaced based on the character size and not fixed values
	vector3df posfront1 = pos+(targetPos.normalize()*20);
	vector3df posfront = pos+(targetPos.normalize()*10);
	vector3df posback1 = pos-(targetPos.normalize()*20);
	vector3df posback = pos-(targetPos.normalize()*10);


	// Redefine the sampling point using a new formula (12/02/12), was giving incorrect coordinates
	posfront.Z = pos.Z - cos((this->getRotation().Y)*PI/180)*20;
    posfront.X = pos.X - sin((this->getRotation().Y)*PI/180)*20;
	posfront.Y = pos.Y;

	posfront1.Z = pos.Z - cos((this->getRotation().Y)*PI/180)*40;
    posfront1.X = pos.X - sin((this->getRotation().Y)*PI/180)*40;
	posfront1.Y = pos.Y;

	posback1.Z = pos.Z + cos((this->getRotation().Y)*PI/180)*40;
    posback1.X = pos.X + sin((this->getRotation().Y)*PI/180)*40;
	posback1.Y = pos.Y;

	posback.Z = pos.Z + cos((this->getRotation().Y)*PI/180)*20;
    posback.X = pos.X + sin((this->getRotation().Y)*PI/180)*20;
	posback.Y = pos.Y;


	// Samples position where the ground is
	f32 height = rayTest(vector3df(pos.X,pos.Y+80,pos.Z),vector3df(pos.X,pos.Y-80,pos.Z));
	//f32 height2 = rayTest(vector3df(posfront.X,posfront.Y+2000,posfront.Z),vector3df(posfront.X,posfront.Y-2000,posfront.Z));
	f32 height3 = rayTest(vector3df(posfront1.X,posfront1.Y+80,posfront1.Z),vector3df(posfront1.X,posfront1.Y-80,posfront1.Z));
	//f32 height4 = rayTest(vector3df(posback.X,posback.Y+2000,posback.Z),vector3df(posback.X,posback.Y-2000,posback.Z));
	//f32 height5 = rayTest(vector3df(posback1.X,posback1.Y+2000,posback1.Z),vector3df(posback1.X,posback1.Y-2000,posback1.Z));

	// Sample in the front
	f32 frontcol = rayTest(vector3df(pos.X,pos.Y+36,pos.Z),vector3df(posfront.X,posfront.Y+36,posfront.Z));
	if (frontcol>-1000)
	{
		collided=true;
	}
	else
		collided=false;

	// if test has failed
	if (height==-1000.0f)
	{
		height = TerrainManager::getInstance()->getHeightAt(pos);
	}

	f32 cliff =  height3 - height;
	if (cliff<0)
		cliff = -cliff;

	if (cliff>30)
	{
		// Do a smaller ray test to check
		f32 oldheight = height;
		f32 oldheight2 = height3;

		// Need to recheck 2 points for the "cliff"
		height = rayTest(vector3df(pos.X,pos.Y+100,pos.Z),vector3df(pos.X,pos.Y-2000,pos.Z));
		height3 = rayTest(vector3df(posfront1.X,posfront1.Y+100,posfront1.Z),vector3df(posfront1.X,posfront1.Y-2000,posfront1.Z));
		if (height==-1000)
			height=oldheight;
		if (height>-1000)
			cliff =  height3 - height;

	}

	if (cliff<0)
		cliff = -cliff;

	//Check the old position and the new position on the Y axis. Must not be too high
	vector3df oldposition = this->getPosition();
	f32 result = (oldposition.Y-height);
	if (result<0)
		result=-result;

	//This is for detecting "jumps" to avoid player jumps on buildings or modeled geometry
	if (result>(getNode()->getAbsoluteTransformation().getScale().Y*0.25f))
	{
		collided=true; //This mean that the distance it too high for the character to move there
#ifdef DEBUG
		printf("Too high! %f units. Name is: %s\n",result,getName().c_str());
		printf("Old position: %f,%f,%f. desired height is: %f\n",pos.X,pos.Y,pos.Z,result);
#endif

	}

	if (cliff > 40)
	{

#ifdef DEBUG
		printf("Cliff is too steep! result: %f\n",cliff);
#endif
		collided=true;
	}

	// The player and NPC should not get into the ocean
	if (height<-80.0f)
		collided=true;

	// The "cliff" is the number of unit of difference from one point to another
	// The limit in the water is to get to -80 (legs into water)
	// This number should be based on the height of the character and not fixed values
	//if (height>-80 && (cliff < 60) && !collided)
	if (!collided)
	{
		pos.Y = height;
		// Get the average of the heights to give a smoother result.
		// pos.Y=((height+height2+height3+height4+height5)/5)+2;
		this->setPosition(pos);
		this->getNode()->updateAbsolutePosition();
	}
	else
	{
		// This will be activated if there is a collision (collided=true)
		// Since we're not using IRRlicht collision response animators now for this,
		// Collision detection between NPC will have to be is not implemented (simple radius detection)
		walkTarget = this->getPosition();
		this->setPosition(oldpos);
		this->getNode()->updateAbsolutePosition();
		reached=true;

		if (enemyUnderAttack)
		{
			stringc currentenemy = enemyUnderAttack->getNode()->getName();
			if (namecollide==currentenemy && Player::getInstance()->controltype!=Player::CONTROL_POINTNCLICK)
			{
				if (objectType==OBJECT_TYPE_PLAYER && enemyUnderAttack->getObjectType() == stringc("ENEMY"))
				{
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(true);
					lookAt(enemyUnderAttack->getPosition());
					setAnimation("attack");
				}
				else
					setAnimation("idle");
			} else
			{
				this->setAnimation("idle");
			}

		} else
		{
			this->setAnimation("idle");
		}

		collided=false; // reset the collision flag
	}

}

// Tell if the object is walking or running
bool DynamicObject::isWalking()
{
	if (getAnimation()!=OBJECT_ANIMATION_WALK && getAnimation()!=OBJECT_ANIMATION_RUN)
		return false;
	else
		return true;
}

f32 DynamicObject::rayTest(vector3df pos, vector3df pos1)
{
	smgr = App::getInstance()->getDevice()->getSceneManager();
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	core::line3d<f32> ray;

    ray.start = pos;
	ray.end = pos1;
	// Tracks the current intersection point with the level or a mesh
	core::vector3df intersection;
    // Used to show with triangle has been hit
    core::triangle3df hitTriangle;
	scene::ISceneNode * selectedSceneNode =
    collMan->getSceneNodeAndCollisionPointFromRay(
		ray,
		intersection,
		hitTriangle,
		100, //100 is the default ID for walkable (ground obj + props)
		0); // Check the entire scene (this is actually the implicit default)

	if (selectedSceneNode)
	{
		// return the height found.
		return intersection.Y;
	}
	else
		// if not return 0
		return -1000;
}

void DynamicObject::setWalkTarget(vector3df newTarget)
{
	// This is temporary fix that redefine a target. The new target will be 50 unit nearer from the old destination.
	// (This allow the NPC not to go directly a the player position)
	// This need to be improved further as this should apply only when a NPC destination is selected.
	
	if (objectType!=OBJECT_TYPE_PLAYER && App::getInstance()->getAppState()==App::APP_GAMEPLAY_NORMAL)
	{
		f32 desiredDistance=0.0f;
			
		desiredDistance=(((node->getBoundingBox().getExtent().X*node->getScale().Y))+DynamicObjectsManager::getInstance()->getPlayer()->getNode()->getBoundingBox().getExtent().X);
		//Check if there is a ennemy defined and find the proper distance by checking the size of it.
		//desiredDistance = 50.0f;

		//printf("Desired distance is: %f\n",(float)desiredDistance);

		f32 distance = getDistanceFrom(newTarget);
		f32 final = (distance-desiredDistance)/distance;
		//walkTarget = newTarget.getInterpolated(getPosition(),final);
		walkTarget = newTarget;
		reached=false;
	} else
	{
		walkTarget=newTarget;
		reached=false;
	}
}

vector3df DynamicObject::getWalkTarget()
{
	return walkTarget;
}


f32 DynamicObject::getDistanceFrom(vector3df pos)
{
    return node->getPosition().getDistanceFrom(pos);
}


//Tried to give a distance based on the depth of the bounding box of the characters (NPC + PLAYER)
// Might not be 100% accurate has such models varies a lot in the depth (Z Axis)
f32 DynamicObject::getObjectSize(bool withenemy)
{

	ISceneNode * us = getNode();
	ISceneNode * enemy = NULL;

	if (objectType!=OBJECT_TYPE_PLAYER)
	{
		enemy = Player::getInstance()->getObject()->getNode();
	} else if (enemyUnderAttack)
	{
		if (enemyUnderAttack)
			enemy = this->enemyUnderAttack->getNode();
		else return 0;
	}

	f32 properdistance = 0.0f;

	
	if (withenemy && enemy) //Give the distance between the enemy and the object
	{
		properdistance = ((enemy->getBoundingBox().getExtent().Z * enemy->getScale().Z)) + ((us->getBoundingBox().getExtent().Z * us->getScale().Z));
		//Note, might divide per 2 to get a more precise distance. using 3/4 of the scale for now
		//properdistance=properdistance*.5f;
	} else //Give the size of the object
	{
		properdistance = ((us->getBoundingBox().getExtent().Z * us->getScale().Z));
	}

 //printf("the distance is %f\n",properdistance);
	return properdistance;
}

DynamicObject * DynamicObject::getCurrentEnemy()
{
	return enemyUnderAttack;
}

void DynamicObject::clearEnemy()
{
	enemyUnderAttack=NULL;
}

//-----------------------------------------------------------------------
// Properties
//-----------------------------------------------------------------------

void DynamicObject::setTemplate(bool value)
{
	this->templateobject=value;
}

bool DynamicObject::isTemplate()
{
	return this->templateobject;
}

DynamicObject::cproperty DynamicObject::getProperties()
{
	return this->properties;
}

void DynamicObject::setProperties(DynamicObject::cproperty prop)
{
	properties = prop;
}


DynamicObject::cproperty DynamicObject::getProp_base()
{
	return this->prop_base;
}

void DynamicObject::setProp_base(DynamicObject::cproperty prop)
{
	prop_base=prop;
}

DynamicObject::cproperty DynamicObject::getProp_level()
{
	return this->prop_level;
}

void DynamicObject::setProp_level(DynamicObject::cproperty prop)
{
	prop_level=prop;
}

void DynamicObject::setMaterials(vector<DynamicObject::DynamicObject_material> mat)
{
	this->materials = mat;	
}

void DynamicObject::setEnabled(bool enabled)
{
    this->enabled = enabled;

    this->node->setVisible(enabled);
	if (this->getNode()->isVisible()==false && this==Player::getInstance()->getObject()->getCurrentEnemy())
	{
		Player::getInstance()->getObject()->clearEnemy();
		Player::getInstance()->getObject()->setAnimation("prespawn");
	}
}

bool DynamicObject::isEnabled()
{
	return enabled;
}
void DynamicObject::setType(stringc name)
{
	if (name=="npc")
		this->objectType=OBJECT_TYPE_NPC;
	if (name=="loot")
		this->objectType=OBJECT_TYPE_LOOT;
	if (name=="interactive")
		this->objectType=OBJECT_TYPE_INTERACTIVE;
	if (name=="non-interactive")
		this->objectType=OBJECT_TYPE_NON_INTERACTIVE;
	if (name=="player")
		this->objectType=OBJECT_TYPE_PLAYER;
	if (name=="editor")
		this->objectType=OBJECT_TYPE_EDITOR;
	if (name=="walkable")
		this->objectType=OBJECT_TYPE_WALKABLE;

	this->typeText = name;
}

void DynamicObject::setType(TYPE type)
{
	objectType=type;
}

DynamicObject::TYPE DynamicObject::getType()
{
	return objectType;
}

void DynamicObject::setName(stringc name)
{
    this->name = name;
    this->getNode()->setName(name);
}

stringc DynamicObject::getName()
{
    return name;
}

void DynamicObject::setMaterialType(E_MATERIAL_TYPE mType)
{
	node->getMaterial(0).MaterialType=mType;
}

E_MATERIAL_TYPE DynamicObject::getMaterialType()
{
    return node->getMaterial(0).MaterialType;
}

void DynamicObject::setScale(vector3df scale)
{
    node->setScale(scale);
}

vector3df DynamicObject::getScale()
{
    return node->getScale();
}

void DynamicObject::setLife(int life)
{
	if (life<0)
		life=0;
    this->properties.life = life;
	if (objectType==OBJECT_TYPE_PLAYER)
		Player::getInstance()->updateDisplay();
}

int DynamicObject::getLife()
{
	if (properties.life)
		return this->properties.life;
	else
		return 0;
}

void DynamicObject::setMoney(int money)
{
	this->properties.money = money;
}

int DynamicObject::getMoney()
{

	return this->properties.money;

}

// Label
void DynamicObject::setObjectLabel(stringc label)
{
	u32 currenttime=App::getInstance()->getDevice()->getTimer()->getRealTime();
	// Will update the display at each 1/10 second, so we have the time to see what is written
	if (currenttime-timer_display>100)
	{
		timer_display=currenttime;
		objLabel->setText(stringw(label).c_str());
		s32 percent = (properties.life * 100);
		s32 p2 = percent / 100;
		if (properties.maxlife>0)
			p2 = percent / properties.maxlife;

		Healthbar->setProgress(p2);
	}
}

void DynamicObject::objectLabelSetVisible(bool visible)
{
    objLabel->setVisible(visible);
	if (objectType!=OBJECT_TYPE_NPC)
		Healthbar->setVisible(false);
	else
		Healthbar->setVisible(visible);
}

// Create and animate a text over the object head
void DynamicObject::createTextAnim(core::stringw text, video::SColor color, u32 duration, dimension2d<f32> size)
{
	const wchar_t * ttext = L" ";
	
	if (!text.empty())
		ttext=text.c_str();

	vector3df start = getNode()->getAbsolutePosition();
	
	f32 height = getNode()->getBoundingBox().getExtent().Y*getScale().Y;
	
	start.Y+=height-30;
	vector3df end = start;
	end.Y+=height+50;
	smgr=App::getInstance()->getDevice()->getSceneManager();

	IBillboardTextSceneNode * nodetext = smgr->addBillboardTextSceneNode(GUIManager::getInstance()->getFont(GUIManager::FONT_LARGE),ttext,smgr->getRootSceneNode(),size,start,-1,color,color);
	
	scene::ISceneNodeAnimator * anim = smgr->createDeleteAnimator(duration);
	scene::ISceneNodeAnimator * anim2 = smgr->createFlyStraightAnimator(start,end,duration);
	if (nodetext && anim)
	{	
		nodetext->addAnimator(anim);
		nodetext->addAnimator(anim2);
	}
}

//-----------------------------------------------------------------------
// Animation system functions
//-----------------------------------------------------------------------
void DynamicObject::setFrameLoop(s32 start, s32 end)
{
    if(hasAnimation()) ((IAnimatedMeshSceneNode*)node)->setFrameLoop(start,end);
}

void DynamicObject::setAnimationSpeed(f32 speed)
{
    if(hasAnimation()) ((IAnimatedMeshSceneNode*)node)->setAnimationSpeed(speed);
}

DynamicObject::OBJECT_ANIMATION DynamicObject::getAnimationState(stringc animName)
{
	if (animName==NULL)
		return OBJECT_ANIMATION_CUSTOM;
	//printf("Asked animation name is: %s\n",animName.c_str());
	OBJECT_ANIMATION Animation;
	// Preset the animation type as "custom", but can be overwritten if standard types are discovered.
	Animation=OBJECT_ANIMATION_CUSTOM;
	if (animName=="idle")
		Animation=OBJECT_ANIMATION_IDLE;
	if (animName=="walk")
		Animation=OBJECT_ANIMATION_WALK;
	if (animName=="run")
		Animation=OBJECT_ANIMATION_RUN;
	if (animName=="attack")
		Animation=OBJECT_ANIMATION_ATTACK;
	if (animName=="hurt")
		Animation=OBJECT_ANIMATION_INJURED;
	if (animName=="knockback")
		Animation=OBJECT_ANIMATION_KNOCKBACK;
	if (animName=="die")
		Animation=OBJECT_ANIMATION_DIE;
	if (animName=="die_knockback")
		Animation=OBJECT_ANIMATION_DIE_KNOCKBACK;
	if (animName=="spawn")
		Animation=OBJECT_ANIMATION_SPAWN;
	if (animName=="despawn")
		Animation=OBJECT_ANIMATION_DESPAWN;
	if (animName=="despawn_knockback")
		Animation=OBJECT_ANIMATION_DESPAWN_KNOCKBACK;
	if (animName=="prespawn")
		Animation=OBJECT_ANIMATION_PRESPAWN;
	return Animation;
}

DynamicObject::OBJECT_ANIMATION DynamicObject::getAnimation(void)
{
	return currentAnimation;
}

// This define the animation to take based on an "ACTION" based name
// Later
bool DynamicObject::setAnimation(stringc animName)
{

	//define if we use a random frame in the idle animation
	bool randomize=true;

	// Setup the animation skinning of the meshes (Allow external animation to be used)
	//ISkinnedMesh* skin = NULL;
	//ISkinnedMesh* defaultskin = NULL;

	if (animName=="die")
	{
		// REmove the collision animator
		node->removeAnimator(animator);

		//
		createTextAnim(LANGManager::getInstance()->getText("float_text_die").c_str(),video::SColor(255,240,240,240),5000,dimension2d<f32>(25,12));
		splillLoot();

		// init the DieState timer. (the update() loop will wait 5 sec to initiate the despawn animation)
		timerDie = App::getInstance()->getDevice()->getTimer()->getRealTime();

		// disable the stun state if present. Dying takes over
		stunstate=false;
		attackdelaystate=false;

		//If this is not a player, hide the targetting
		if (objectType!=OBJECT_TYPE_PLAYER)
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
		
		// Switch to third person view if killed in first person
		if (CameraSystem::getInstance()->getViewType()==CameraSystem::VIEW_FPS)
		{
			if (objectType==OBJECT_TYPE_PLAYER)
			{
				CameraSystem::getInstance()->setViewType(CameraSystem::VIEW_RPG);
				CameraSystem::getInstance()->setCameraZoom(200);
				CameraSystem::getInstance()->setGameCameraRange(60,300);
			}
		}
	}

	if (animName!="hurt" && animName!="die" )
	{
		//printf("The attack has been done state is active no animation is permitted!\n");
		if (attackdelaystate && CameraSystem::getInstance()->getViewType()==CameraSystem::VIEW_RTS)
		return false;
	}

	// Return if the character is stunned
	if (stunstate && animName!="die")
	{
		//printf("The stun state is active no animation is permitted!\n");
		return false;
	}

	// Reset the walktimer if the walk anim is triggered (for the time delay calculation)
	if (animName=="walk" || animName=="run")
		lastTime=App::getInstance()->getDevice()->getTimer()->getRealTime();


	if (animName=="idle")
	{
		if (oldAnimName=="attack" && CameraSystem::getInstance()->getViewType()!=CameraSystem::VIEW_RTS && objectType==OBJECT_TYPE_PLAYER)
		{	
			animName="attack";
		}
		else
		{
			// Stop moving
			this->setWalkTarget(this->getPosition());
			reached=true;
			//node->removeAnimator(animator);
		}
		
	}

	//Forced idle
	if (animName=="forceidle")
	{
		animName="idle";
	}

	// Don't call the animation if the result is not positive (result coming from the combat class)
	if (animName=="attack" && oldAnimName!="attack")
	{

		//When the attack animation is triggered, the class interrogate the combat class and check
		//that the attack is successful before starting it
		if (objectType==OBJECT_TYPE_PLAYER && 
			(!attackdelaystate ||  CameraSystem::getInstance()->getViewType()!=CameraSystem::VIEW_RTS))
		{
			/*
			if (enemyUnderAttack)
			{
				// Call the combat class to evaluate the damage that should be done BEFORE the attack anim
				// is done.
				// If the attack is missed, we could play a specific animation for this (to do)
				// Damage is done in the "animation" at the "attack event"
				attackresult=Combat::getInstance()->attack(this,enemyUnderAttack);
				if (attackresult==0)
					//enemyUnderAttack->setObjectLabel("Miss!");
				{	
					if (enemyUnderAttack->getLife()!=0)
						enemyUnderAttack->createTextAnim(LANGManager::getInstance()->getText("float_text_miss").c_str(),video::SColor(255,240,120,0),3000,dimension2d<f32>(12,8));
				}
				else
				{
					core::stringw textdam = LANGManager::getInstance()->getText("float_text_hit");
					textdam.append(stringc(attackresult));
					//enemyUnderAttack->setObjectLabel(textdam.c_str());
					if (enemyUnderAttack->getLife()!=0)
						enemyUnderAttack->createTextAnim(textdam);
				}
				
			}*/
		}

		// 
		if (objectType!=OBJECT_TYPE_PLAYER && !attackdelaystate && oldAnimName!="attack")
		{

			/*	f32 properdistance = this->getObjectSize();
						
				if (Player::getInstance()->getNode()->getPosition().getDistanceFrom(getNode()->getPosition())<properdistance)
				{
					attackresult=Combat::getInstance()->attack(this,Player::getInstance()->getObject());
					if (attackresult==0)
						Player::getInstance()->getObject()->createTextAnim(LANGManager::getInstance()->getText("float_text_miss").c_str(),video::SColor(255,240,120,0),3000,dimension2d<f32>(12,8));
					else
					{
						stringc textdam = LANGManager::getInstance()->getText("float_text_hit").c_str();
						textdam.append(stringc(attackresult));
						//Player::getInstance()->getObject()->setObjectLabel(textdam.c_str());
						Player::getInstance()->getObject()->createTextAnim(textdam);
					}

			}*/	
		}

	}


	// This will activate the "hurt" stun state
	if (oldAnimName == "hurt" && animName=="hurt" && !stunstate)
	{
		if (oldAnimName!="attack")
		{
			if (CameraSystem::getInstance()->getViewType()!=CameraSystem::VIEW_RTS) 
				stunstate=true;
			else
				animName="attack";
		}

		timerStun = App::getInstance()->getDevice()->getTimer()->getRealTime();

	}

	// Activate the "attack delay" after the attack is initialized
	//if (oldAnimName=="attack" && !attackdelaystate)
	if (oldAnimName == "attack" && animName == "attack" && !attackdelaystate)
	{
		if (CameraSystem::getInstance()->getViewType()==CameraSystem::VIEW_RTS || objectType!=OBJECT_TYPE_PLAYER)
		{
			attackdelaystate=true;
		} 
		
		timer_attackdelay=App::getInstance()->getDevice()->getTimer()->getRealTime();
	} else
	{
		attackdelaystate=false;
	}

	// When a character is dead, don't allow anything exept prespawn or despawn
	if (currentAnimation==OBJECT_ANIMATION_DIE && animName!="prespawn" && animName!="despawn" )
	{
		return false;
	}


	// This should stop the NPc from attacking the player if they are not the "tagged" enemy.
	if (App::getInstance()->getAppState()==App::APP_GAMEPLAY_NORMAL && objectType!=OBJECT_TYPE_PLAYER)
	{

		if (Player::getInstance()->getTaggedTarget() && this->getName()!=Player::getInstance()->getTaggedTarget()->getName())
		{
			if (animName=="attack")
			{
				printf("This object is not the one that battle the player!\n");
				this->setWalkTarget(this->getPosition());
				reached=true;
				animName="idle";
				oldAnimName="attack";
			}

		}
	
	}

	if (!getNode())
		return false;

	//if (objectType==OBJECT_TYPE_PLAYER)
	//	printf("Here the player is doing this animation: %s \n",animName.c_str());
	
	// Search for the proper animation name and set it.
	core::stringc oldanim=""; //Used for checking for duplicate names, so it won't be playing the animation all the time.
	
	u32 variationcounter=1; //Starting at with because of the "default".

	//Count the possible variations
	for(int i=0;i < (int)animations.size();i++)
    {
		DynamicObject_Animation tempAnim = (DynamicObject_Animation)animations[i];
		if( tempAnim.name == animName && tempAnim.variation>0)
			variationcounter++;
	}



    for(int i=0;i < (int)animations.size();i++)
    {

		// temporary (until a real prespawn is defined inside the game)
		if (animName=="prespawn")
		{
			stunstate=false;
			attackdelaystate=false;
			animName="idle";
			randomize=true;
			//printf("Prespawn is called here.\n");
		}

		OBJECT_ANIMATION Animation = this->getAnimationState(animName);

		DynamicObject_Animation tempAnim = (DynamicObject_Animation)animations[i];

		int random = rand() % variationcounter + 1;

		//stance with "" is the default state
		//wear with "" is the default
		//Later could put a string to check here when theses are changing 
		// Ex: implement wear, could create a "currentwear" with what is in there. (Will need a fallback to default if not found, so a search first)
			
		if( tempAnim.name == animName && tempAnim.stance=="" && tempAnim.wear=="" && tempAnim.variation==random-1)
        {
			oldanim=tempAnim.name;

			if ((Animation!=this->currentAnimation) || Animation==OBJECT_ANIMATION_CUSTOM)
			{
				
				if (tempAnim.meshname!="" && nodeAnim->getMesh()->getFrameCount()>1)
				{
					if (tempAnim.mesh)
					{
						ISkinnedMesh* tempanim = (ISkinnedMesh*)tempAnim.mesh;
						((ISkinnedMesh*)nodeAnim->getMesh())->useAnimationFrom(tempanim);
					}
				} else
				{
					if (this->mesh)
					{
						ISkinnedMesh* tempanim = (ISkinnedMesh*)this->mesh;
						((ISkinnedMesh*)nodeAnim->getMesh())->useAnimationFrom(tempanim);
					}
				}
				// Store the old animations
				this->oldAnimation=currentAnimation;
				this->oldAnimName = animName;

					// Set the state of the current one
				this->currentAnimation=Animation;
				this->currentAnim=tempAnim;

				// Set the frameloop, the current animation and the speed
				this->setFrameLoop(tempAnim.startFrame,tempAnim.endFrame);
				this->setAnimationSpeed(tempAnim.speed);
				this->nodeAnim->setLoopMode(tempAnim.loop);
				//printf ("More info is start:%i end: %i\n",(int)tempAnim.startFrame,tempAnim.endFrame);

				// Special case for the idle animation (randomisation)
				if (animName=="idle" && randomize)
				{

					// Fix a random frame so the idle for different character are not the same.
					if (tempAnim.endFrame>0)
					{
						f32 random = (f32)(rand() % tempAnim.endFrame+1);
						this->nodeAnim->setCurrentFrame(random+1);
					}
				}
			}
            return true;
        }
    }

	#ifdef DEBUG
	if (objectType==OBJECT_TYPE_NPC || objectType==OBJECT_TYPE_PLAYER)
		cout << "ERROR : DYNAMIC_OBJECT" << name.c_str() <<  ": ANIMATION " << animName.c_str() <<  " NOT FOUND!" << endl;
    #endif

	// If the die animation is not there, the flag become active (will start the die timer anyway)
	// As always this does not apply to the player (event if it misse it's die animation)
	if (animName=="die" && this->getType()!=DynamicObject::OBJECT_TYPE_PLAYER)
		diePresent=false;
	if (animName=="despawn" && this->getType()!=DynamicObject::OBJECT_TYPE_PLAYER)
		despawnPresent=false;

	return false;
}

// To do, this will trigger the combat damage and the sound when it reach the proper frame
// It is checking the current animation (does care what animation)
// Called at each refresh (1/60 th sec)
void DynamicObject::checkAnimationEvent()
{
	//Reset the flags
	attackActivated.clear();
	soundActivated.clear();

    //TODO: Return when the object has no animation
	if (!nodeAnim)
		return;

	// Check if the character is hurt and tell the combat manager to stop the attack while the character play all the animation
	// Need to update this to support more specific animation that MUST not be stopped
	if ((s32)nodeAnim->getFrameNr()!=lastframe && this->currentAnimation==OBJECT_ANIMATION_INJURED)
	{
		if (((s32)nodeAnim->getFrameNr() == currentAnim.startFrame))
		{
			// Set the AI State to busy, so the combat manager won't call animations
			AI_State=AI_STATE_BUSY;
		}

		// should do something when the animation can reach the end
		// This is required
		if (((s32)nodeAnim->getFrameNr() == currentAnim.endFrame))
		{
			AI_State=AI_STATE_IDLE;
		}

	}
	// Check if the current animation have an attack event
	if ((s32)nodeAnim->getFrameNr()!=lastframe && this->currentAnimation==OBJECT_ANIMATION_ATTACK)
	{

		//This set the animation back to idle when it's played
		if ((s32)nodeAnim->getFrameNr()>currentAnim.endFrame-1)
		{
			if (attackActivated[0])
			{
				attackActivated[0]=false;
				GUIManager::getInstance()->setConsoleText(core::stringw(L"Error! Attack was not triggered!"),video::SColor(255,0,0,0));
			}
			//nodeAnim->setCurrentFrame(currentAnim.startFrame);
			this->setAnimation("forceidle");
			//printf("Reset animation to idle here\n");
		}

		// Set a default attack event if there is none defined.
		if (currentAnim.attackevent[0]==-1)
		{
			GUIManager::getInstance()->setConsoleText(core::stringw(L"No attack defined!"),video::SColor(255,0,0,0));
			currentAnim.attackevent[0] = currentAnim.startFrame+1;
		}

		for (u32 b=0; b<currentAnim.attackevent.size(); b++)
		{
			attackActivated.push_back(false);
		}
		for(u32 a=0; a<currentAnim.attackevent.size(); a++)
		{
			// This only mean that the attack animation is still looking for the event
			if ((nodeAnim->getFrameNr() > currentAnim.attackevent[a]-1) && nodeAnim->getFrameNr() <= currentAnim.attackevent[a])
			{
				attackActivated[a]=true;
				if (getType()!=OBJECT_TYPE_PLAYER)
					printf("Attack activated for %s\n",name.c_str());
			}
			

			if (attackActivated[a]) // && nodeAnim->getFrameNr() <= currentAnim.attackevent[a])
			{

			// Attack result is precalculated, if the animation of attack is played, them it was sucessful
			// the damage will then be done at the "impact" frame from the animation
			// If the character health rise 0, then call the die animation.

			// The logic might be moved inside the combat manager... Not sure at the moment.
				attackActivated[a] = false;
				if (!attackActivated[a])
				{
					if (objectType==OBJECT_TYPE_PLAYER)
					{

						int resultlife = 0;
						if (enemyUnderAttack && enemyUnderAttack->getLife()>0)
						{
							printf("Attack event!\n");
							
						
							attackresult=Combat::getInstance()->attack(Player::getInstance()->getObject(),enemyUnderAttack);
						
							if (attackresult==0)
							{
								enemyUnderAttack->createTextAnim(LANGManager::getInstance()->getText("float_text_miss").c_str(),
									video::SColor(255,240,120,0),3000,dimension2d<f32>(12,8));
							}
							else
							{
								stringc textdam = LANGManager::getInstance()->getText("float_text_hit").c_str();
								textdam.append(stringc(attackresult));
								enemyUnderAttack->createTextAnim(textdam);
					
								resultlife = enemyUnderAttack->getLife()-attackresult;
								enemyUnderAttack->setLife(resultlife);
								enemyUnderAttack->setAnimation("hurt");
							}

							core::stringw textresult = core::stringw(L"The player attacked ").append(enemyUnderAttack->getName()).append(L" and caused ")
								.append(core::stringw(attackresult)).append(L" points of damage!");
						
							GUIManager::getInstance()->setConsoleText(textresult,video::SColor(255,0,0,65));
						
							if (enemyUnderAttack->getLife()<1)
							{
								enemyUnderAttack->setAnimation("die");
								this->setAnimation("idle2");

								GUIManager::getInstance()->setConsoleText(core::stringw(L"The player killed ")
									.append(core::stringw(enemyUnderAttack->getName())
									.append(L"!")),video::SColor(255,120,0,0));
							
								enemyUnderAttack=NULL;
								Player::getInstance()->setTaggedTarget(NULL);
							}
						}
					}

					if (objectType!=OBJECT_TYPE_PLAYER)
					{
						attackresult=Combat::getInstance()->attack(this, Player::getInstance()->getObject());
						int resultlife = Player::getInstance()->getObject()->getLife()-attackresult;
						Player::getInstance()->getObject()->setLife(resultlife);

						if (attackresult==0)
							Player::getInstance()->getObject()->createTextAnim(LANGManager::getInstance()->getText("float_text_miss").c_str(),
							video::SColor(255,240,120,0),3000,dimension2d<f32>(12,8));
						else
						{
							stringc textdam = LANGManager::getInstance()->getText("float_text_hit").c_str();
							textdam.append(stringc(attackresult));
							//Player::getInstance()->getObject()->setObjectLabel(textdam.c_str());
							Player::getInstance()->getObject()->createTextAnim(textdam);
						}

						// When damage is done, the defender will look (turn to look at) at his opponent
						Player::getInstance()->getObject()->lookAt(this->getPosition());

						core::stringw textresult = core::stringw(getName()).append(L" attacked the player and caused ").append(core::stringw(attackresult)).append(L" points damage!");
						GUIManager::getInstance()->setConsoleText(textresult,video::SColor(255,0,0,65));

						if (resultlife>0)
						{
							if (attackresult>0)
								Player::getInstance()->getObject()->setAnimation("hurt");
						}
						else 
							Player::getInstance()->getObject()->setAnimation("die");

					}
				}
				else
				{
					core::stringw textresult = L"";
					if (enemyUnderAttack)
					{
						textresult = core::stringw(getName()).append(L" attacked ").append(core::stringw(enemyUnderAttack->getName()))
							.append(L" and missed! ");
					}
					else
						textresult = core::stringw(getName()).append(L" attacked the player and missed!");

					GUIManager::getInstance()->setConsoleText(textresult,video::SColor(255,0,0,65));
				}
			}
		}
	}


	for (u32 b=0; b<currentAnim.soundevent.size(); b++)
	{
		soundActivated.push_back(false);
	}
	for (u32 b=0; b<currentAnim.soundevent.size(); b++)
	{
		// Check if the current animation have an sound event
		if (nodeAnim->getFrameNr()>currentAnim.soundevent[b]-1 && nodeAnim->getFrameNr() <= currentAnim.soundevent[b])
			soundActivated[b]=true;

		if ((currentAnim.sound.size() > 0) && soundActivated[b])
		{
			soundActivated[b]=false;
			stringc sound = currentAnim.sound;			
			stringc soundName = "../media/sound/";
			soundName += sound.c_str();
			//irrklang::vec3df pos = this->getNode()->getPosition();

			//SoundManager::getInstance()->playSound2D(soundName.c_str(),false);
			
			if (soundfx)
			{
				if (!soundfx->isFinished()) //If the sound is not finished stop it so we don't have weird mix.
				{
					soundfx->stop();
				}

				if (soundfx && soundfx->isFinished())
					soundfx=SoundManager::getInstance()->playSound2D(soundName.c_str(),false);

			}

			if (!soundfx)
				soundfx = SoundManager::getInstance()->playSound2D(soundName.c_str(),false);
		}
		lastframe=(s32)nodeAnim->getFrameNr();

	}
}
void DynamicObject::setRunningMode(bool run)
{
	runningMode=run;
}

//-----------------------------------------------------------------------
// Collision response
//-----------------------------------------------------------------------
void DynamicObject::setAnimator(ISceneNodeAnimatorCollisionResponse* animator_node)
{
	this->animator=animator_node;
}

ISceneNodeAnimatorCollisionResponse* DynamicObject::getAnimator()
{
	return animator;
}

ITriangleSelector* DynamicObject::getTriangleSelector()
{
    return selector;
}



// Main attack feature (Player + NPC)
void DynamicObject::attackEnemy(DynamicObject* obj)
{
	if (!obj)
		return;

	if (enemyUnderAttack==obj && Player::getInstance()->controltype==Player::CONTROL_POINTNCLICK)
		return;

    enemyUnderAttack = obj;

	if (Player::getInstance()->controltype!=Player::CONTROL_POINTNCLICK)
	{
		// Attack animation triggered for FPS or RPG
		setAnimation("attack");
		attackresult=Combat::getInstance()->attack(this,obj);
		if (attackresult>0)
			this->setAnimation("attack"); // Have some hit points
		else 
			this->setAnimation("idle"); // Missed

		return;

	}
	else
	{
        this->lookAt(obj->getPosition());
		// This return the size (back to front of an object.
		f32 size = ((obj->getNode()->getBoundingBox().getExtent().X*obj->getScale().X) + (this->getNode()->getBoundingBox().getExtent().X*this->getScale().X));
		
		if(obj->getDistanceFrom(Player::getInstance()->getObject()->getPosition()) < (size/2)+10)
		{
			// Attack triggered for the other types of camera controls
			attackresult=Combat::getInstance()->attack(this,obj);
			if (attackresult>0)
				this->setAnimation("attack"); // Have some hit points
			else
				this->setAnimation("idle"); // Missed

			obj->notifyClick();
		}
    }

}

//-----------------------------------------------------------------------
// INVENTORY features (need to be changed to use dynamic objects instead of text)
//-----------------------------------------------------------------------
void DynamicObject::addItem(stringc itemName)
{
    items.push_back(itemName);
}

void DynamicObject::removeItem(stringc itemName)
{
    for(int i=0;i<(int)items.size();i++)
    {
        if(items[i] == itemName)
        {
            items.erase(items.begin() + i);
            return;//remove only one item
        }
    }
}

vector<stringc> DynamicObject::getItems()
{
    return items;
}

int DynamicObject::getItemCount(stringc itemName)
{
    int total = 0;

    for(int i=0;i<(int)items.size();i++)
    {
        if(items[i] == itemName)
        {
            total++;
        }
    }

    return total;
}

bool DynamicObject::hasItem(stringc itemName)
{
    for(int i=0;i<(int)items.size();i++)
    {
        if(items[i] == itemName)
        {
            return true;
        }
    }

    return false;
}

void DynamicObject::removeAllItems()
{
    items.clear();
}
//-----------------------------------------------------------------------
//Loot Management (work with dynamic object pointers)
//-----------------------------------------------------------------------
void DynamicObject::addLootItem(DynamicObject* loot)
{
	lootitems.push_back(loot);
}

void DynamicObject::removeLoot(DynamicObject* loot)
{
	for(int i=0;i<(int)lootitems.size();i++)
    {
        if(lootitems[i] == loot)
        {
            lootitems.erase(lootitems.begin() + i);
            return;//remove only one loot item
        }
    }
}

vector<DynamicObject*> DynamicObject::getLootItems()
{
	return lootitems;
}

void DynamicObject::removeAllLoot()
{
	lootitems.clear();
}

//-----------------------------------------------------------------------
//Script management
//-----------------------------------------------------------------------
stringc DynamicObject::getScript()
{
    return script;
}

void DynamicObject::setScript(stringw script)
{
    this->script = script;
}

void DynamicObject::clearScripts()
{
    if(hasAnimation())
	{
		//this->setFrameLoop(0,0);
		this->setAnimation("prespawn");
		//printf("Script had been cleared... idle.\n");
	}
	/*if (objectType == OBJECT_TYPE_PLAYER)
	{
		printf("Player restore the original position \n");
		lua_getglobal(L,"IRBRestorePlayerParams");
		if(lua_isfunction(L, -1)) lua_call(L,0,0);
		lua_pop( L, -1 );
		lua_close(L);
	}*/
	if (objectType != OBJECT_TYPE_PLAYER)
	{
		lua_close(ls);
	}
}

void DynamicObject::doScript()
{
    // create an Lua pointer instance
    ls = lua_open();

    // load the libs
    luaL_openlibs(ls);

    // register dynamic object functions

	lua_register(ls,"getEnemyCount",getEnemyCount);

    lua_register(ls,"setPosition",setPosition);
    lua_register(ls,"getPosition",getPosition);
    lua_register(ls,"setRotation",setRotation);
    lua_register(ls,"getRotation",getRotation);
    lua_register(ls,"lookAt",lookAt);
    lua_register(ls,"lookToObject",lookToObject);

	lua_register(ls,"attack",attackObj);
	lua_register(ls,"setProperty",setProperty);
	lua_register(ls,"getProperty",getProperty);

	lua_register(ls,"setPropertie",setProperty); //Old commands, should be removed in 0.4. New name is setProperty
	lua_register(ls,"getPropertie",getProperty); //Old commands, should be removed in 0.4. New name is getProperty
    
	lua_register(ls,"move",move);
	lua_register(ls,"walkTo",walkToLUA);
    lua_register(ls,"distanceFrom",distanceFrom);
	lua_register(ls,"getName",getNameLUA);
	lua_register(ls,"setName",setNameLUA);


    lua_register(ls,"setFrameLoop",setFrameLoop);
    lua_register(ls,"setAnimationSpeed",setAnimationSpeed);
    lua_register(ls,"setAnimation",setAnimation);

    lua_register(ls,"showObjectLabel",showObjectLabel);
    lua_register(ls,"hideObjectLabel",hideObjectLabel);
    lua_register(ls,"setObjectLabel",setObjectLabel);
	lua_register(ls,"setObjectType",setObjectType);
	lua_register(ls,"moveObjectLoot",addPlayerLoot);
	lua_register(ls,"addLoot",addLootLUA);
	lua_register(ls,"setEnemy1",setEnemy);
	
	//Dialog Functions
    lua_register(ls,"showDialogMessage",showDialogMessage);
	lua_register(ls,"showDialogQuestion",showDialogQuestion);

    lua_register(ls,"setEnabled",setEnabled);

	lua_register(ls,"hasReached",hasReached);

	lua_register(ls,"destroyAfterUse",destroyAfterUse);

    //register basic functions
    LuaGlobalCaller::getInstance()->registerBasicFunctions(ls);

    //associate the "objName" keyword to the dynamic object name
    stringc scriptTemp = "objName = '";
    scriptTemp += this->getNode()->getName();
    scriptTemp += "'";
    luaL_dostring(ls,scriptTemp.c_str());


    luaL_dostring(ls,stringc(script).c_str());

    //set default object type
    luaL_dostring(ls,"objType = 'OBJECT'");
    //set enemy (when you click an enemy you attack it)
    luaL_dostring(ls,"function setEnemy() objType = 'ENEMY' setEnemy1(true) end");
    //set object (when you click an object you interact with it)
    luaL_dostring(ls,"function setObject() objType = 'OBJECT' setEnemy1(false) end");


	//run onLoad() function if it exists
    lua_getglobal(ls,"onLoad");
    //if top of stack is not a function then onLoad does not exist
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop( ls, -1 );
	storeParams();
}

void DynamicObject::storeParams()
{
	// store in memory the current position and rotation of the object for later retrieval.
	this->originalPosition=this->getPosition();
	this->originalRotation=this->getRotation();
	this->original_life=this->getLife();
	this->original_maxlife=this->getProperties().maxlife;
	this->oldparent=this->getNode()->getParent();
}

void DynamicObject::restoreParams()
{
    // Restore the initial parameters of the dynamic object.
	this->lootitems.clear(); //Clear the loot
	this->getNode()->setParent(oldparent); // Clear parent
	this->setPosition(this->originalPosition);
	this->setRotation(this->originalRotation);
	this->setEnabled(true);
	this->objLabel->setVisible(false);
	this->deadstate=false;
	this->setLife(original_life);
	this->properties.maxlife=original_maxlife;
	this->setAnimation("prespawn");
}

// Update the node, for animation event, collision check, lua refresh, etc.
void DynamicObject::update()
{
	// Used for culling check
	bool culled = false;

	//For some reason the scene manage pointer was corrupted. Refresh it to be sure it's alway correct.
	smgr = App::getInstance()->getDevice()->getSceneManager();

	// Reference timer for the update loop
	u32 timerobject = App::getInstance()->getDevice()->getTimer()->getRealTime();

	if (rotationupdater)
		updateRotation();
	
	// Check for an event in the current animation. This will be done at the fastest speed possible
	if (this->objectType==OBJECT_TYPE_NPC || this->objectType==OBJECT_TYPE_PLAYER)
	{
		checkAnimationEvent();
		if (objectType==OBJECT_TYPE_PLAYER)
			CameraSystem::getInstance()->updateGameCamera(); //Update camera at each update cycle of the player (better sync)

		//walkTo(walkTarget); //Seem that updating the movement "smooth out" the movement.
		//There must be a better way, the mesh seem to walk back/forth in small motion.
		//This behavior must be fixed.
	}

	if (timerobject-timerLUA>17) // Evaluated at each 17ms or more.
	{
		// Check for collision with another node
		// This is not working anymore (oct 2012) as the collision response animator is removed
		// A new method should be implemented in the dynamic object manager (using oldpos)
		if (animator && animator->collisionOccurred())
		{
			//printf ("Collision occured with %s\n",anim->getCollisionNode()->getName());
			collided=true;
			notifyCollision();
			namecollide = animator->getCollisionNode()->getName();
			this->setAnimation("idle");
			this->setPosition(oldpos);
			this->setWalkTarget(this->getPosition());
			reached=true;
		}

		// timed interface an culling check.
		// Added a timed call to the lua but only a 1/4 sec intervals. (Should be used for decision making)
		// Check if the node is in walk state, so update the walk at 1/60 intervals (animations need 1/60 check)
		// Check for culling on a node and don't update it if it's culled.


		//check if the node is culled -- Disable node culling for the moment (12/08/12) Some character have weird moves
		//culled = App::getInstance()->getDevice()->getSceneManager()->isCulled(this->getNode());
		//if (!nodeLuaCulling && culled)
		//	setAnimation("idle");

		// This is for the LUA move command. Refresh and update the position of the mesh (Now refresh of this is 1/60th sec)
		//old code: if (currentAnimation==OBJECT_ANIMATION_WALK && !culled && (timerobject-timerLUA>17) && (objectType!=OBJECT_TYPE_PLAYER)) // 1/60 second

		//
		// Check and update the walking of the object
		//if ((currentAnimation==OBJECT_ANIMATION_WALK || currentAnimation==OBJECT_ANIMATION_RUN) && !culled)
		if ((currentAnimation==OBJECT_ANIMATION_WALK || OBJECT_ANIMATION_RUN) && !culled)
		{ // timerLUA=17
		
			updateWalk();
			if (objectType==OBJECT_TYPE_PLAYER)
				CameraSystem::getInstance()->updateGameCamera(); //Update camera at each update cycle of the player (better sync)

			if (currentSpeed!=0)
				timerLUA=timerobject;
		}
	}

	//300ms second evaluate the LUA scripts
	if((timerobject-timerAnimation>300) && enabled) // Lua UPdate to 1/4 second (300)
	{
		if (!nodeLuaCulling)
		{// Special abilitie of the object. this will overide the culling refresh
			if (!culled)
			{
				luaRefresh();
				timerAnimation = timerobject;
			}
		} else
		{// if not then check if the node is culled to refresh
			luaRefresh();
			timerAnimation = timerobject;
		}
	}
	// Special timer check for animation states, will trigger after a time has passed
	// Special timer to init when the character is dead for 5 seconds
	if (this->enabled)
	{
		if((this->currentAnimation==OBJECT_ANIMATION_DIE) && (timerobject-timerDie>5000) && (this->getType()!=DynamicObject::OBJECT_TYPE_PLAYER) || (!diePresent))
		{
			// Init the despawn timer
			this->setAnimation("despawn");
			timerDespawn = timerobject;
		}
	}
	// Special timer to init when the character is being despawned (5 seconds)
	// This can be overided if the character don't have a die or despawn animation
	if (!despawnPresent && isEnabled())
	{
		//printf("No despawn Anim, we should see the disabling now!\n");
		setAnimation("prespawn");
		this->setEnabled(false);
		return;
	}


	// Check and update after the despawn of the character.
	if ((this->currentAnimation==OBJECT_ANIMATION_DESPAWN && this->isEnabled()))
	{
		if (timerobject-timerDespawn>5000)
		{
			//printf("Done despawn, disabling the character now!\n");
			// will disable the character after 5 seconds
			setAnimation("prespawn");
			this->setEnabled(false);

		}
	}

	// Stun state of the character, evaluation and refresh
	if (stunstate)
	{
		// 400 ms default delay for hurt (old value was 400)
		if (timerobject-timerStun>150)
		{
			// Disable the stun state and restore the previous animation
			stunstate=false;
			//setAnimation(this->oldAnimName);
			setAnimation("idle");
		}
	}

	if (attackdelaystate)
	{
		if (timerobject-this->timer_attackdelay>this->properties.attackdelay)
		{
			attackdelaystate=false;
			//setAnimation(this->oldAnimName);
			setAnimation("idle");
			//printf("Attack delay for %s is %d mil\n",this->getName().c_str(),this->properties.attackdelay);
		}

	}

	// Call the animation blending ending loop Doesnt work in 1.8.0, need to have a patch for it in 1.8.1
	//if (this->objectType==OBJECT_TYPE_NPC || this->objectType==OBJECT_TYPE_PLAYER)

	if (this->objectType==OBJECT_TYPE_PLAYER) // || this->objectType==OBJECT_TYPE_NPC)
		((IAnimatedMeshSceneNode*)this->getNode())->setTransitionTime(0.35f);
}

void DynamicObject::updateRotation()
{
	vector3df oldrot = getRotation();

	u32 currentime=App::getInstance()->getDevice()->getTimer()->getRealTime();
	u32 elapsedtime=currentime-rotationcounter;

	f32 interp = (f32)(rotationtime-elapsedtime)/(f32)rotationtime;

	if (elapsedtime>rotationtime)
	{
		rotationupdater=false;
		return;
	}

	vector3df finalrotation = rotfrom.getInterpolated(rotto,interp);
	setRotation(finalrotation);

}

void DynamicObject::updateWalk()
{
	// Trick to not account for the Y axis when checking for the distance.
	walkTarget.Y=this->getPosition().Y;


	if (objectType==OBJECT_TYPE_NPC || objectType==OBJECT_TYPE_PLAYER || objectType==OBJECT_TYPE_LOOT)
	{
		// Stop the walk when in range
		//if ((this->getAnimation()==OBJECT_ANIMATION_WALK || this->getAnimation()==OBJECT_ANIMATION_RUN ) && (this->getPosition().getDistanceFrom(walkTarget) < ((meshScale*meshSize)*2) || collided))

		//This will find the size of the ennemy and calculate a proper distance before the attack.

		f32 objectsize = this->getObjectSize(false); // This only get the character size (Z axis)
		if (enemyUnderAttack || objectType==OBJECT_TYPE_NPC) // The object size is calculated from both (defender+attacker sizes (Z axis) and give the distance
		{	
			objectsize = this->getObjectSize()*0.5f;
		}

		// This tries to stop the NPC or player if it reach the proper distance
		// This is needed to stop the character before combat or when it reach the destination
		if (this->getAnimation()==OBJECT_ANIMATION_WALK || this->getAnimation()==OBJECT_ANIMATION_RUN)
		{



			if (objectType==OBJECT_TYPE_PLAYER && !Player::getInstance()->getTaggedTarget())
			{
				vector<DynamicObject*> list=DynamicObjectsManager::getInstance()->getObjectNearPosition(this->getPosition(),objectsize, OBJECT_TYPE_NPC);
				if (list.size()>0)
				{
					if (list[0]->getLife()>0)
					{ 
						Player::getInstance()->setTaggedTarget(list[0]);
						//enemyUnderAttack=list[0];
						this->setWalkTarget(this->getPosition());
						this->setAnimation("idle");
						printf("Stop the PLAYER!\n");
						return;
					}
					
				} 
			}
			//New trick check in radius of the walking for a target. If yes then stop and set the attack
			if (objectType==OBJECT_TYPE_NPC)
			{
				vector<DynamicObject*> list=DynamicObjectsManager::getInstance()->getObjectNearPosition(this->getPosition(),objectsize, OBJECT_TYPE_PLAYER);
				if (list.size()>0)
				{
					//enemyUnderAttack=list[0]; //Got bugs if this is not used by the player. Would need to be used by everyone.
					this->setWalkTarget(this->getPosition());
					this->setAnimation("idle");
					printf("Stop the NPC!\n");
					return;
				}
			}














			//This will stop the player if he reach the target on the ground
			if (!enemyUnderAttack && this->getPosition().getDistanceFrom(walkTarget) < 1 && this->objectType==OBJECT_TYPE_PLAYER) 
			{
				this->setWalkTarget(this->getPosition());
				this->setAnimation("idle");

				DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
				return;
			}
			// This will stop the NPC if it get in a "correct" range of the player
			else if ((!enemyUnderAttack && this->getPosition().getDistanceFrom(walkTarget) < objectsize && this->objectType==OBJECT_TYPE_NPC) &&  // NPC
				(!enemyUnderAttack && this->getPosition().getDistanceFrom(walkTarget) < objectsize*0.75f && this->objectType==OBJECT_TYPE_NPC))
			{
				this->setWalkTarget(this->getPosition());
				this->setAnimation("idle");
				return;
			}
			
			else if ((getPosition().getDistanceFrom(walkTarget) < objectsize) && enemyUnderAttack) // triggered mostly for the player
			{
				this->setWalkTarget(this->getPosition());
				this->setAnimation("idle");

				// For the player, hides the target if get to the destination
				if (objectType==OBJECT_TYPE_PLAYER)
					DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
				return;
			}
		}
		
		// If the object is not near the destination point, then move in that direction
		if( ((this->getPosition().getDistanceFrom(walkTarget) > objectsize*1.10f) &&  (this->getLife()!=0))) // ||
		{	
			//if (objectType==OBJECT_TYPE_NPC)
				//printf ("DEBUG: Object position is now: %f,%f,%f\n      walktarget is set at: %f,%f,%f\n",
				//this->getPosition().X,this->getPosition().Y,this->getPosition().Z,
				//this->walkTarget.X,this->walkTarget.Y,this->walkTarget.Z);
			{
				if (runningMode)
				{
					if (this->getAnimation()!=OBJECT_ANIMATION_RUN)
					{
						this->setAnimation("run");
					}
				}
				else
				{
					if (this->getAnimation()!=OBJECT_ANIMATION_WALK)
					{
						this->setAnimation("walk");
					}
				}
			}
		// This will try to reposition the NPC to get to a better position because its too near of it.
		} else if ((this->getPosition().getDistanceFrom(walkTarget) < objectsize*0.75f) &&  (this->getPosition().getDistanceFrom(walkTarget) > 1 ) && (this->getLife()!=0))
		{
			if (objectType==OBJECT_TYPE_NPC && objectType==OBJECT_TYPE_PLAYER)
			{
#ifdef DEBUG
				printf("Reposition character named: %s \n",getName().c_str());
#endif

				// Trie to recalculate the proper distance position based on the current position
				vector3df pos1 = (walkTarget-getPosition()).normalize(); //Get the directionnal vector and normalize it
				vector3df pos2 = getPosition()+(pos1*(objectsize*0.75f));
				setWalkTarget(pos2);
				setPosition(pos2);

				lookAt(Player::getInstance()->getNode()->getPosition());

				if (runningMode)
				{
					if (this->getAnimation()!=OBJECT_ANIMATION_RUN)
					{
						this->setAnimation("run");
					}
				}
				else
				{
					if (this->getAnimation()!=OBJECT_ANIMATION_WALK)
					{
						this->setAnimation("walk");
					}
				}
			}

		}


		// Move the character only if this character is doing the walk or run animation
		if (this->getAnimation()==OBJECT_ANIMATION_WALK || this->getAnimation()==OBJECT_ANIMATION_RUN)
			this->walkTo(walkTarget);

	}



}

void DynamicObject::luaRefresh()
{

	if (!ls)
	{
		GUIManager::getInstance()->setConsoleText("LUA error, lost pointer",SColor(255,255,0,0));
		return;
	}

	if (App::getInstance()->getAppState() > 100 && ls)
	{//app_state < APP_STATE_CONTROL
		lua_getglobal(ls,"onUpdate");
		if(lua_isfunction(ls, -1))
		{
			if (lua_pcall(ls,0,0,0)!=0)
			{
				GUIManager::getInstance()->setConsoleText("LUA error running funtion <<onUpdate>>",SColor(255,255,0,0));
			}	
		}
		lua_pop( ls, -1 );

		
		lua_getglobal(ls,"step");
		if(lua_isfunction(ls, -1))
		{
			if (lua_pcall(ls,0,0,0)!=0)
			{
				GUIManager::getInstance()->setConsoleText("LUA error running funtion <<step>>",SColor(255,255,0,0));
			}
		}
		lua_pop( ls, -1 );
		

		//custom update function (updates walkTo for example..)
		lua_getglobal(ls,"CustomDynamicObjectUpdate");
		if(lua_isfunction(ls, -1))
		{
			if (lua_pcall(ls,0,0,0)!=0)
			{
				GUIManager::getInstance()->setConsoleText("LUA error running funtion <<CustomDynamicObjectUpdate>>",SColor(255,255,0,0));
			}
		}
		lua_pop( ls, -1 );
		
	}
	lua_getglobal(ls,"CustomDynamicObjectUpdateProgrammedAction");
	if(lua_isfunction(ls, -1))
		lua_pcall(ls,0,0,0);
	lua_pop( ls, -1 );
}


//LUA FUNCTIONS

int DynamicObject::getEnemyCount(lua_State *ls)
{
	int number=DynamicObjectsManager::getInstance()->getEnemyCount();
	lua_pushnumber(ls,number);
	return 1;

}

int DynamicObject::setEnabled(lua_State *ls)
{
    int LUAenabled = lua_toboolean(ls, -1);

	bool enabled = false;
	if (LUAenabled==1)
		enabled=true;

	lua_pop(ls, 1);

	///TODO: create getObjectByName() as static to avoid code duplication!
	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);
	///================================

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    tempObj->setEnabled(enabled);

	return 0;
}

int DynamicObject::hasReached(lua_State *ls)
{
	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
		lua_pushboolean(ls, tempObj->reached);
    }

    return 1;
}

int DynamicObject::setPosition(lua_State *ls)
{
    float z = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	float y = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	float x = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);


	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);
	DynamicObjectsManager::getInstance()->getObjectByName(objName)->getNode()->updateAbsolutePosition();
    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setPosition(vector3df(x,y,z));

    return 0;
}

int DynamicObject::setRotation(lua_State *ls)
{
	float z = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	float y = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

    float x = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
    if(tempObj) tempObj->setRotation(vector3df(x,y,z));

    return 0;
}

int DynamicObject::getPosition(lua_State* ls)
{
    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);
	if(objName.c_str() == "player")
        {
            vector3df pos = Player::getInstance()->getObject()->getPosition();
			lua_pushnumber(ls,pos.X);
			lua_pushnumber(ls,pos.Y);
			lua_pushnumber(ls,pos.Z);
        }
	else
		{
			DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
		    if(tempObj)
			{
				 vector3df pos = tempObj->getPosition();

				lua_pushnumber(ls,pos.X);
				lua_pushnumber(ls,pos.Y);
				lua_pushnumber(ls,pos.Z);
			}
		}

    return 3;
}

int DynamicObject::getRotation(lua_State* ls)
{
    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        vector3df rot = tempObj->getRotation();

        lua_pushnumber(ls,rot.X);
        lua_pushnumber(ls,rot.Y);
        lua_pushnumber(ls,rot.Z);
    }

    return 3;
}

int DynamicObject::turn(lua_State *ls)//turn(degrees)
{
    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	float angle = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        tempObj->setRotation(tempObj->getRotation() + vector3df(0,angle,0));
    }

    return 0;
}

int DynamicObject::move(lua_State *ls)//move(speed)
{
    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	float speed = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);


    if(tempObj)
    {
		if (tempObj->runningMode)
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_RUN)
				tempObj->setAnimation("run");
		}
		else
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_WALK)
				tempObj->setAnimation("walk");
		}

		//printf ("Lua call the walk animation.\n");

		tempObj->moveObject(speed);
    }

    return 0;
}

int DynamicObject::walkToLUA(lua_State *ls)
{
	float z = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	float y = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

    float x = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);


    if(tempObj)
    {
		/*if (tempObj->runningMode)
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_RUN)
				tempObj->setAnimation("run");
		}
		else*/
		{
			if (tempObj->getAnimation()!=OBJECT_ANIMATION_WALK)
				tempObj->setAnimation("walk");
		}
		tempObj->setWalkTarget(vector3df(x,y,z));

		// Determine if the object is the player or the NPC (will set the player as target)
		// Code need to be reworked as the NPCs are blocked
		/*if (tempObj->getType()!=OBJECT_TYPE_PLAYER)
			tempObj->enemyUnderAttack=Player::getInstance()->getObject();*/
    }

    return 0;
}

int DynamicObject::lookAt(lua_State *ls)
{
	float z = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	float y = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

    float x = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        tempObj->lookAt(vector3df(x,y,z));
    }

    return 0;
}

int DynamicObject::lookToObject(lua_State *ls)
{
    stringc otherObjName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);


    vector3df otherObjPosition;

    if(otherObjName == "player")
    {
		otherObjPosition = Player::getInstance()->getObject()->getPosition();
    }
    else
    {
        DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherObjName.c_str()).c_str());

        otherObjPosition = otherObj->getPosition();
    }


    tempObj->lookAt(otherObjPosition);


    return 0;
}


// LUA command "attack("object name")
// Will use the combat manager
int DynamicObject::attackObj(lua_State *ls)
{
	stringc otherObjName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	DynamicObject* tempObj = NULL;
	if(otherObjName != "player")
    {
		tempObj = DynamicObjectsManager::getInstance()->getObjectByName(otherObjName);
	}
	else
	{
		tempObj = Player::getInstance()->getObject();
	}
	if (tempObj)
	{
		DynamicObject* tempObj2 = NULL;
		tempObj2 = DynamicObjectsManager::getInstance()->getObjectByName(objName);
		//printf("The LUA use attack with that target: %s\n",tempObj->getName().c_str());
		tempObj2->attackresult=Combat::getInstance()->attack(DynamicObjectsManager::getInstance()->getObjectByName(objName),tempObj);
		if (tempObj2->attackresult>0)
			tempObj2->setAnimation("attack");
		else
			tempObj2->setAnimation("idle");
	}

	return 0;
}

int DynamicObject::setProperty(lua_State *ls)
{
	float value = (float)lua_tonumber(ls, -1);
	lua_pop(ls, 1);



	stringc propertieName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	// I would like to specify the object name so lua could set the properies of another object (another command?)
	//stringc otherObjName = lua_tostring(ls, -1);
	//lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	//printf("Set propertie %s to %f from object named: %s\n",propertieName,value,objName.c_str());

	DynamicObject* Obj = NULL;
	if (objName!="")
		Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName.c_str());
	else
		Obj = DynamicObjectsManager::getInstance()->getPlayer();

	if (propertieName=="life")
		Obj->properties.life = (int)value;
	if (propertieName=="maxlife")
		Obj->properties.maxlife = (int)value;
	if (propertieName=="mindamage")
		Obj->properties.mindamage = (int)value;
	if (propertieName=="maxdamage")
		Obj->properties.maxdamage = (int)value;
	if (propertieName=="hurt_resist")
		Obj->properties.hurt_resist = (int)value;
	if (propertieName=="experience")
		Obj->properties.experience = (int)value;
	if (propertieName=="dodge_prob")
		Obj->properties.dodge_prop = (f32)value;
	if (propertieName=="hit_prob")
		Obj->properties.hit_prob = (f32)value;
	if (propertieName=="mana")
		Obj->properties.mana = (int)value;
	if (propertieName=="maxmana")
		Obj->properties.maxmana = (int)value;
	if (propertieName=="money")
		Obj->properties.money = (int)value;
	if (propertieName=="dotduration")
		Obj->properties.dotduration = (int)value;
	if (propertieName=="armor")
		Obj->properties.armor = (int)value;
	if (propertieName=="magic_armor")
		Obj->properties.magic_armor = (int)value;
	if (propertieName=="regenlife")
		Obj->properties.regenlife = (int)value;
	if (propertieName=="regenmana")
		Obj->properties.regenmana = (int)value;
	if (propertieName=="level")
		Obj->properties.level = (int)value;

	if (objName=="player")
		Player::getInstance()->updateDisplay();

	//if (otherObjName=="me")

	return 0;
}

int DynamicObject::getProperty(lua_State *ls)
{
	stringc propertieName = lua_tostring(ls, -1);
	lua_pop(ls, 1);


	//stringc otherObjName = lua_tostring(ls, -1);
	//lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);

	lua_pop(ls, 1);
	int value = 0;
	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName.c_str());
	if (propertieName=="life")
		value = Obj->properties.life;
	if (propertieName=="money")
		value = Obj->properties.money;
	if (propertieName=="maxlife")
		value = Obj->properties.maxlife;
	if (propertieName=="mana")
		value = Obj->properties.mana;
	if (propertieName=="maxmana")
		value = Obj->properties.maxmana;
	if (propertieName=="mindamage")
		value = Obj->properties.mindamage;
	if (propertieName=="maxdamage")
		value = Obj->properties.maxdamage;
	if (propertieName=="hurt_resist")
		value = Obj->properties.hurt_resist;
	if (propertieName=="experience")
		value = Obj->properties.experience;
	if (propertieName=="level")
		value = Obj->properties.level;
	if (propertieName=="regenlife")
		value = Obj->properties.regenlife;
	if (propertieName=="regenmana")
		value = Obj->properties.regenmana;

	if (propertieName=="dodge_prob")
	{
		float value2 = Obj->properties.dodge_prop;
		lua_pushnumber(ls,value2);
		return 1;
	}
	if (propertieName=="hit_prob")
	{
		float value2 = Obj->properties.hit_prob;
		lua_pushnumber(ls,value2);
		return 1;
	}

	lua_pushnumber(ls,value);

	return 1;
}
int DynamicObject::getNameLUA(lua_State *ls)
{
	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
	//stringc value = Obj->getTemplateObjectName();
	stringc value = (stringc)Obj->internalname;
	lua_pushstring(ls,value.c_str());
	//lua_pushstring(ls,objName.c_str());

	return 1;
}

int DynamicObject::setNameLUA(lua_State *ls)
{
	
	stringc name = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	DynamicObject* Obj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
	//stringc value = Obj->getTemplateObjectName();
	if (Obj)
	{
		Obj->internalname=name;
	}

	return 0;
}
int DynamicObject::setFrameLoop(lua_State *ls)
{
    int start = (int)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	int end = (int)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setFrameLoop(start,end);

    return 0;
}

int DynamicObject::setAnimation(lua_State *ls)
{
    stringc animName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setAnimation(animName);
	return true;
}

int DynamicObject::setAnimationSpeed(lua_State *ls)
{
    f32 speed = (f32)lua_tonumber(ls, -1);
	lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObjectsManager::getInstance()->getObjectByName(objName)->setAnimationSpeed(speed);
	return 0;
}

int DynamicObject::distanceFrom(lua_State *ls)
{
    vector3df otherPos = vector3df(-1000,-1000,-1000);

    if(lua_isnumber(ls, -1))//read (x,y,z)
    {
        float z = (float)lua_tonumber(ls, -1);
        lua_pop(ls, 1);

        float y = (float)lua_tonumber(ls, -1);
        lua_pop(ls, 1);

        float x = (float)lua_tonumber(ls, -1);
        lua_pop(ls, 1);

        otherPos = vector3df(x,y,z);
    }
    else if(lua_isstring(ls,-1))//get distance from an object
    {
        std::string otherName = lua_tostring(ls, -1);
        lua_pop(ls, 1);

		if(otherName.c_str() == "player")
        {
            otherPos = Player::getInstance()->getObject()->getPosition();
        }
        else
        {
            DynamicObject* otherObj = DynamicObjectsManager::getInstance()->getObjectByName(GlobalMap::getInstance()->getGlobal(otherName.c_str()).c_str());

            if(otherObj)
            {
                otherPos = otherObj->getPosition();
            }
        }
    }

    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj)
    {
        lua_pushnumber(ls,otherPos.getDistanceFrom(tempObj->getPosition()));
        return 1;
    }

    return 0;
}

int DynamicObject::showObjectLabel(lua_State *ls)
{
    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj) tempObj->objectLabelSetVisible(true);

    return 0;
}

int DynamicObject::hideObjectLabel(lua_State *ls)
{
    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

    if(tempObj) tempObj->objectLabelSetVisible(false);

    return 0;
}

int DynamicObject::setObjectLabel(lua_State *ls)
{
    std::string newLabel = lua_tostring(ls, -1);
    lua_pop(ls, 1);

    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	if(tempObj) 
	{
		tempObj->setObjectLabel(newLabel.c_str());
		tempObj->displayName=(core::stringw)newLabel.c_str();
	}


    return 0;
}

int DynamicObject::showDialogMessage(lua_State *ls)
{
	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	DynamicObjectsManager::getInstance()->setDialogCaller(tempObj);

	std::string param1 = lua_tostring(ls, -1);
    lua_pop(ls, 1);

    std::string param2 = "";

    if(lua_isstring(ls, -1))
    {
        param2 = lua_tostring(ls, -1);
        lua_pop(ls, 1);
    }

	App::getInstance()->getDevice()->getCursorControl()->setVisible(true);

    if(param2!="")
		GUIManager::getInstance()->showDialogMessage((stringw)param2.c_str(), param1);
    else
		GUIManager::getInstance()->showDialogMessage((stringw)param1.c_str(), "");

    return 1;
}

int DynamicObject::showDialogQuestion(lua_State *ls)
{
	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	DynamicObjectsManager::getInstance()->setDialogCaller(tempObj);

	std::string param1 = lua_tostring(ls, -1);
    lua_pop(ls, 1);

    std::string param2 = "";

    if(lua_isstring(ls, -1))
    {
        param2 = lua_tostring(ls, -1);
        lua_pop(ls, 1);
    }

	App::getInstance()->getDevice()->getCursorControl()->setVisible(true);

    if(param2!="")
		GUIManager::getInstance()->showDialogQuestion((stringw)param2.c_str(), param1);
    else
		GUIManager::getInstance()->showDialogQuestion((stringw)param1.c_str(), "");

    return 1;
}

void DynamicObject::notifyClick()
{
    lua_getglobal(ls,"onClicked");
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop( ls, -1 );
}

void DynamicObject::notifyAttackRange()
{
    lua_getglobal(ls,"onAttackRange");
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop( ls, -1 );
}

void DynamicObject::notifyCollision()
{
    lua_getglobal(ls,"onCollision");
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop(ls, -1 );
}

void DynamicObject::notifyAnswer(bool answer)
{
	LuaGlobalCaller::getInstance()->setAnswer(answer);
	lua_getglobal(ls,"onAnswer");
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop(ls, -1 );
}

void DynamicObject::notifyUse()
{
	lua_getglobal(ls,"onUse");
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop( ls, -1 );
}

void DynamicObject::notifyWear()
{
	lua_getglobal(ls,"onWear");
    if(lua_isfunction(ls, -1)) lua_pcall(ls,0,0,0);
    lua_pop( ls, -1 );
}


stringc DynamicObject::getObjectType()
{
    lua_getglobal(ls,"objType");
    stringc objType = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	return objType;
}

int DynamicObject::setObjectType(lua_State *ls)
{
	core::stringc type = (core::stringc)lua_tostring(ls, -1);
    lua_pop(ls, 1);

    lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

    DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	if(tempObj) tempObj->setType(type);

    return 0;
}

//Add the currently selected object into the player loot.
//This will add an already placed object on the map
//NOTE: Name should be changed to moveObjectLoot
int DynamicObject::addPlayerLoot(lua_State *ls)
{
    stringc target="";
	stringc objName="";
	int top = lua_gettop(ls);
	if (top>0)
	{
		target = lua_tostring(ls, -1);
		lua_pop(ls, 1);
		lua_getglobal(ls,"objName");
		objName = lua_tostring(ls, -1);
		lua_pop(ls, 1);
	} else
	{
		target = "player"; //Default to the player
		lua_getglobal(ls,"objName");
		objName = lua_tostring(ls, -1);
		lua_pop(ls, 1);
	}

	DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	if(tempObj) // Was the object found?
	{
		if (tempObj->getType()==DynamicObject::OBJECT_TYPE_LOOT) // Was the object a loot object?
		{
			DynamicObject* targetobj = DynamicObjectsManager::getInstance()->getObjectByName(target);
			if (targetobj)
			{
				targetobj->addLootItem(tempObj); //Add this pointer object to the object loot
				tempObj->getNode()->setVisible(false); //Hide the node
				tempObj->getNode()->setPosition(core::vector3df(0,0,0)); // Reset the position
				tempObj->getNode()->setParent(Player::getInstance()->getObject()->getNode()); // Parent it to the player
				tempObj->isInBag=true;
			}
		}
	}

    return 0;
}

//!Add the specified template object directly into the loot of the dynamic object
int DynamicObject::addLootLUA(lua_State *ls)
{

	core::stringc tempname = (core::stringc)lua_tostring(ls, -1);
    lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	if(tempObj) // Was the object found?
	{
		if (tempObj->getType()==DynamicObject::OBJECT_TYPE_NPC || tempObj->getType()==DynamicObject::OBJECT_TYPE_PLAYER) // Was the object a loot object?
		{
			DynamicObject* daloot = DynamicObjectsManager::getInstance()->createTemplateAt(tempname,vector3df(0.0f,-2000.0f,0.0f));
			
			if (daloot) //If the user had entered the wrong name in the template, the object will be removed (will generate the current template)
			{
				if (daloot->getType()!=OBJECT_TYPE_LOOT)
				{
					DynamicObjectsManager::getInstance()->removeObject(daloot->getName());
					daloot=NULL;
				}
				
			}
			
			if (daloot) //Name is ok and will generate the object directly in this object loot
			{
				tempObj->addLootItem(daloot); //Add this pointer object to the player loot
				daloot->getNode()->setVisible(false); //Hide the node
				daloot->getNode()->setPosition(core::vector3df(0,0,0)); // Reset the position
				daloot->getNode()->setParent(Player::getInstance()->getObject()->getNode()); // Parent it to the player
				daloot->isInBag=true;
				daloot->isGenerated=true; //Tell IRB that this object was generated ingame.
			}
		}
	}

    return 0;
}

//Will hide, set the life a 0, and drop an object when it's been used. This set the flag.
int DynamicObject::destroyAfterUse(lua_State *LS)
{
	
	int valueinput = lua_toboolean(LS, -1);
	bool value = false;
	if (valueinput>0)
		value = true;

    lua_pop(LS, 1);

	lua_getglobal(LS,"objName");
	stringc objName = lua_tostring(LS, -1);
	lua_pop(LS, 1);

	DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);
	tempObj->isDestroyedAfterUse=value;
	
	return 0;
}

//!Will splill the loot on the terrain. Called when the character dies
void DynamicObject::splillLoot()
{
	if (lootitems.size()>0)
	{
		for(int i=0;i<(int)lootitems.size();i++)
		{
			if (lootitems[i])
			{
				vector3df prepos = getNode()->getPosition(); // Get the parent position
				int randomx = rand() % 50; int randomz = rand() %50; randomx-=25; randomz-=25;
				printf("Random gerenrated number is: %i, %i\n",randomx,randomz);

				prepos += vector3df(f32(randomx),0.0f,f32(randomz)) ;  //50 in random position offset X and Z
				lootitems[i]->getNode()->setParent(smgr->getRootSceneNode()); //put back on the floor		
			
				f32 height = TerrainManager::getInstance()->getHeightAt(prepos); //Set the new position (but should touch the floor)
				prepos.Y=height;
				lootitems[i]->getNode()->setPosition(prepos);
				lootitems[i]->getNode()->setRotation(vector3df(0,f32(rand() % 360),0)); //Random y angle
				lootitems[i]->getNode()->setVisible(true);
				lootitems[i]->isInBag=false;
				printf("This object: %s is dropping this object: %s \n",this->getName().c_str(),lootitems[i]->getName().c_str());

			}
		}
		//Clear the list
		lootitems.clear();
	}

}

int DynamicObject::setEnemy(lua_State *ls)
{
	bool isenemy = bool(lua_toboolean(ls, -1));
    lua_pop(ls, 1);

	lua_getglobal(ls,"objName");
	stringc objName = lua_tostring(ls, -1);
	lua_pop(ls, 1);

	DynamicObject* tempObj = DynamicObjectsManager::getInstance()->getObjectByName(objName);

	if(tempObj) 
	{
		tempObj->isEnemy=isenemy;
	}

	return 0;
}