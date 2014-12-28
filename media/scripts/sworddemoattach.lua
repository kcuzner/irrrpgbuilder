-- Attach demo script

-- will attach the item to the head of the player.

-- The player must hadve the "head" attachment point defined in the XML




-- By Christian Clavet





function onUse()

    attachObject("player","head")

end


-- Called when the user click on the object or use the interaction button in proximity

function onClicked()

	-- When this object is clicked it will go inside the player backpack

	moveObjectLoot()

end




-- Called when the game start

function onLoad()
	
-- Set the display name and show it

	setObjectLabel("Sword")

	showObjectLabel()

	destroyAfterUse(false)

end