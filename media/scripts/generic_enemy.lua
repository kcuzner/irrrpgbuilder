local a = 0

function setProperties()
-- Using the new properties attributes, can set the NPC combat attributes
	setEnemy() -- this is an ennemy
	setPropertie("life",50) -- current life of the NPC
	setPropertie("maxlife",50) -- current Maximum life of this NPC
	setPropertie("experience",10) -- given experience to attacker if killed
	setPropertie("mindamage",2) -- minimum damage inflicted
	setPropertie("maxdamage",5) -- maximum damage inflicted
	setPropertie("hurtresist",50) -- resistance to attacks in %. If obtained, trigger the "hurt" animation then return to idle, breaking the current attack
end

function hitplayer()
  attack("player")
  a = 0
end

-- "onClicked" will trigger is the dynamic object is being clicked on by the user
function onClicked()
	print("Lua character is being clicked on!")
end

-- new game function
-- "onCollision" will trigger if the dynamic object got a collision with something
function onCollision()
	print("Lua character had a collision");
	blocked=true;
end

-- "step" will trigger at each time interval (around 1/4 second)
function onUpdate()
  name = getName()..": "..getPropertie("life").."/"..getPropertie("maxlife")
  if (getPropertie("life") == 0) then setEnabled(false) end
  local x,y,z = getObjectPosition("player")
  if(distanceFrom(x,y,z) < 288) then 
    setObjectLabel(name)
    showObjectLabel()
    if(distanceFrom(x,y,z) < 60 or blocked) then
		setAnimation("idle")
		setFrameLoop(0,0);
		-- reset the collision
		if (blocked) then
			blocked=false;
			a=1
		end
		  if (a==0)then
		  -- wait 0.5 second then do the attack (the animation is not present for the attack)
          programAction(0.5, hitplayer)
          a = 1 -- this will "block the next attacks until it has been really done"
        end  
      else
        chaseObject("player",0.8,60,800)
      end
  else
    hideObjectLabel()
    walkRandomly()
  end
end

-- "onLoad" will trigger when the dynamic object is initialized (only once)
function onLoad()
	setProperties();
	blocked = false;
 end


