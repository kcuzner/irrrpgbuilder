local a = 0

function hitplayer()
  -- Does random damage from 1-4 point of health on the player
  --decreasePlayerLife(math.random(1,4))
  -- New command to use the combat system instead (use the default hardcoded properties for now)
  -- New lua commands will add the possibilities to setup the NPC properties
  -- Or load a defined set
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
function step()
  life = getObjectLife(objName);
  if (life == 0) then setEnabled(false) end
  local x,y,z = getObjectPosition("player")
  if(distanceFrom(x,y,z) < 144) then 
    setObjectLabel("Enemy life:"..life.."/50")
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
          programAction(0.5, hitplayer)
          a = 1
        end  
      else
        chaseObject("player",0.8,60,600)
      end
  else
    hideObjectLabel()
    walkRandomly()
  end
end

-- "onLoad" will trigger when the dynamic object is initialized (once)
function onLoad()
  setEnemy()
  setObjectLife(objName,50);
  blocked = false;
end


