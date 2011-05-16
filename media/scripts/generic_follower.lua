local following=false;

-- Simple follower script
function setProperties()
-- Using the new properties attributes, can set the NPC combat attributes
	setPropertie("life",50) -- current life of the NPC
	setPropertie("maxlife",50) -- current Maximum life of this NPC
	setPropertie("experience",10) -- given experience to attacker if killed
	setPropertie("mindamage",2) -- minimum damage inflicted
	setPropertie("maxdamage",5) -- maximum damage inflicted
	setPropertie("hurtresist",50) -- resistance to attacks in %. If obtained, trigger the "hurt" animation then return to idle, breaking the current attack
end

function onClicked()
-- Will ask the player if it want it to follow him
-- There a is following "mode" that decide the type of question to ask
  if (following) then
    showDialogQuestion("Do you want that I stop following you?")
   else 
    showDialogQuestion("Do you want that I follow you?")
 end

function onAnswer()
-- Here if we get the answer, decide what to do.
  answer = getAnswer()
  print ("answer given!")
  
  if (answer and following) then
    following=false
	return
  end
  if (answer and not following) then
    following=true
	return
  end
end

function onUpdate()
  if (following) then
	name = getName()..": "..getPropertie("life").."/"..getPropertie("maxlife")
	local x,y,z = getObjectPosition("player")
	if (getPropertie("life") == 0) then setEnabled(false) end
	  chaseObject("player",0.8,160,400)
	  if(distanceFrom(x,y,z) < 144) then 
		setObjectLabel(name)
		showObjectLabel()
	  end
    else
      hideObjectLabel()
    end
  end
end

function onLoad()
	setProperties();
 end