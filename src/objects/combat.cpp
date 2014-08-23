#include "combat.h"

#include "../App.h"
#include "DynamicObject.h"
#include "DynamicObjectsManager.h"
#include "../LuaGlobalCaller.h"
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

int Combat::attack(DynamicObject* attacker, DynamicObject* defender)
{
	
	DynamicObject::cproperty attacker_prop = attacker->getProperties();
	DynamicObject::cproperty defender_prop = defender->getProperties();
	
	// Retrieve the current life meter on the defender
	u32 life=defender->getLife();

	//dumpProperties(attacker);
	//dumpProperties(defender);
	

	//GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	

	// This give the expected damage the character will have on another
	int damage = chances(attacker_prop.mindamage,attacker_prop.maxdamage);

	
	stringw temptext="";
	// Probable damage
	temptext=stringw(defender->getName().c_str());
	temptext+=" should have ";
	temptext+=stringw(damage);
	temptext+=" points of damage!!.";
	//GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

/*	// hit probability
	temptext=stringw(attacker->getName().c_str());
	temptext+=" have ";
	temptext+=stringw(attacker_prop.hit_prob).subString(0,4);
	temptext+="% hit probability.";
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// dodge probability
	temptext=stringw(defender->getName().c_str());
	temptext+=" have ";
	temptext+=stringw(defender_prop.dodge_prop).subString(0,4);
	temptext+="% dodge probability.";
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));
*/	

	// This one determine the hit and dodge probability, so will have the damage full or missed the hit.
	if (!percent(attacker_prop.hit_prob-defender_prop.dodge_prop))
	{
		damage = 0;
		
		//defender->update();
		#ifdef _MSC_VER
		// Missed!
		temptext=stringw(attacker->getName().c_str());
		temptext+=" missed ";
		temptext+=stringw(defender->getName().c_str());
		temptext+="!";
		//GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,160,0,0));
		#endif
	}
	life -= damage;
	#ifdef _MSC_VER
	if (damage>0)
	{
		// Damage final
		temptext=stringw(defender->getName().c_str());
		temptext+=" lost ";
		temptext+=stringw(damage);
		temptext+=" points of life!!.";
		//GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,160,0,0));
	}
	#endif

	// Basic "Hurt" state
	// So if the attacker got almost all his points the defender will be "hurt"
	// need to update the rules to use the "hurt resistance" propertie...
	//if (((damage+2)>attacker_prop.maxdamage) && damage>2)
	
	// Update (1/15/12)
	// Would have to check and calculate a chance of breaking an attack
	// Example: attacker attack another attacker, who will win the attack?
	if (damage>0)
	{	//if (defender->AI_State==AI_STATE_IDLE)
			//defender->setAnimation("hurt");
	}
	
	// limit the damage to the life of the defender.
	if (life<0) 
			life=0;
	
	// need to do the damage at the damage event
	//defender->setLife(life);
		
	// Position the target to the defender (Apply only on the player
	if (attacker->getName()==Player::getInstance()->getObject()->getName())
		DynamicObjectsManager::getInstance()->getTarget()->setPosition(defender->getPosition()+vector3df(0,0.1f,0));

	// Will calculate the level only if the player IS the attacker. NPC should not gain a level by defeating others
	if (attacker->getName()==Player::getInstance()->getObject()->getName() &&
		life==0)
	{
		// get the experience of the defender
		attacker_prop.experience += defender_prop.experience;
		// Set the properties of the attacker back since the experience increased
		attacker->setProperties(attacker_prop);
		// Determine if a level raise is needed
		u32 baseXP = attacker->getProp_base().experience;
		u32 levelXP = attacker->getProp_level().experience;
		u32 level = attacker_prop.level;
		if (attacker_prop.experience>(baseXP+(levelXP*(level*level))))
			updateLevel(attacker);

		temptext=stringw(defender->getName().c_str());
		temptext+=" was killed by ";
		temptext+=stringw(attacker->getName().c_str());
		temptext+="!!.";
		//GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,160,0,0));
		
		// Set the animation for the attacker when the defender is dead
		//defender=NULL;
		//attacker->setAnimation("idle");
		//attacker->clearEnemy();
		//defender->setAnimation("die");
		//DynamicObjectsManager::getInstance()->getTarget()->getNode()->setVisible(false);
			
	}
	
	// Names
	temptext="Attacker:";
	temptext+=stringw(attacker->getName().c_str());
	temptext+=" and defender: ";
	temptext+=stringw(defender->getName().c_str());
	temptext+="!!!";
	//GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));
	
	//GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	//GUIManager::getInstance()->setConsoleText("  COMBAT - COMBAT - COMBAT - COMBAT - COMBAT - COMBAT - COMBAT",SColor(255,128,0,128));
	//GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	
	//Player::getInstance()->updateDisplay();
	return damage;
}

void Combat::updateLevel(DynamicObject* object)
{
	DynamicObject::cproperty object_prop = object->getProperties();
	DynamicObject::cproperty object_base = object->getProp_base();
	DynamicObject::cproperty object_level = object->getProp_level();

	// NPC cannot have attribution points
	//
	// TODO: Player class will need an interface for this 
	// Also a GUI need to be created to assign theses points.

	freepoints+=2;
	skillspoints+=1;


	// Properties that are not changed by this:
	// level in "base" and "leve", only used in "properties"
	// experience, dotduration, skillevel
	object_prop.level += 1;
	object_prop.maxdamage = object_base.maxdamage+(object_level.maxdamage*object_prop.level);
	object_prop.mindamage = object_base.mindamage+(object_level.mindamage*object_prop.level);
	object_prop.armor = object_base.armor+(object_level.armor*object_prop.level);
	object_prop.magic_armor = object_base.magic_armor+(object_level.magic_armor*object_prop.level);
	object_prop.maxlife = object_base.maxlife+(object_level.maxlife*object_prop.level);
	// There no mana to use right now (no skills yet)
	//object_prop.maxmana = object_base.maxmana+(object_level.maxmana*object_prop.level);
	//object_prop.hurt_resist = object_base.hurt_resist+(object_level.hurt_resist*object_prop.level);
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
	if (chances(1,100) > (u32)round32(percent))
		return false;
	else
		return true;
}
u32 Combat::chances(u32 min, u32 max)
// Use a random value to determinate the result
// Example damage done ranging from 10 to 100 points min=10, max=100
{
	u32 result = (max-min);
	if (result<1)
		return 0;
	else
		return min + (rand() % result);
}

void Combat::dumpProperties(DynamicObject* object)
{
	stringw temptext="";
	stringw temptext2="";
	
	// send the dump in reverse order (the log show the latest element first) So this block is readable

	GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	// Hurt resistance (%)  
	temptext="Hurt resistance (%)       |  ";
	temptext+=(stringw((f32)object->getProperties().hurt_resist)+stringw("     ")).subString(0,4)+"%";
	temptext+="      |   ";
	temptext+=(stringw((f32)object->getProp_base().hurt_resist)+stringw("     ")).subString(0,4)+"%";
	temptext+="      |   ";
	temptext+=(stringw((f32)object->getProp_level().hurt_resist)+stringw("     ")).subString(0,4)+"%";
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// Dodge (%)  
	temptext="Dodge probability(%)      |  ";
	temptext+=(stringw(object->getProperties().dodge_prop)+stringw("     ")).subString(0,4)+"%";
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().dodge_prop)+stringw("     ")).subString(0,4)+"%";
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().dodge_prop)+stringw("     ")).subString(0,4)+"%";
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// Dodge (%)  
	temptext="Hit probability(%)        |  ";
	temptext+=(stringw(object->getProperties().hit_prob)+stringw("     ")).subString(0,4)+"%";
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().hit_prob)+stringw("%     ")).subString(0,4)+"%";
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().hit_prob)+stringw("%     ")).subString(0,4)+"%";
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));

	// Max Damage
	temptext="Maximum damage            |  ";
	temptext+=(stringw(object->getProperties().maxdamage)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().maxdamage)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().maxdamage)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// Min Damage
	temptext="Minimum damage            |  ";
	temptext+=(stringw(object->getProperties().mindamage)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().mindamage)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().mindamage)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));

	// Level
	temptext="Level                     |  ";
	temptext+=(stringw(object->getProperties().level)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().level)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().level)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// Money
	temptext="Money (Gold)              |  ";
	temptext+=(stringw(object->getProperties().money)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().money)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().money)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// Experience
	temptext="Experience                |  ";
	temptext+=(stringw(object->getProperties().experience)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().experience)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().experience)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	// Max life
	temptext="Maximum life              |  ";
	temptext+=(stringw(object->getProperties().maxlife)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().maxlife)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().maxlife)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	// life
	temptext="Current life              |  ";
	temptext+=(stringw(object->getProperties().life)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_base().life)+stringw("     ")).subString(0,5);
	temptext+="      |   ";
	temptext+=(stringw(object->getProp_level().life)+stringw("     ")).subString(0,5);
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));

	GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	GUIManager::getInstance()->setConsoleText("PROPERTIE NAME            |  Current    |   Base       |   Level  ",SColor(255,128,0,128));
	GUIManager::getInstance()->setConsoleText("----------------------------------------------------------------",SColor(255,128,0,128));
	temptext="Object:";
	temptext+=stringw(object->getName().c_str());
	GUIManager::getInstance()->setConsoleText(temptext.c_str(),SColor(255,128,0,128));
	GUIManager::getInstance()->setConsoleText(" ",SColor(255,128,0,128));
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