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
	freepoints = 0;
	skillspoints = 0 ;
	dotenabled=false;
	tempObject=NULL;
	dotvictim.clear();
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
	
	// Retrieve the current life meter on the defender
	int life=defender->getLife();
	printf ("----------------------------------------------------------------\n");
	printf ("  COMBAT - COMBAT - COMBAT - COMBAT - COMBAT - COMBAT - COMBAT\n");
	printf ("----------------------------------------------------------------\n");
	printf ("Attacker: %s and defender:%s!!!\n",attacker->getName().c_str(),defender->getName().c_str());
	printf ("----------------------------------------------------------------\n");
	dumpProperties(attacker);
	dumpProperties(defender);
	
	// TODO: Documentation will have to be written so users could the properties adequately.
	
	int damage = chances(attacker_prop.mindamage,attacker_prop.maxdamage);
	printf ("----------------------------------------------------------------\n");
	printf("%s should have %i point of damage!!\n",defender->getName().c_str(),damage);
	printf("%s have %i%% Hit probability\n",attacker->getName().c_str(),(int)attacker_prop.hit_prob);
	printf("%s have %i%% Dodge probability\n",defender->getName().c_str(),(int)defender->getProperties().dodge_prop);
	if (!percent(attacker_prop.hit_prob-defender->getProperties().dodge_prop))
	{
		damage = 0;
		defender->setObjectLabel("Miss!");
		printf("%s missed %s!\n",attacker->getName().c_str(),defender->getName().c_str());
	}
	life -= damage;

	// Basic "Hurt" state
	// So if the attacker got almost all his points the defender will be "hurt"
	// need to update the rules to use the "hurt resistance" propertie...
	if (((damage+2)>attacker->getProperties().maxdamage) && damage>2)
		defender->setAnimation("hurt");
	
	// limit the damage to the life of the defender.
	if (life<0) 
			life=0;
	
	defender->setLife(life);
	// Position the target to the defender (Apply only on the player
	if (attacker->getName()==Player::getInstance()->getObject()->getName())
		DynamicObjectsManager::getInstance()->getTarget()->setPosition(defender->getPosition()+vector3df(0,0.1f,0));
	printf ("----------------------------------------------------------------\n");
	if (life!=0)
		printf("%s lost %i point of life!!\n\n",defender->getName().c_str(),damage);
	if (life==0)
		{
			// get the experience of the defender
			attacker_prop.experience += defender->getProperties().experience;
			// Set the properties of the attacker back since the experience increased
			attacker->setProperties(attacker_prop);

			// Determine if a level raise is needed
			int baseXP = attacker->getProp_base().experience;
			int levelXP = attacker->getProp_level().experience;
			int level = attacker_prop.level;
			if (attacker_prop.experience>(baseXP+(levelXP*(level*level))))
				updateLevel(attacker);
			printf("%s was killed by %s!!\n\n",defender->getName().c_str(),attacker->getName().c_str());
			// Set the animation for the attacker when the defender is dead
			//defender=NULL;
			attacker->setAnimation("idle");
			attacker->clearEnemy();
			defender->setAnimation("die");
			DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
			
		}
	printf ("----------------------------------------------------------------\n");
	Player::getInstance()->updateDisplay();
}

void Combat::updateLevel(DynamicObject* object)
{
	property object_prop = object->getProperties();
	property object_base = object->getProp_base();
	property object_level = object->getProp_level();

	// NPC cannot have attribution points
	//
	// TODO: Player class will need an interface for this 
	// Also a GUI need to be created to assign theses points.

	freepoints+=2;
	skillspoints+=1;


	// Properties that are not changed by this:
	// level in "base" and "level", only used in "properties"
	// experience, dotduration, skillevel
	object_prop.level += 1;
	object_prop.maxdamage = object_base.maxdamage+(object_level.maxdamage*object_prop.level);
	object_prop.mindamage = object_base.mindamage+(object_level.mindamage*object_prop.level);
	object_prop.armor = object_base.armor+(object_level.armor*object_prop.level);
	object_prop.magic_armor = object_base.magic_armor+(object_level.magic_armor*object_prop.level);
	object_prop.maxlife = object_base.maxlife+(object_level.maxlife*object_prop.level);
	// There no mana to use right now (no skills yet)
	//object_prop.maxmana = object_base.maxmana+(object_level.maxmana*object_prop.level);
	object_prop.hurt_resist = object_base.hurt_resist+(object_level.hurt_resist*object_prop.level);
	object_prop.regenlife = object_base.regenlife+(object_level.regenlife*object_prop.level);
	object_prop.regenmana = object_base.regenmana+(object_level.regenmana*object_prop.level);
	object_prop.dodge_prop = object_base.dodge_prop+(object_level.dodge_prop*object_prop.level);
	object_prop.hit_prob = object_base.hit_prob+(object_level.hit_prob*object_prop.level);

	// if the level increased put the health back at max life (should have a way to activate or deactivate it on the need
	object_prop.life = object_prop.maxlife;

	// put the stuff back into the dynamic object
	object->setProperties(object_prop);
	
}

bool Combat::percent(f32 percent)
{
	if (chances(1,100) > round32(percent))
		return false;
	else
		return true;
}

int Combat::chances(int min, int max)
// Use a random value to determinate the result
// Example damage done ranging from 10 to 100 points min=10, max=100
{
	int result = (max-min) + min;
	if (result<1)
		return 0;
	else
		return rand() % result;
}

void Combat::dumpProperties(DynamicObject* object)
{
	printf ("\nObject: %s\n",object->getName().c_str());
	printf ("----------------------------------------------------------------\n");
	printf ("PROPERTIE NAME            |  Current   |  Base     |  Level     |\n");
	printf ("-----------------------------------------------------------------\n");
	printf ("Life (Hit points/hp)      |  %4i      |   %4i    |   %4i     |\n",object->getProperties().life,object->getProp_base().life,object->getProp_level().life);
	printf ("Experience                |  %4i      |   %4i    |   %4i     |\n",object->getProperties().experience,object->getProp_base().experience,object->getProp_level().experience);
	printf ("Money (Gold)              |  %4i      |   %4i    |   %4i     |\n",object->getProperties().money,object->getProp_base().money,object->getProp_level().money);
	printf ("Level                     |  %4i      |   %4i    |   %4i     |\n",object->getProperties().level,object->getProp_base().level,object->getProp_level().level);
	printf ("-----------------------------------------------------------------\n");
	printf ("Minimum damage            |  %4i      |   %4i    |   %4i     |\n",object->getProperties().mindamage,object->getProp_base().mindamage,object->getProp_level().mindamage);
	printf ("Maximum damage            |  %4i      |   %4i    |   %4i     |\n",object->getProperties().maxdamage,object->getProp_base().maxdamage,object->getProp_level().maxdamage);
	printf ("Hurt resistance (%%)       |  %4i      |   %4i    |   %4i     |\n",object->getProperties().hurt_resist,object->getProp_base().hurt_resist,object->getProp_level().hurt_resist);
	printf ("Dodge probability(%%)      |  %4i      |   %4i    |   %4.2f     |\n",(int)object->getProperties().dodge_prop,(int)object->getProp_base().dodge_prop,object->getProp_level().dodge_prop);
	printf ("Hit probability(%%)        |  %4i      |   %4i    |   %4.2f     |\n",(int)object->getProperties().hit_prob,(int)object->getProp_base().hit_prob,object->getProp_level().hit_prob);
	printf ("-----------------------------------------------------------------\n");
	printf ("Armor                     |  %4i      |   %4i    |   %4i     |\n",object->getProperties().armor,object->getProp_base().armor,object->getProp_level().armor);
	printf ("Magic Armor               |  %4i      |   %4i    |   %4i     |\n",object->getProperties().magic_armor,object->getProp_base().magic_armor,object->getProp_level().magic_armor);
	printf ("-----------------------------------------------------------------\n");
	printf ("Regen life points per tick|  %4i      |   %4i    |   %4i     |\n",object->getProperties().regenlife,object->getProp_base().regenlife,object->getProp_level().regenlife);
	printf ("Regen mana points per tick|  %4i      |   %4i    |   %4i     |\n",object->getProperties().regenmana,object->getProp_base().regenmana,object->getProp_level().regenmana);
	printf ("Dot(poison) duration      |  %4i      |   %4i    |   %4i     |\n",object->getProperties().dotduration,object->getProp_base().dotduration,object->getProp_level().dotduration);
	printf ("----------------------------------------------------------------\n");
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