local a = 0

function setProperties()
-- Using the new properties attributes, can set the NPC combat attributes. 
	setEnemy() -- this is an ennemy
	setPropertie("life",10) -- current life of the NPC
	setPropertie("maxlife",10) -- current Maximum life of this NPC
	setPropertie("experience",5) -- given experience to attacker if killed
	setPropertie("mindamage",0) -- minimum damage inflicted
	setPropertie("maxdamage",1) -- maximum damage inflicted
	setPropertie("hurtresist",50) -- resistance to attacks in %. If obtained, trigger the "hurt" animation then return to idle, breaking the current attack
	setPropertie("hit_prob",50)
	setPropertie("dodge_prob",12)
end

-- "onClicked" will trigger is the dynamic object is being clicked on by the user
function onClicked()
	print("Lua character is being clicked on!")
end

-- new game function
-- "onCollision" will trigger if the dynamic object got a collision with something
function onCollision()
end

-- "step" will trigger at each time interval (around 1/4 second)
function onUpdate()
  name = getName()..": "..getPropertie("life").."/"..getPropertie("maxlife")
  if (getPropertie("life") == 0) then 
	hideObjectLabel()
	return
  end
  local x,y,z = getObjectPosition("player")
   if(distanceFrom(x,y,z) < 288 and getPlayerLife()>0) then 
    if(distanceFrom(x,y,z) < 66) then
	  setAnimation("attack")
	else
      chaseObject("player",0.8,66,800)
    end
	setObjectLabel(name)
    showObjectLabel()
  else
    hideObjectLabel()
    walkRandomly()
  end
end

-- "onLoad" will trigger when the dynamic object is initialized (only once)
function onLoad()
	setProperties()
	blocked = false
end
