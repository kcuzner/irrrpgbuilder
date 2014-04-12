// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __TEST_ANDRES__
#define __TEST_ANDRES__

#include "CSceneNodeAnimatorCameraMayaIRB.h"
#include "ICursorControl.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"
#include "ISceneManager.h"
#include "../App.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorCameraMayaIRB::CSceneNodeAnimatorCameraMayaIRB(gui::ICursorControl* cursor,
	f32 rotateSpeed, f32 zoomSpeed, f32 translateSpeed, f32 distance, SKeyMap2* keyMapArray, u32 keyMapSize)
	: CursorControl(cursor), OldCamera(0), MousePos(0.5f, 0.5f),
	ZoomSpeed(zoomSpeed), RotateSpeed(rotateSpeed), TranslateSpeed(translateSpeed),
	CurrentZoom(distance), RotX(0.0f), RotY(0.0f),
	Zooming(false), Rotating(false), Moving(false), Translating(false)
{

	camera = NULL;
	bypass_switch=false;
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorCameraMayaIRB");
	#endif

	if (CursorControl)
	{
		CursorControl->grab();
		MousePos = CursorControl->getRelativePosition();
	}

	allKeysUp();
	FirstUpdate=true;

	// create key map
    if (!keyMapArray || !keyMapSize)
    {
		// create default key map
		KeyMap.push_back(SKeyMap2(EK_ROTATE, irr::KEY_LBUTTON));
		KeyMap.push_back(SKeyMap2(EK_TRANSLATE, irr::KEY_RBUTTON));
		KeyMap.push_back(SKeyMap2(EK_ZOOM, irr::KEY_MBUTTON));
		KeyMap.push_back(SKeyMap2(EK_STRAFE_LEFT, irr::KEY_LEFT));
		KeyMap.push_back(SKeyMap2(EK_STRAFE_RIGHT, irr::KEY_RIGHT));
		KeyMap.push_back(SKeyMap2(EK_MOVE_FORWARD, irr::KEY_UP));
		KeyMap.push_back(SKeyMap2(EK_MOVE_BACKWARD, irr::KEY_DOWN));
    }
    else
    {
        // create custom key map
        setKeyMap(keyMapArray, keyMapSize);
    }
}


//! destructor
CSceneNodeAnimatorCameraMayaIRB::~CSceneNodeAnimatorCameraMayaIRB()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CSceneNodeAnimatorCameraMayaIRB::OnEvent(const SEvent& evt)
{
	switch(evt.EventType)
	{
		case EET_KEY_INPUT_EVENT:
			for (u32 i=0; i<KeyMap.size(); ++i)
			{
				if (KeyMap[i].KeyCode == evt.KeyInput.Key)
				{
					printf("Received a keyboard event here!: Action= %d",(int)i);
					App::getInstance()->getDevice()->getGUIEnvironment()->setFocus(App::getInstance()->getDevice()->getGUIEnvironment()->getRootGUIElement()); // DEBUG: Reset the FOCUS when theses keys are pressed
					
					ActionKeys[KeyMap[i].Action] = evt.KeyInput.PressedDown;
					if (evt.KeyInput.PressedDown)
						printf(" pressed down!\n");
					else
						printf(" released!\n");
				} 
					
			}
		break;

		case EET_MOUSE_INPUT_EVENT:
			for (u32 i=0; i<KeyMap.size(); ++i)
			{
				if (KeyMap[i].KeyCode == irr::KEY_LBUTTON && App::getInstance()->getAppState()==APP_EDIT_VIEWDRAG)
				{
					ActionKeys[KeyMap[i].Action] = evt.MouseInput.isLeftPressed();
				}

				if (KeyMap[i].KeyCode == irr::KEY_RBUTTON && App::getInstance()->getAppState()==APP_EDIT_VIEWDRAG)
				{
					ActionKeys[KeyMap[i].Action] = evt.MouseInput.isRightPressed();
				}

				if (KeyMap[i].KeyCode == irr::KEY_MBUTTON && App::getInstance()->getAppState()==APP_EDIT_VIEWDRAG)
				{
					ActionKeys[KeyMap[i].Action] = evt.MouseInput.isMiddlePressed();
				}	

			}

			if (evt.MouseInput.Event == EMIE_MOUSE_MOVED)
			{
				if (App::getInstance()->getAppState()==APP_EDIT_VIEWDRAG)
					MousePos = CursorControl->getRelativePosition();
				return true;
			}
		break;

	default:
		break;
	}

	return false;
}


//! OnAnimate() is called just before rendering the whole scene.
void CSceneNodeAnimatorCameraMayaIRB::animateNode(ISceneNode *node, u32 timeMs)
{
	//Alt + LM = Rotate around camera pivot
	//Alt + LM + MM = Dolly forth/back in view direction (speed % distance camera pivot - max distance to pivot)
	//Alt + MM = Move on camera plane (Screen center is about the mouse pointer, depending on move speed)

	if (!node || node->getType() != ESNT_CAMERA)
		return;

	camera = static_cast<ICameraSceneNode*>(node);


	// If the camera isn't the active camera, and receiving input, then don't process it.
	if (!camera->isInputReceiverEnabled() && !bypass_switch)
	{
		// If the input receiver is down, then reset the camera, so it take the new position to start
		FirstUpdate=true;
		//printf("keycode?\n");
		return;
	}
	//reset the bypass switch
	bypass_switch=false;

	scene::ISceneManager * smgr = camera->getSceneManager();
	if (smgr && smgr->getActiveCamera() != camera)
	{
		//printf("Camera is inactive!\n");
		return;
	}

	if (OldCamera != camera)
	{
		LastCameraTarget = OldTarget = camera->getTarget();
		OldCamera = camera;
	}
	else
	{
		OldTarget += camera->getTarget() - LastCameraTarget;
	}

	// Calculate the camera angle and position for the first time
	if (FirstUpdate)
	{
		core::vector3df OffsetVector = camera->getTarget()-camera->getPosition();
		core::vector3df OffsetVec2 = core::vector3df(OffsetVector.Z, OffsetVector.Y, OffsetVector.X);
		core::vector3df InitialRotation = (-OffsetVec2).getHorizontalAngle();
		RotX = -InitialRotation.Y;
		RotY = -InitialRotation.X;
		CurrentZoom = camera->getPosition().getDistanceFrom(camera->getTarget());
		MousePos==core::position2df(0.5f,0.5f);
	}

	f32 nRotX = RotX;
	f32 nRotY = RotY;
	f32 nZoom = CurrentZoom;

	if (ActionKeys[EK_ZOOM])
	{
		if (!Zooming)
		{
			ZoomStart = MousePos;
			Zooming = true;
		}
		else
		{
			const f32 targetMinDistance = 0.01f;
			nZoom += (ZoomStart.X - MousePos.X) * ZoomSpeed;

			if (nZoom < targetMinDistance) // jox: fixed bug: bounce back when zooming to close
				nZoom = targetMinDistance;
		}
	}
	else if (Zooming)
	{
		const f32 old = CurrentZoom;
		CurrentZoom = CurrentZoom + (ZoomStart.X - MousePos.X ) * ZoomSpeed;
		nZoom = CurrentZoom;

		if (nZoom < 0)
			nZoom = CurrentZoom = old;
		Zooming = false;
	}

	// Translation ---------------------------------

		// New (aug 2013) define the speed based on the camera distance to the target
	TranslateSpeed = camera->getTarget().getDistanceFrom(camera->getPosition());

	core::vector3df translate(OldTarget);
	const core::vector3df upVector(camera->getUpVector());
	const core::vector3df target = camera->getTarget();

	core::vector3df pos = camera->getPosition();
	core::vector3df tvectX = pos - target;
	tvectX = tvectX.crossProduct(upVector);
	tvectX.normalize();

	const SViewFrustum* const va = camera->getViewFrustum();
	core::vector3df tvectY = (va->getFarLeftDown() - va->getFarRightDown());
	tvectY = tvectY.crossProduct(upVector.Y > 0 ? pos - target : target - pos);
	tvectY.normalize();


	if (ActionKeys[EK_STRAFE_LEFT] && !Zooming)
	{
		translate -=  tvectX;
		OldTarget = translate;		
	}
	if (ActionKeys[EK_STRAFE_RIGHT] && !Zooming)
	{
		translate +=  tvectX;
		OldTarget = translate;		
	}
	if (ActionKeys[EK_MOVE_FORWARD] && !Zooming)
	{ 
		translate += tvectX.crossProduct(core::vector3df(0,1,0));
		OldTarget = translate;		
	}
	if (ActionKeys[EK_MOVE_BACKWARD] && !Zooming)
	{
		translate -= tvectX.crossProduct(core::vector3df(0,1,0));
		OldTarget = translate;		
	}
		

	if (ActionKeys[EK_TRANSLATE] && !Zooming)
	{
		if (!Translating)
		{
			TranslateStart = MousePos;
			Translating = true;
		}
		else
		{
			//printf("DEBUG: Translating the node position:\n");
			translate +=  tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed +
			              tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed;
		}
	}
	else if (Translating)
	{
		//printf("DEBUG: Translating --- Stop translating:\n");
		translate += tvectX * (TranslateStart.X - MousePos.X)*TranslateSpeed +
		             tvectY * (TranslateStart.Y - MousePos.Y)*TranslateSpeed;
		OldTarget = translate;
		Translating = false;
	}

	// Rotation ------------------------------------

	if (ActionKeys[EK_ROTATE] && !Zooming)
	{
		if (!Rotating)
		{
			RotateStart = MousePos;
			Rotating = true;
			nRotX = RotX;
			nRotY = RotY;
		}
		else
		{
			nRotX += (RotateStart.X - MousePos.X) * RotateSpeed;
			nRotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		}
	}
	else if (Rotating)
	{
		RotX += (RotateStart.X - MousePos.X) * RotateSpeed;
		RotY += (RotateStart.Y - MousePos.Y) * RotateSpeed;
		nRotX = RotX;
		nRotY = RotY;
		Rotating = false;
	}

	// Set pos ------------------------------------

	pos = translate;
	pos.X += nZoom;

	pos.rotateXYBy(nRotY, translate);
	pos.rotateXZBy(-nRotX, translate);

	camera->setPosition(pos);
	camera->setTarget(translate);

	// Rotation Error ----------------------------

	// jox: fixed bug: jitter when rotating to the top and bottom of y
	pos.set(0,1,0);
	pos.rotateXYBy(-nRotY);
	pos.rotateXZBy(-nRotX+180.f);
	camera->setUpVector(pos);
	LastCameraTarget = camera->getTarget();
	FirstUpdate=false;
}

void CSceneNodeAnimatorCameraMayaIRB::allKeysUp()
{
	for (u32 i=0; i<EK_COUNT; ++i)
		ActionKeys[i] = false;
}


//! Sets the rotation speed
void CSceneNodeAnimatorCameraMayaIRB::setRotateSpeed(f32 speed)
{
	RotateSpeed = speed;
}


//! Sets the movement speed
void CSceneNodeAnimatorCameraMayaIRB::setMoveSpeed(f32 speed)
{
	TranslateSpeed = speed;
}


//! Sets the zoom speed
void CSceneNodeAnimatorCameraMayaIRB::setZoomSpeed(f32 speed)
{
	ZoomSpeed = speed;
}


//! Set the distance
void CSceneNodeAnimatorCameraMayaIRB::setDistance(f32 distance)
{
	CurrentZoom=distance;
	bypass_switch=true;
}


//! Gets the rotation speed
f32 CSceneNodeAnimatorCameraMayaIRB::getRotateSpeed() const
{
	return RotateSpeed;
}


// Gets the movement speed
f32 CSceneNodeAnimatorCameraMayaIRB::getMoveSpeed() const
{
	return TranslateSpeed;
}


//! Gets the zoom speed
f32 CSceneNodeAnimatorCameraMayaIRB::getZoomSpeed() const
{
	return ZoomSpeed;
}

//! Sets the keyboard mapping for this animator
void CSceneNodeAnimatorCameraMayaIRB::setKeyMap(SKeyMap2 *map, u32 count)
{
	// clear the keymap
	KeyMap.clear();

	// add actions
	for (u32 i=0; i<count; ++i)
	{
		KeyMap.push_back(map[i]);
	}
}

void CSceneNodeAnimatorCameraMayaIRB::setKeyMap(const core::array<SKeyMap2>& keymap)
{
	KeyMap=keymap;
}

const core::array<SKeyMap2>& CSceneNodeAnimatorCameraMayaIRB::getKeyMap() const
{
	return KeyMap;
}

//! Returns the current distance, i.e. orbit radius
f32 CSceneNodeAnimatorCameraMayaIRB::getDistance() const
{
	return CurrentZoom;
}


ISceneNodeAnimator* CSceneNodeAnimatorCameraMayaIRB::createClone(ISceneNode* node, ISceneManager* newManager)
{
	CSceneNodeAnimatorCameraMayaIRB * newAnimator =
		new CSceneNodeAnimatorCameraMayaIRB(CursorControl, RotateSpeed, ZoomSpeed, TranslateSpeed);
	return newAnimator;
}

} // end namespace
} // end namespace

#endif
