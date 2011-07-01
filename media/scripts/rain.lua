function setRain(intensity, speed)

  if(intensity == nil) then intensity = 0 end
  if(speed == nil) then speed = 100 end
  
  if(intensity > 100) then intensity = 100 end
  
  setFogRange(10,1000 + (100-intensity)*10 )
  setFogColor(100,100,110)

  setWeatherPr(intensity, speed)
  
end
