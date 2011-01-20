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
	dotvictim.clear();
	dotduration.clear();
	dotdamage.clear();
	dotenabled=false;
}

Combat::~Combat()
{

}

Combat* Combat::getInstance()
{
    static Combat* instance = 0;
    if(!instance) instance = new Combat();
    return instance;
}

void Combat::attack(DynamicObject* attacker, DynamicObject* defender)
{
	
	property attacker_prop = attacker->getProperties();
	property attacker_base = attacker->getProp_base();
	property attacker_level = attacker->getProp_level();
	
	int life=defender->getLife();
	//attacker->setProp_base(attacker_base);
	//attacker->setProp_level(attacker_level);
	
	dumpProperties(attacker);
	dumpProperties(defender);
	// Basic rules that evaluate only the damage (and evaluation per level)
	// TODO: Need to be changed. The "current" properties is the only one to be evaluated.
	// The 2 others are needed only at level change to set the "current" properties.
	// Documentation will have to be written so users could those adequately.
	int maxdamage = attacker_base.maxdamage+(attacker_level.maxdamage*attacker->getProperties().level);
	int mindamage = attacker_base.mindamage+(attacker_level.mindamage*attacker->getProperties().level);
	if (maxdamage<1)
		maxdamage=1;
	if (mindamage<0) 
		mindamage=0;
	int damage = chances(mindamage,maxdamage);
	life -= damage;
	
	if (life<0) 
			life=0;
	
	defender->setLife(life);
	DynamicObjectsManager::getInstance()->getTarget()->setPosition(defender->getPosition()+vector3df(0,0.1f,0));
	printf("Combat was initiated between: %s and %s!!!\n",attacker->getName().c_str(),defender->getName().c_str());
	printf("%s lost %i point of life!!\n",defender->getName().c_str(),damage);
	if (life==0)
		{
			// get the experience of the defender
			attacker_prop.experience += defender->getProperties().experience;
			// Set the properties of the attacker back
			attacker->setProperties(attacker_prop);
			defender=NULL;
			attacker->setAnimation("idle");
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
		}
	
	Player::getInstance()->updateDisplay();
}

int Combat::chances(int min, int max)
// Use a random value to determinate the result
// Example damage done ranging from 10 to 100 points min=10, max=100
{
	return rand() % (max-min) + min;
}

void Combat::dumpProperties(DynamicObject* object)
{
	printf ("\nHere are the stats for NPC: %s\n\n",object->getName().c_str());
	printf ("----------------------------------------------------------\n");
	printf ("PROPERTIE NAME            |  Current |  Base   |  Level   |\n");
	printf ("----------------------------------------------------------\n");
	printf ("Life (Hit points/hp)      |  %i      |   %i    |   %i     |\n",object->getProperties().life,object->getProp_base().life,object->getProp_level().life);
	printf ("Experience                |  %i      |   %i    |   %i     |\n",object->getProperties().experience,object->getProp_base().experience,object->getProp_level().experience);
	printf ("Money (Gold)              |  %i      |   %i    |   %i     |\n",object->getProperties().money,object->getProp_base().money,object->getProp_level().money);
	printf ("Level                     |  %i      |   %i    |   %i     |\n",object->getProperties().level,object->getProp_base().level,object->getProp_level().level);
	printf ("Minimum damage            |  %i      |   %i    |   %i     |\n",object->getProperties().mindamage,object->getProp_base().mindamage,object->getProp_level().mindamage);
	printf ("Maximum damage            |  %i      |   %i    |   %i     |\n",object->getProperties().maxdamage,object->getProp_base().maxdamage,object->getProp_level().maxdamage);
	printf ("Hurt resistance (%%)       |  %i      |   %i    |   %i     |\n",object->getProperties().hurt_resist,object->getProp_base().hurt_resist,object->getProp_level().hurt_resist);
	printf ("Armor                     |  %i      |   %i    |   %i     |\n",object->getProperties().armor,object->getProp_base().armor,object->getProp_level().armor);
	printf ("Magic Armor               |  %i      |   %i    |   %i     |\n",object->getProperties().magic_armor,object->getProp_base().magic_armor,object->getProp_level().magic_armor);
	printf ("Regen life points per tick|  %i      |   %i    |   %i     |\n",object->getProperties().regenlife,object->getProp_base().regenlife,object->getProp_level().regenlife);
	printf ("Regen mana points per tick|  %i      |   %i    |   %i     |\n",object->getProperties().regenmana,object->getProp_base().regenmana,object->getProp_level().regenmana);
	printf ("Dot(poison) duration      |  %i      |   %i    |   %i     |\n",object->getProperties().dotduration,object->getProp_base().dotduration,object->getProp_level().dotduration);
	printf ("---------------------------------------------------------\n\n");
}

void Combat::dot(DynamicObject *victim, int duration, int damage)
{
	// Assign DOT damage to a victim (could be player or any NPC)
	u32 timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	dotvictim.push_back(victim);
	dotduration.push_back(duration);
	dotdamage.push_back(damage);
	dottimer.push_back(timer);
	dotenabled = true;
	
}

void Combat::update()
{
	u32 timer = App::getInstance()->getDevice()->getTimer()->getRealTime();
	// Updating function mostly for dealing with DOT damage (DOT is Damage Over Time, like poison)
	bool dotenabled=false;
	if (dotenabled) // Dot is enabled, check for victims and do the damage.
	{
		for(int i=0;i<(int)dotvictim.size();i++)
		{
			if (dottimer[i]-timer<(u32)dotduration[i])
			{
				// Do the damage from the tick
				int hp=dotvictim[i]->getLife();
				((DynamicObject*)dotvictim[i])->setLife(hp-dotdamage[i]);
			} else
			{
				// Timer is elapsed and the victim does not have DOT anymore
				dotvictim.erase(dotvictim.begin()+i);
				dotduration.erase(dotduration.begin()+i);
				dotdamage.erase(dotdamage.begin()+i);
				dottimer.erase(dottimer.begin()+1);
			}
		}
	}
	
}