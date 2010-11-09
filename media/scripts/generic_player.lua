--This is a simple example for the player script

function onLoad()
  print("playerOnload")
  setPlayerLife(100) 
end

function step()
  if (getPlayerLife() == 0) then
    showBlackScreen("YOU LOSE!")
    sleep(0.5)
    answer = showDialogQuestion("Do you want to continue the game?")
    if (answer == true) then   
      hideBlackScreen()
      setPlayerLife(100)     
    end
  end
end

