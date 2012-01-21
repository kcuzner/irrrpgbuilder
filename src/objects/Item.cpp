#include "Item.h"

Item::Item(stringc name, stringc script)
{
    this->name = name;
    this->script = script;
}

Item::~Item()
{
    //dtor
}

Item* Item::clone()
{
    Item* newItem = new Item(name,script);
	return newItem;
}

void Item::setScript(stringc script)
{
    this->script = script;
}

stringc Item::getScript()
{
    return script;
}
