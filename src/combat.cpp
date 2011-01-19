#include "combat.h"

#include "App.h"
#include "DynamicObject.h"
#include "DynamicObjectsManager.h"
#include "LuaGlobalCaller.h"
#include "Player.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;


Combat::Combat()
{
}

Combat::~Combat()
{
    
}

void Combat::attack(DynamicObject* attacker, DynamicObject* defender)
{
	int life=defender->getLife();
	property attacker_base = attacker->getProp_base();
	property attacker_level = attacker->getProp_level();
	
	//attacker->setProp_base(attacker_base);
	//attacker->setProp_level(attacker_level);
	
	dumpProperties(attacker);
	dumpProperties(defender);
	// Basic rules that evaluate only the damage (and evaluation per level)
	int maxdamage = attacker_base.maxdamage+(attacker_level.maxdamage*attacker->getProperties().level);
	int mindamage = attacker_base.mindamage+(attacker_level.mindamage*attacker->getProperties().level);
	if (maxdamage<1)
		maxdamage=1;
	if (mindamage<0) 
		mindamage=0;
	int damage = rand() % (maxdamage-mindamage) + mindamage;
	life -= damage;
	
	if (life<0) 
			life=0;
	
	defender->setLife(life);
	DynamicObjectsManager::getInstance()->getTarget()->setPosition(defender->getPosition()+vector3df(0,0.1f,0));
	printf("Combat was initiated between: %s and %s!!!\n",attacker->getName().c_str(),defender->getName().c_str());
	printf("%s lost %i point of life!!\n",defender->getName().c_str(),damage);
	if (life==0)
		{
			defender=NULL;
			attacker->setAnimation("idle");
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
		}
	
	
}

void Combat::dumpProperties(DynamicObject* object)
{
	printf ("\nHere are the stats for NPC: %s\n\n",object->getName().c_str());
	printf ("----------------------------------------------------------\n");
	printf ("PROPERTIE NAME            |  Current |  Base   |  Level   |\n");
	printf ("----------------------------------------------------------\n");
	printf ("Life                      |  %i      |   %i    |   %i     |\n",object->getProperties().life,object->getProp_base().life,object->getProp_level().life);
	printf ("Experience                |  %i      |   %i    |   %i     |\n",object->getProperties().experience,object->getProp_base().experience,object->getProp_level().experience);
	printf ("Money                     |  %i      |   %i    |   %i     |\n",object->getProperties().money,object->getProp_base().money,object->getProp_level().money);
	printf ("Level                     |  %i      |   %i    |   %i     |\n",object->getProperties().level,object->getProp_base().level,object->getProp_level().level);
	printf ("Minimum damage            |  %i      |   %i    |   %i     |\n",object->getProperties().mindamage,object->getProp_base().mindamage,object->getProp_level().mindamage);
	printf ("Maximum damage            |  %i      |   %i    |   %i     |\n",object->getProperties().maxdamage,object->getProp_base().maxdamage,object->getProp_level().maxdamage);
	printf ("Hurt resistance (%%)       |  %i      |   %i    |   %i     |\n",object->getProperties().hurt_resist,object->getProp_base().hurt_resist,object->getProp_level().hurt_resist);
	printf ("Armor                     |  %i      |   %i    |   %i     |\n",object->getProperties().armor,object->getProp_base().armor,object->getProp_level().armor);
	printf ("Magic Armor               |  %i      |   %i    |   %i     |\n",object->getProperties().magic_armor,object->getProp_base().magic_armor,object->getProp_level().magic_armor);
	
	printf ("---------------------------------------------------------\n\n");
}
