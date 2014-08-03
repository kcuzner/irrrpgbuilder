-- Global script for the potions
function HealthPotion()
     -- Increase the player life
   setPlayerLife(getPlayerLife()+10)

   -- Remove the item from the inventory as it's being used.   
   removePlayerItem("HealthPotion")
end
