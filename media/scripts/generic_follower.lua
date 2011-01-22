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

function step()
	name = getName()..": "..getPropertie("life").."/"..getPropertie("maxlife")
	local x,y,z = getObjectPosition("player")
	if (getPropertie("life") == 0) then setEnabled(false) end
	chaseObject("player",0.8,160,400)
	if(distanceFrom(x,y,z) < 144) then 
		setObjectLabel(name)
		showObjectLabel()
	end
end

function onLoad()
	setProperties();
 end