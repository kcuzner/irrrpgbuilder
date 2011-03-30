#ifndef WX_IRRLICHT
#define WX_IRRLICHT

#include <wx/wx.h>
#include <irrlicht.h>

enum wxIrrOrientation
{
	WXIRR_DIRNONE,
	WXIRR_DIRHORIZ,
	WXIRR_DIRVERT,
	WXIRR_DIRWEST,
	WXIRR_DIREAST,
	WXIRR_DIRNORTH,
	WXIRR_DIRSOUTH,
};

typedef irr::IEventReceiver					wxIEventReceiver;
typedef irr::SEvent							wxIEvent;
typedef irr::SIrrlichtCreationParameters	wxICreateParam;
typedef irr::IrrlichtDevice					wxIDevice;
typedef irr::s32							wxIS32;

typedef irr::video::E_DRIVER_TYPE			wxIDriverType;
typedef irr::video::IVideoDriver			wxIVideoDriver;
typedef irr::video::SColor					wxIColor;
typedef irr::video::E_MATERIAL_FLAG			wxIMaterialFlag;

typedef irr::scene::ISceneManager			wxISceneManager;
typedef irr::scene::ICameraSceneNode		wxICamera;
typedef irr::scene::ISceneNode				wxINode;

typedef irr::core::position2d<irr::s32>		wxIPosition2DInteger;
typedef irr::core::dimension2d<irr::u32>	wxIDimension2DInteger;
typedef irr::core::dimension2d<irr::f32>	wxIDimension2DFloat;
typedef irr::core::vector3df				wxIVector3DFloat;
typedef irr::core::rect<irr::s32>			wxIRectInteger;
typedef irr::core::stringw					wxIWString;

typedef irr::gui::IGUIFont					wxIGuiFont;

#endif
