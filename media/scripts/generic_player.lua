--This is a simple example for the player script

function die()
  showBlackScreen("YOU LOSE!")
  sleep(0.5)
  answer = showDialogQuestion("Do you want to continue the game?")
  if (answer == true) then   
    hideBlackScreen()
    setPlayerLife(100)
	setAnimation("prespawn")	
  end
  dietoggle=false
end

function regen()
  increasePlayerLife(getPropertie("regenlife"))
  print("regen was called!")
  regentoggle=false
end

function onLoad()
  print("playerOnload")
  setPlayerLife(100)
  regentoggle=false
  dietoggle=false
end

function step()
  if ( getPlayerLife() < getPropertie("maxlife") and regentoggle==false) then
      programAction(5,regen)
      regentoggle=true
  end
  if ( getPlayerLife() == 0 and dietoggle==false) then
    programAction(3,die)
    dietoggle=true
  end
end


