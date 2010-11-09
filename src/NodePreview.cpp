#include "NodePreview.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

NodePreview::NodePreview(rect<s32> viewPort, ISceneNode* node)
{
    this->node = node;
    this->viewPort = viewPort;
}

NodePreview::~NodePreview()
{
}

void NodePreview::draw(IVideoDriver* driver)
{
    if(!node) return;

    rect<s32> originalViewport = driver->getViewPort();

    driver->setViewPort(viewPort);

    matrix4 oldProjMat = driver->getTransform(video::ETS_PROJECTION);
    matrix4 oldViewMat = driver->getTransform(video::ETS_VIEW);
    matrix4 oldWorldMat = driver->getTransform(video::ETS_WORLD);

    matrix4 mat = core::IdentityMatrix;
    mat.setScale(1.8f * node->getScale().X);

    mat.setTranslation(vector3df(-node->getPosition().X * 1.8f,-node->getPosition().Y * 1.8f - 0.9f,-node->getPosition().Z * 1.8f));

    driver->setTransform ( video::ETS_PROJECTION, core::IdentityMatrix );
    driver->setTransform ( video::ETS_VIEW, mat );
    driver->setTransform ( video::ETS_WORLD, core::IdentityMatrix );

    node->render();

    driver->setTransform ( video::ETS_PROJECTION, oldProjMat );
    driver->setTransform ( video::ETS_VIEW, oldViewMat );
    driver->setTransform ( video::ETS_WORLD, oldWorldMat );

    driver->setViewPort(originalViewport);
}

void NodePreview::setNode(ISceneNode* node)
{
    this->node = node;
}

ISceneNode* NodePreview::getNode()
{
    return node;
}

void NodePreview::setViewport(rect<s32> viewPort)
{
    this->viewPort = viewPort;
}

rect<s32> NodePreview::getViewPort()
{
    return viewPort;
}
