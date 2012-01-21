// Copyright (C) 2002-2011 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// Patched by Christian Clavet (Jan 2012) to support more features

#ifndef __C_SCENE_NODE_ANIMATOR_CAMERA_MAYA_H_INCLUDED__
#define __C_SCENE_NODE_ANIMATOR_CAMERA_MAYA_H_INCLUDED__

#include "ISceneNodeAnimatorCameraMaya.h"
#include "ICameraSceneNode.h"
#include "vector2d.h"
#include "Keycodes.h"

namespace irr
{

	//! enumeration for key actions. Used for example in the FPS Camera.
	enum EKEY_ACTION2
	{
		EK_MOVE_FORWARD = 0,
		EK_MOVE_BACKWARD,
		EK_STRAFE_LEFT,
		EK_STRAFE_RIGHT,
		EK_JUMP_UP,
		EK_CROUCH,
		EK_ROTATE,
		EK_TRANSLATE,
		EK_ZOOM,
		EK_COUNT,

		//! This value is not used. It only forces this enumeration to compile in 32 bit.
		EK_FORCE_32BIT = 0x7fffffff
	};

	//! Struct storing which keyboard key belongs to which action.
	struct SKeyMap2
	{
		SKeyMap2() {}
		SKeyMap2(EKEY_ACTION2 action, EKEY_CODE keyCode) : Action(action), KeyCode(keyCode) {}

		EKEY_ACTION2 Action;
		EKEY_CODE KeyCode;
	};




namespace gui
{
	class ICursorControl;
}

namespace scene
{

	//! Special scene node animator for FPS cameras
	/** This scene node animator can be attached to a camera to make it act
	like a 3d modelling tool camera
	*/
	class CSceneNodeAnimatorCameraMaya : public ISceneNodeAnimatorCameraMaya 
	{
	public:
		//! Constructor
		CSceneNodeAnimatorCameraMaya(gui::ICursorControl* cursor, f32 rotateSpeed = -1500.f, 
			f32 zoomSpeed = 200.f, f32 translationSpeed = 1500.f, f32 distance=70.f, 
			SKeyMap2* keyMapArray=0, u32 keyMapSize=0);

		//! Destructor
		virtual ~CSceneNodeAnimatorCameraMaya();

		//! Animates the scene node, currently only works on cameras
		virtual void animateNode(ISceneNode* node, u32 timeMs);

		//! Event receiver
		virtual bool OnEvent(const SEvent& evt);

		//! Returns the speed of movement in units per millisecond
		virtual f32 getMoveSpeed() const;

		//! Sets the speed of movement in units per millisecond
		virtual void setMoveSpeed(f32 moveSpeed);

		//! Returns the rotation speed
		virtual f32 getRotateSpeed() const;

		//! Set the rotation speed
		virtual void setRotateSpeed(f32 rotateSpeed);

		//! Returns the zoom speed
		virtual f32 getZoomSpeed() const;

		//! Set the zoom speed
		virtual void setZoomSpeed(f32 zoomSpeed);

		//! Sets the keyboard mapping for this animator (old style)
		//! \param keymap: an array of keyboard mappings, see SKeyMap
		//! \param count: the size of the keyboard map array
		virtual void setKeyMap(SKeyMap2 *map, u32 count);

		//! Sets the keyboard mapping for this animator
		//!	\param keymap The new keymap array 
		virtual void setKeyMap(const core::array<SKeyMap2>& Keymap);

		//! Gets the keyboard mapping for this animator
		virtual const core::array<SKeyMap2>& getKeyMap() const;

		//! Returns the current distance, i.e. orbit radius
		virtual f32 getDistance() const;

		//! Set the distance
		virtual void setDistance(f32 distance);

		//! This animator will receive events when attached to the active camera
		virtual bool isEventReceiverEnabled() const
		{
			return true;
		}

		//! Returns type of the scene node
		virtual ESCENE_NODE_ANIMATOR_TYPE getType() const 
		{
			return ESNAT_CAMERA_MAYA;
		}

		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling
		this. */
		virtual ISceneNodeAnimator* createClone(ISceneNode* node, ISceneManager* newManager=0);

	private:

		void allKeysUp();
		void animate();
		
		gui::ICursorControl *CursorControl;
		scene::ICameraSceneNode* OldCamera;
		core::vector3df OldTarget;
		core::vector3df LastCameraTarget;	// to find out if the camera target was moved outside this animator
		core::position2df RotateStart;
		core::position2df ZoomStart;
		core::position2df TranslateStart;
		core::position2df MousePos;
		f32 ZoomSpeed;
		f32 RotateSpeed;
		f32 TranslateSpeed;
		f32 CurrentZoom;
		f32 RotX, RotY;
		bool Zooming;
		bool Rotating;
		bool Moving;
		bool Translating;
		bool FirstUpdate;

		core::array<SKeyMap2> KeyMap;
		bool ActionKeys[EK_COUNT];


	};

} // end namespace scene
} // end namespace irr

#endif

