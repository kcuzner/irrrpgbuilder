-- Global script for the potions
function HealthPotion()
   setPlayerLife(getPLayerLife()+10)
   removePlayerItem(HealthPotion)
end
