#include "CameraSystem.h"

#include "../sound/SoundManager.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;




CameraSystem::CameraSystem()
{
	//irr::f32 CameraSystem::cameraHeight = 4.0f;
	lightset=false;
	this->light=NULL;
	this->sun=NULL;
	camera=CAMERA_EDIT;

	// Create the cutscene camera
	cutsceneCam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
	cutsceneCam->setFarValue(5000);
	cutsceneCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	
	// Create the ingame camera (RTS mecanic)
	gameCam = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode();
	gameCam->setFarValue(5000);
	gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	
	setViewType(VIEW_RTS);
	controltype = CONTROL_POINTNCLICK;
	cameraAngle=vector3df(135.0f,45.0f,0.0f);
	

	// New edit camera
	editCamMaya = addCameraSceneNodeMaya(0, -450.0f, 800.0f, 400.0f);
	editCamMaya->setFarValue(5000);
	editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
	editCamMaya->setPosition(vector3df(0,1000,-1000));
	editCamMaya->setTarget(vector3df(0,0,0));
	setCamera(camera);

    oldrot=-90+Player::getInstance()->getObject()->getRotation().Y;

	initrotation=false;
	oldmouse=vector2df(0,0);
	initangle=vector2d<f32>(0,0); //Initialize the initial angle of the RTS camera (Calculated from here)

	//Set default values for the ingame camera range
	this->gameCamRangeMin=72;
	this->gameCamRangeMax=2000;

	//Set the default angle limits for the ingame camera (up/down angle)
	cameraAngleLimit.X=-25.0f;
	cameraAngleLimit.Y=89.0f;

	//define the keys for the camera system (not fully implemented as 08/14)
	//Will need a way to redefine thoses in LUA/GUI
	keyforward=KEY_KEY_W;
	keybackward=KEY_KEY_S;
	keyleftside=KEY_KEY_A;
	keyrightside=KEY_KEY_D;
	keyaction=EMIE_LMOUSE_PRESSED_DOWN;
	keyinteraction=KEY_KEY_E;
	keyinventory=KEY_KEY_I;

	moveforward=false;
	movebackward=false;
	moveleftside=false;
	moverightside=false;

	device=App::getInstance()->getDevice();

	
	
}

CameraSystem::~CameraSystem()
{
	if (anm)
		anm->drop();

	if (cutsceneCam)
		cutsceneCam->drop();

	if (gameCam)
		gameCam->drop();

	if (editCamMaya)
		editCamMaya->drop();

    //dtor
}

CameraSystem* CameraSystem::getInstance()
{
    static CameraSystem *instance = 0;
    if (!instance) instance = new CameraSystem();
    return instance;
}

void CameraSystem::defineKeys(core::stringc key, core::stringc action)
{
	irr::s32 keyfinal=0;
	 // First define the key value
	if (key=="A")
		keyfinal = KEY_KEY_A;
	else if (key=="B")
		keyfinal = KEY_KEY_B;
	else if (key=="C")
		keyfinal = KEY_KEY_C;
	else if (key=="D")
		keyfinal = KEY_KEY_D;
	else if (key=="E")
		keyfinal = KEY_KEY_E;
	else if (key=="F")
		keyfinal = KEY_KEY_F;
	else if (key=="G")
		keyfinal = KEY_KEY_G;
	else if (key=="H")
		keyfinal = KEY_KEY_H;
	else if (key=="I")
		keyfinal = KEY_KEY_I;
	else if (key=="J")
		keyfinal = KEY_KEY_J;
	else if (key=="K")
		keyfinal = KEY_KEY_K;
	else if (key=="L")
		keyfinal = KEY_KEY_L;
	else if (key=="M")
		keyfinal = KEY_KEY_M;
	else if (key=="N")
		keyfinal = KEY_KEY_N;
	else if (key=="O")
		keyfinal = KEY_KEY_O;
	else if (key=="P")
		keyfinal = KEY_KEY_P;
	else if (key=="Q")
		keyfinal = KEY_KEY_Q;
	else if (key=="R")
		keyfinal = KEY_KEY_R;
	else if (key=="S")
		keyfinal = KEY_KEY_S;
	else if (key=="T")
		keyfinal = KEY_KEY_T;
	else if (key=="U")
		keyfinal = KEY_KEY_U;
	else if (key=="V")
		keyfinal = KEY_KEY_V;
	else if (key=="W")
		keyfinal = KEY_KEY_W;
	else if (key=="X")
		keyfinal = KEY_KEY_X;
	else if (key=="Y")
		keyfinal = KEY_KEY_Y;
	else if (key=="Z")
		keyfinal = KEY_KEY_Z;
	else if (key=="1")
		keyfinal = KEY_KEY_1;
	else if (key=="2")
		keyfinal = KEY_KEY_2;
	else if (key=="3")
		keyfinal = KEY_KEY_3;
	else if (key=="4")
		keyfinal = KEY_KEY_4;
	else if (key=="5")
		keyfinal = KEY_KEY_5;
	else if (key=="6")
		keyfinal = KEY_KEY_6;
	else if (key=="7")
		keyfinal = KEY_KEY_7;
	else if (key=="8")
		keyfinal = KEY_KEY_8;
	else if (key=="9")
		keyfinal = KEY_KEY_9;
	else if (key=="0")
		keyfinal = KEY_KEY_0;
	
	// get out if the key is not known
	if (keyfinal==0)
		return;
	
	if (action=="FORWARD")
		keyforward=keyfinal;
	else if(action=="BACK")
		keybackward=keyfinal;
	else if(action=="LEFT")
		keyleftside=keyfinal;
	else if(action=="RIGHT")
		keyrightside=keyfinal;
	else if(action=="INVENTORY")
		keyinventory=keyfinal;
	else if(action=="INTERACTION")
		keyinteraction=keyfinal;

}

//! Get back the events from the keyboard (called in the APP Class -> EventManager)
void CameraSystem::eventsKeyboard(s32 key)
{
	if (key==keyforward) //Use the current game mecanics to move the player (define a walktarget position)
	{
		if (viewtype==VIEW_RPG || viewtype==VIEW_FPS)
		{
			moveforward=true;
			vector3df delta;

			if (viewtype!=VIEW_RPG)
			{
				delta=vector3df(0,0,150);
				delta.rotateXZBy(Player::getInstance()->getNode()->getRotation().Y);
			}
			else
			{
				delta=vector3df(0,0,-150);
				delta.rotateXZBy(currentCam->getRotation().Y);
			}


			delta.Z=-delta.Z;

			vector3df endposition = Player::getInstance()->getNode()->getPosition()+delta;

			Player::getInstance()->getObject()->setWalkTarget(endposition);
		}
		return;

	}
	else if (key==keybackward)
	{
		if (viewtype==VIEW_RPG || viewtype==VIEW_FPS)
		{
			movebackward=true;
			vector3df delta;
			
			if (viewtype!=VIEW_RPG)
			{
				delta=vector3df(0,0,-150);
				delta.rotateXZBy(Player::getInstance()->getNode()->getRotation().Y);				
			}
			else
			{
				delta=vector3df(0,0,150);
				delta.rotateXZBy(currentCam->getRotation().Y);
			}

			delta.Z=-delta.Z;

			vector3df endposition = Player::getInstance()->getNode()->getPosition()+delta;

			Player::getInstance()->getObject()->setWalkTarget(endposition);
			
		}
		return;
	}
	else if (key==keyleftside)
	{
		if (viewtype==VIEW_FPS || viewtype==VIEW_RPG)
		{
			moveleftside=true;
			vector3df delta;
		
			if (viewtype!=VIEW_RPG)
			{
				delta=vector3df(150,0,0);
				delta.rotateXZBy(Player::getInstance()->getNode()->getRotation().Y);
			}
			else
			{
				delta=vector3df(-150,0,0);
				delta.rotateXZBy(currentCam->getRotation().Y);
			}

			delta.Z=-delta.Z;

			vector3df endposition = Player::getInstance()->getNode()->getPosition()+delta;

			Player::getInstance()->getObject()->setWalkTarget(endposition);
		}
	}
	else if (key==keyrightside)
	{
		if (viewtype==VIEW_FPS || viewtype==VIEW_RPG)
		{
			moverightside=true;
			vector3df delta;

			if (viewtype!=VIEW_RPG)
			{
				delta=vector3df(-150,0,0);
				delta.rotateXZBy(Player::getInstance()->getNode()->getRotation().Y);
			}
			else
			{
				delta=vector3df(150,0,0);
				delta.rotateXZBy(currentCam->getRotation().Y);
			}

			delta.Z=-delta.Z;

			vector3df endposition = Player::getInstance()->getNode()->getPosition()+delta;

			Player::getInstance()->getObject()->setWalkTarget(endposition);
		}
	}
	else if (key==keyinteraction)
	{
		printf("Interaction key was triggered!\n");
		if (Player::getInstance()->getTaggedTarget())
			Player::getInstance()->getTaggedTarget()->notifyClick();

	}
	else if (key==KEY_SPACE)
	{
		device->getCursorControl()->setVisible(!device->getCursorControl()->isVisible());
	}
	else if (key==KEY_ESCAPE) // Stop the game
	{
		App::getInstance()->stopGame();
	}
	else if (key==keyinventory) //Open the inventory
	{
		App::getInstance()->openItemsPanel();
		device->getCursorControl()->setVisible(true);
	}
	
	
	

}

//! Get back the events from the mouse (called in the APP Class -> EventManager)
void CameraSystem::eventsMouseKey(s32 key)
{
	if (key==keyaction)
	{
		printf("Action key was triggered!\n");
		//Will attack if there is a "tagged" object
		if (viewtype!=VIEW_RTS || viewtype!=VIEW_RTS_FIXED)
		{
			if (Player::getInstance()->getTaggedTarget())
			{
				//if (Player::getInstance()->getTaggedTarget()->getObjectType() == stringc("ENEMY"))
					Player::getInstance()->getObject()->attackEnemy(Player::getInstance()->getTaggedTarget());
			}
			else
				Player::getInstance()->getObject()->clearEnemy();
		}

	} else if (key==EMIE_RMOUSE_PRESSED_DOWN)
	{
		if (viewtype==VIEW_RTS || viewtype==VIEW_RTS_FIXED || viewtype==VIEW_RPG)
		{//Hide the mouse pointer while rotation is done
			initrotation=true;
			device->getCursorControl()->setVisible(false);
			initangle.X=getAngle().X;
			initangle.Y=getAngle().Y;
			oldmouse = device->getCursorControl()->getRelativePosition();
			printf("The right mouse button was pressed!");
		}
	} else if (key==EMIE_RMOUSE_LEFT_UP)
	{
		if (viewtype==VIEW_RTS || viewtype==VIEW_RTS_FIXED || viewtype==VIEW_RPG)
		{
			device->getCursorControl()->setVisible(true);
			initrotation=false;
		}
	}


}

void CameraSystem::eventMouseWheel(f32 value)
{
	if (viewtype!=VIEW_FPS)
		setCameraHeight(value);
}

//! Set the camera (ingame or edit or cutscene)
void CameraSystem::setCamera(CAMERA_TYPE tempCamera)
{
	camera = tempCamera;

	switch (camera)
	{
		// Camera 1 - Gameplay
		case CAMERA_GAME: fov=0.85f;
				gameCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				cameraHeight = 600.0f;
				cameraAngle = vector3df(135.0f,45.0f,0);
				gameCam->setRotation(cameraAngle);
				cameraRotationSpeed = 15.0f;
				cameraTargetHeight = 48.0f;
				currentCam = gameCam;
				gameCam->bindTargetAndRotation(true);
				this->updatePointClickCam();

				// Use the RTS view by default
				viewtype=VIEW_RTS;
				break;

		// Camera 2 - Editing
		case CAMERA_EDIT: fov=0.45f;
				cameraHeight = 1000.0f;
				currentCam = editCamMaya;
				editCamMaya->setUpVector(vector3df(0,1,0));
		        editCamMaya->setPosition(vector3df(0.0f,1000.0f,-1000.0f));
				editCamMaya->setTarget(vector3df(0.0f,0.0f,0.0f));

				//editCamMaya->setInputReceiverEnabled(false);
				editCamMaya->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				editCamMaya->setInputReceiverEnabled(true);
				//editCamMaya->setInputReceiverEnabled(false);
				editCamMaya->setFarValue(cameraHeight*3.0f);
	
				break;

		// Camera 3 - Cutscene
		case CAMERA_CUTSCENE: fov=0.45f;
				cutsceneCam->setAspectRatio((f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Width/
				(f32)App::getInstance()->getDevice()->getVideoDriver()->getScreenSize().Height);
				currentCam=cutsceneCam;
				break;
	}
	App::getInstance()->getDevice()->getSceneManager()->setActiveCamera(currentCam);

	currentCam->setFOV(fov);
	currentCam->setTarget(currentCam->getTarget());

	// Add a specular light to the camera.
	if (lightset)
	{
		SColorf color = App::getInstance()->getDevice()->getSceneManager()->getAmbientLight();
		//light = App::getInstance()->getDevice()->getSceneManager()->addLightSceneNode(gameCam,vector3df(0,0,250),video::SColorf(0.5f,0.5f,0.6f),250);
		//sun = App::getInstance()->getDevice()->getSceneManager()->addLightSceneNode(0,vector3df(0,500,0),color,250);
		lightset=true;
	}
}

CameraSystem::CAMERA_TYPE CameraSystem::getCamera()
{
	return camera;
}

//Update the camera
void CameraSystem::updateGameCamera()
{

	//vector3df rotation = currentCam->getRotation();
	//printf("Here are the current camera rotation info: %f,%f,%f\n",rotation.X,rotation.Y,rotation.Z);
	if (initrotation && (viewtype==VIEW_RTS || viewtype==VIEW_RPG)) 
		findCamAngle();

	// This would stop the player when it's moving and the key is released
	// This should be improved to check is ANY other keys are pressed
	// With this currently when we press a key and press another one, it's stop the player.
	if (moveforward && !App::getInstance()->isKeyPressed(keyforward)) // Stop the player if the forward key is released
	{
		moveforward=false;
		Player::getInstance()->getObject()->setWalkTarget(Player::getInstance()->getNode()->getPosition());
	}
	if (movebackward && !App::getInstance()->isKeyPressed(keybackward)) // Stop the player if the forward key is released
	{
		movebackward=false;
		Player::getInstance()->getObject()->setWalkTarget(Player::getInstance()->getNode()->getPosition());
	}
	if (moveleftside && !App::getInstance()->isKeyPressed(keyleftside)) // Stop the player if the forward key is released
	{
		moveleftside=false;
		Player::getInstance()->getObject()->setWalkTarget(Player::getInstance()->getNode()->getPosition());
	}
	if (moverightside && !App::getInstance()->isKeyPressed(keyrightside)) // Stop the player if the forward key is released
	{
		moverightside=false;
		Player::getInstance()->getObject()->setWalkTarget(Player::getInstance()->getNode()->getPosition());
	}

	switch (viewtype)
	{
	case VIEW_RTS:
		{
			updatePointClickCam();
			break;
		}
	case VIEW_RTS_FIXED:
		{
			updatePointClickCam();
			break;
		}
	case VIEW_RPG:
		{
			if (device->getCursorControl()->isVisible() && App::getInstance()->getAppState()==App::APP_GAMEPLAY_NORMAL)
				device->getCursorControl()->setVisible(false);
			//updatePointClickCam();
			updateRPGCamera();
			//updatePlayerRotation();
			break;
		}
	case VIEW_FPS:
		{
			if (device->getCursorControl()->isVisible() && App::getInstance()->getAppState()==App::APP_GAMEPLAY_NORMAL)
				device->getCursorControl()->setVisible(false);
			updatePlayerRotation();
			updateFPSCamera();
			
			break;
		}
	}
}

// Note: Taken and modified from IRRlicht
//! Adds a camera scene node which is able to be controlled with the mouse similar
//! to in the 3D Software Maya by Alias Wavefront.
//! The returned pointer must not be dropped.
ICameraSceneNode* CameraSystem::addCameraSceneNodeMaya(ISceneNode* parent,
	f32 rotateSpeed, f32 zoomSpeed, f32 translationSpeed, s32 id, f32 distance,
	bool makeActive)
{
	ICameraSceneNode* node = App::getInstance()->getDevice()->getSceneManager()->addCameraSceneNode(parent, core::vector3df(),
			core::vector3df(0,0,100), id, makeActive);
	if (node)
	{
		anm = new CSceneNodeAnimatorCameraMayaIRB(App::getInstance()->getDevice()->getCursorControl(),
			rotateSpeed, zoomSpeed, translationSpeed, distance);

		node->addAnimator((ISceneNodeAnimator*)anm);
		
	}

	return node;
}

void CameraSystem::setCameraHeight(irr::f32 increments)
{
	f32 max = 0;
	f32 min = 0;
	
	switch (camera)
	{
		/*case 1: max = 6;
				min = 2;
				break;*/
		case CAMERA_GAME: max = gameCamRangeMax;
				min = gameCamRangeMin;
				gameCam->setFarValue(10000.0f);
				gameCam->setNearValue(1.0f);
				break;
		case CAMERA_EDIT: max = 30000;
				min = 30;
				editCamMaya->setFarValue(55000.0f);
				editCamMaya->setNearValue(1.0f);
				break;
	}

	if (camera==CAMERA_GAME) // point n click
		updatePointClickCam();

	// Get the current camera height
	if (camera==CAMERA_EDIT) // edit cam
		cameraHeight=anm->getDistance();
	else
		{
		// Get the distance and set it on the ingame camera (point & click)
			f32 distance=Player::getInstance()->getNode()->getPosition().getDistanceFrom(gameCam->getPosition());
			cameraHeight-=(increments*(distance/10));
		}

	// Fix the limits of the cam (not going out of range)
	if (cameraHeight>max)
		cameraHeight=max;
	if (cameraHeight<min)
		cameraHeight=min;

	if (camera==CAMERA_EDIT)
	{
		// Get the distance and set it on the edit camera
		f32 distance = cameraHeight;
		distance=distance+(increments*(distance/10));
		
		if (distance<1.0f) 
		{
			distance=1.0f;
			GUIManager::getInstance()->setConsoleText(L"Reached limit of camera zoom!",video::SColor(255,180,0,0));
		}
		anm->setDistance(distance);
		//if (distance>144)
		//	editCamMaya->setFarValue(distance*4.0f);

	}
	else
	{
		if (camera==CAMERA_GAME)
		{
			
			updatePointClickCam();
		}
	}
}

// Return the distance between the subject and the camera
f32 CameraSystem::getCameraHeight()
{
	return cameraHeight;
}


// This method update the point&click camera
void CameraSystem::updatePointClickCam()
{
	if (camera==CAMERA_GAME)
	{
		// Get the player and find a "reference" position based on it.
		core::vector3df camrefpos = Player::getInstance()->getObject()->getPosition();
		camrefpos.Y+=cameraTargetHeight;

		// Find the distance between the current camera and the reference position
		//f32 camdistance = this->getPosition().getDistanceFrom(camrefpos);

		// Initialize the vector and set the distance of the camera toward it
		vector3df pos=vector3df(0,0,0);
		pos.X+=cameraHeight;

		// Offset from the reference position (the player)
		pos+=camrefpos;
	

		// This update the camera view when it's set as a RPG Camera
		// Parent is player, and use the player angle as reference
		
		//Disabled for the moment, but there is a good interpolation code in there.
		/*if (viewtype==VIEW_RPG)
		{
			f32 camrefangle=0;

			if (Player::getInstance()->getObject()->isWalking())
				camrefangle = -90+(Player::getInstance()->getObject()->getRotation()).Y;
			else
				camrefangle = cameraAngle.X;

			// Limit camera and oldrot to stay in 0-360 degree range
			if (camrefangle<0)
				camrefangle+=360;

			if (camrefangle>360)
				camrefangle-=360;

			if (oldrot<0)
				oldrot+=360;

			if (oldrot>360)
				oldrot-=360;
		
			f32 diff = camrefangle-oldrot;
	
			// Turn on the other side if the difference is too great
			if (diff>181)
			{
				camrefangle=180-camrefangle;
				diff = camrefangle+oldrot;
			}

			// Turn on the other side if the difference is too great
			if (diff<-181)
			{
				camrefangle=180+camrefangle;
				diff = camrefangle+oldrot;
			}

			// The "matching speed" of the rotation
			f32 matchspeed = 0.1f;
			bool correction=true;

			if (diff<matchspeed && diff>matchspeed)
				correction=false;


			if (diff>matchspeed && correction)
			{

				//camrefangle=oldrot+(matchspeed);
				if (diff>45)
					camrefangle=oldrot+(matchspeed*5);
				if (diff>20 && diff<46)
					camrefangle=oldrot+(matchspeed*2);
				if (diff<21)
					camrefangle=oldrot+(matchspeed);

			}
		
			if (diff<-matchspeed && correction)
			{
				//camrefangle=oldrot-(matchspeed);
				if (diff<-45)
					camrefangle=oldrot-(matchspeed*5);
				if (diff<-20 && diff>-46)
					camrefangle=oldrot-(matchspeed*2);
				if (diff>-21)
					camrefangle=oldrot-(matchspeed);
			}

			if (Player::getInstance()->getObject()->isWalking())
				cameraAngle.X=camrefangle;

			oldrot=camrefangle;

		}
		*/

		// Do the rotation calculation
		pos.rotateXYBy(cameraAngle.Y, camrefpos);
		pos.rotateXZBy(-cameraAngle.X, camrefpos);

	
		// Set the position and angle of the cam
		currentCam->setPosition(pos);
		currentCam->setTarget(camrefpos);
	}
	
}

void CameraSystem::findCamAngle() 
{
	vector2d<f32> pom = vector2d<f32>(0,0);
	
	if (initrotation) // && timer-timer4>17)
	{
		// Offset from the stored value
		vector2d<f32> pombase=device->getCursorControl()->getRelativePosition();
		vector2d<f32> pom1 = oldmouse-pombase;
		pom.X=initangle.X-(pom1.X*360);
		pom.Y=initangle.Y-(pom1.Y*360);

		
		if ((pombase.X<0.01f) || (pombase.X>0.99f))  //make the mouse cursor "loop" to prevent locking
		{
			initangle.X=getAngle().X;
			initangle.Y=getAngle().Y;
			if (pombase.X>0.99f)
			{
				device->getCursorControl()->setPosition(vector2df(0.01f,pombase.Y));
				oldmouse=vector2df(0.01f,pombase.Y);
				device->sleep(2);
			} else if (pombase.X<0.01f)
			{
				device->getCursorControl()->setPosition(vector2df(0.99f,pombase.Y));
				oldmouse=vector2df(0.99f,pombase.Y);
				device->sleep(2);
			} 

			pom1 = oldmouse-device->getCursorControl()->getRelativePosition();
			pom.X=initangle.X-(pom1.X*360);
			pom.Y=initangle.Y-(pom1.Y*360);
			
		}

		if((pombase.Y<0.01f) || (pombase.Y>0.95f))
		{
			initangle.X=getAngle().X;
			initangle.Y=getAngle().Y;
			if (pombase.Y<0.01f)
			{
				device->getCursorControl()->setPosition(vector2df(pombase.X,0.95f));
				device->getCursorControl()->setPosition(vector2df(pombase.X,0.95f));
				oldmouse=vector2df(pombase.X,0.95f);
				device->sleep(2);
				
			} else if (pombase.Y>0.95f)
			{
				device->getCursorControl()->setPosition(vector2df(pombase.X,0.01f));
				oldmouse=vector2df(pombase.X,0.01f);
				device->sleep(2);
			}
			pom1 = oldmouse-device->getCursorControl()->getRelativePosition();
			pom.X=initangle.X-(pom1.X*360);
			pom.Y=initangle.Y-(pom1.Y*360);
			

		}

		if (viewtype==VIEW_RTS || viewtype==VIEW_RTS_FIXED)
				setPointNClickAngle(pom);

		if (viewtype==VIEW_RPG)
		{
			//if (Player::getInstance()->getObject()->isWalking())
			//	setPointNClickAngle(vector2df(initangle.X,pom.Y));
			//else
				setPointNClickAngle(pom);

		}

	}		
}

//This will rotate the player depending of the view
void CameraSystem::updatePlayerRotation()
{
	vector2d<f32> pom = vector2d<f32>(0,0);
	vector3df oldangle = Player::getInstance()->getNode()->getRotation();
	
	// Offset from the stored value
	vector2d<f32> pom1 = oldmouse-device->getCursorControl()->getRelativePosition();
	pom.X=oldangle.X-(pom1.X*360);
	pom.Y=oldangle.Y-(pom1.Y*360);
	if (Player::getInstance()->getObject()->isWalking())
	{
			Player::getInstance()->getNode()->setRotation(vector3df(0,pom.X,0));
	}
	if (viewtype==VIEW_FPS)
		Player::getInstance()->getNode()->setRotation(vector3df(0,pom.X,0));


	vector2d<f32> pombase=device->getCursorControl()->getRelativePosition();
	if ((pombase.X<0.01f) || (pombase.X>0.99f))  //make the mouse cursor "loop" to prevent locking
	{
		initangle.X=getAngle().X;
		initangle.Y=getAngle().Y;

		if (pombase.X>0.99f)
		{
			device->getCursorControl()->setPosition(vector2df(0.01f,pombase.Y));
			oldmouse=vector2df(0.01f,pombase.Y);
			device->sleep(2);
		} else if (pombase.X<0.01f)
		{
			device->getCursorControl()->setPosition(vector2df(0.99f,pombase.Y));
			oldmouse=vector2df(0.99f,pombase.Y);
			device->sleep(2);
		} 

		pom1 = oldmouse-device->getCursorControl()->getRelativePosition();
		pom.X=initangle.X-(pom1.X*360);
			
	}


}

void CameraSystem::updateFPSCamera()
{
	if (camera==CAMERA_GAME)
	{

		vector3df rotation = Player::getInstance()->getNode()->getRotation();
		rotation.Z=0;
		vector3df pos = Player::getInstance()->getNode()->getPosition();
		vector3df offset = vector3df(0,58.0f,-4.0f);
		//vector3df offset = vector3df(0,59.0f,150.0f);

		offset.rotateXZBy(-rotation.Y);
		pos+=offset;	
		// Set the position and angle of the cam
		currentCam->setPosition(pos);
		
		vector2df pom = oldmouse-device->getCursorControl()->getRelativePosition();
		rotation.X-=(pom.Y*360.0f);

		//Limit the rotation view on the UP/DOWN axis
		if (rotation.X>60.0f)
			rotation.X=60.0f;
		if (rotation.X<-60.0f)
			rotation.X=-60.0f;

		rotation.Y+=180.0f; //have to flip the angle
		

		currentCam->setRotation(rotation);
		currentCam->updateAbsolutePosition();
		//Player::getInstance()->getNode()->setRotation(vector3df(0,rotation.Y,0));
		//device->getCursorControl()->setPosition(vector2df(0.5f,0.5f));
		
	}
}

void CameraSystem::updateRPGCamera()
{
	
	if (!initrotation)
	{
		initrotation=true;
		initangle.X=getAngle().X;
		initangle.Y=getAngle().Y;
		oldmouse = device->getCursorControl()->getRelativePosition();
	}


	// Get the player and find a "reference" position based on it.
	core::vector3df camrefpos = Player::getInstance()->getObject()->getPosition();
	camrefpos.Y+=cameraTargetHeight;

	// Find the distance between the current camera and the reference position
	//f32 camdistance = this->getPosition().getDistanceFrom(camrefpos);

	// Initialize the vector and set the distance of the camera toward it
	vector3df pos=vector3df(0,0,0);
	pos.X+=cameraHeight;

	// Offset from the reference position (the player)
	pos+=camrefpos;

	findCamAngle();

	// Do the rotation calculation
	pos.rotateXYBy(cameraAngle.Y, camrefpos);		
	pos.rotateXZBy(-cameraAngle.X, camrefpos);

	// Set the position and angle of the cam
	currentCam->setPosition(pos);
	currentCam->setTarget(camrefpos);
}

//! Define the control types in the player class
void CameraSystem::updateTypes()
{
	switch (viewtype)
	{
	case VIEW_RTS:
		Player::getInstance()->controltype=Player::CONTROL_POINTNCLICK;
		break;
	case VIEW_RTS_FIXED:
		Player::getInstance()->controltype=Player::CONTROL_POINTNCLICK;
		break;
	case VIEW_FPS:
		Player::getInstance()->controltype=Player::CONTROL_WASD;
		oldmouse=device->getCursorControl()->getRelativePosition();
		break;
	case VIEW_RPG:
		Player::getInstance()->controltype=Player::CONTROL_WASD;
		oldmouse=device->getCursorControl()->getRelativePosition();
		break;
	}
}

//! Will update the angle of the pointNClick camera by mouse offsets
void CameraSystem::setPointNClickAngle(vector2df angle)
{
	//Old behavior was adding angle to the current angle.
	//Has some complaints from forum user about imprecision.

	//angle*=cameraRotationSpeed;
    //cameraAngle.X+=-(angle.X/2);
	//cameraAngle.Y+=-(angle.Y/2);
	if (viewtype==VIEW_RTS_FIXED)
	{
		return;
	}

	//New behavior take the angle directly
	cameraAngle.X = angle.X;
	cameraAngle.Y = angle.Y;
	
	// Limit the view
	if (cameraAngle.Y>cameraAngleLimit.Y) //89 High angle (Y)
		cameraAngle.Y=cameraAngleLimit.Y;
	if (cameraAngle.Y<cameraAngleLimit.X) //-25.0 low angle (X)
		cameraAngle.Y=cameraAngleLimit.X;
}

// Return the current camera node pointer
ICameraSceneNode* CameraSystem::getNode()
{
	return currentCam;
}


void CameraSystem::fixRatio(IVideoDriver * driver)
{
	dimension2d<u32> screensize = driver->getScreenSize();
	currentCam->setAspectRatio((irr::f32)screensize.Width/screensize.Height);
}

//! Move the current camera
void CameraSystem::moveCamera(vector3df pos)
{
    currentCam->setPosition(currentCam->getPosition() + pos);
	currentCam->setTarget(currentCam->getTarget() + pos);
}

void CameraSystem::setPosition(vector3df pos)
{
	currentCam->setPosition(pos);
}

vector3df CameraSystem::getPosition()
{
    return currentCam->getPosition();
}

core::vector3df CameraSystem::getTarget()
{
	return currentCam->getTarget();
}


