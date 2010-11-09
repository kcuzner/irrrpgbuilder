local a = 0
local b = 0

function hitplayer()
  decreasePlayerLife(1)
  a = 0
end

function step()
  local x,y,z = getObjectPosition("player")
  if(distanceFrom(x,y,z) < 1.5) then
    showObjectLabel()
    if(distanceFrom(x,y,z) < 0.6) then
        if (a==0)then
          setFrameLoop(0,0) 
          programAction(0.5, hitplayer)
          a = 1
          b = 0
        end  
      else
        if (b==0) then
          setAnimation("walk")
          b=1
        end
        chaseObject("player",0.01,0.5,2)
      end
  else
    hideObjectLabel()
    walkRandomly()
  end
end

function onLoad()
  setEnemy()
  life = 50
  setAnimation("walk")
  setObjectLabel("Enemy life:"..life.."/50")
end

function onClicked()
  if (life > 0) then
    life = life - 1
    setObjectLabel("Enemy life:"..life.."/50")
  elseif (life == 0) then
    setEnabled(false)
  end
end

