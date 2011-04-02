#ifndef NODEPREVIEW_H
#define NODEPREVIEW_H

#include <irrlicht.h>
#include "IGUIElement.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

namespace irr
{
namespace gui
{

class NodePreview : public IGUIElement
{
    public:
		NodePreview(IGUIEnvironment* environment, IGUIElement* parent, const core::rect<s32>& rectangle, s32 id);
        //NodePreview(rect<s32> viewPort, ISceneNode* node = 0);

        void setNode(ISceneNode* node);
        ISceneNode* getNode();

		void drawBackground(bool draw);
		void drawBorder(bool draw);
		void setBackgroundColor(video::SColor color);

		void setViewport(core::rect<s32> viewPort);
		core::rect<s32> getViewPort();

        virtual void draw();

        virtual ~NodePreview();
    private:
		core::rect<s32> rect;
		video::IVideoDriver * driver;

		bool background;
		bool border;
		video::SColor backgroundcolor;

        ISceneNode* node;
		core::rect<s32> viewPort;
		f32 rotation;
};
}
}
#endif // NODEPREVIEW_H
