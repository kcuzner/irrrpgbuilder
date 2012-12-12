#include "../App.h"
#include "DynamicObjectsManager.h"

#include "TemplateObject.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


TemplateObject::TemplateObject(irr::core::stringw name)
{
	// This is done when an dynamic object is initialised (template && player)
    ISceneManager* smgr = App::getInstance()->getDevice()->getSceneManager();

	stringc realFile = "../media/dynamic_objects/";
    realFile += meshFile;

	templateName = name;

}


TemplateObject::~TemplateObject()
{
  
	
}

void TemplateObject::setName(stringw name)
{
	templateName=name;
}

core::stringw TemplateObject::getName()
{
	if (templateName!=NULL)
		return templateName;
	else return L"Error!!!";
}

void TemplateObject::setType(stringc name)
{
	if (name=="npc")
		this->objectType=OBJECT_TYPE_NPC;
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

void TemplateObject::setType(TYPE type)
{
	objectType=type;
}

TYPE TemplateObject::getType()
{
	return objectType;
}

cproperty TemplateObject::getProperties()
{
	return this->properties;
}

void TemplateObject::setProperties(cproperty prop)
{
	properties = prop;
}


cproperty TemplateObject::getProp_base()
{
	return this->prop_base;
}

void TemplateObject::setProp_base(cproperty prop)
{
	prop_base=prop;
}

void TemplateObject::setMaterialType(E_MATERIAL_TYPE mType)
{
    materialType=mType;
}

E_MATERIAL_TYPE TemplateObject::getMaterialType()
{
    return materialType;
}

void TemplateObject::setScale(f32 scale)
{
    templateScale = scale;
}

f32 TemplateObject::getScale()
{
    return templateScale;
}
