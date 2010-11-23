local a = 0

function hitplayer()
  decreasePlayerLife(1)
  a = 0
end

function step()
  life = getObjectLife(objName);
  if (life == 0) then setEnabled(false) end
  local x,y,z = getObjectPosition("player")
  if(distanceFrom(x,y,z) < 96) then 
    setObjectLabel("Enemy life:"..life.."/50")
    showObjectLabel()
    if(distanceFrom(x,y,z) < 32) then
		chaseObject("player",0.8,32,600)
        if (a==0)then
          programAction(0.5, hitplayer)
          a = 1
        end  
      else
        chaseObject("player",0.8,32,600)
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

