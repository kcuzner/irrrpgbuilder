#ifndef NODEPREVIEW_H
#define NODEPREVIEW_H

#include <irrlicht.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class NodePreview
{
    public:
        NodePreview(rect<s32> viewPort, ISceneNode* node = 0);

        void setNode(ISceneNode* node);
        ISceneNode* getNode();

        void setViewport(rect<s32> viewPort);
        rect<s32> getViewPort();

        void draw(IVideoDriver* driver);

        ~NodePreview();
    private:
        ISceneNode* node;
        rect<s32> viewPort;
		f32 rotation;
};

#endif // NODEPREVIEW_H
