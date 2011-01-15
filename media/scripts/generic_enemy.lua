local a = 0

function hitplayer()
  decreasePlayerLife(math.random(1,4))
  a = 0
end

function step()
  life = getObjectLife(objName);
  if (life == 0) then setEnabled(false) end
  local x,y,z = getObjectPosition("player")
  if(distanceFrom(x,y,z) < 144) then 
    setObjectLabel("Enemy life:"..life.."/50")
    showObjectLabel()
    if(distanceFrom(x,y,z) < 60) then
		--chaseObject("player",0.8,54,600)
		setAnimation("idle")
		setFrameLoop(0,0);
		--move(0);
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

function onLoad()
  setEnemy()
  setObjectLife(objName,50);
end

function onClicked()
end

