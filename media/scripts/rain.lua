function setRain(intensity)

  if(intensity > 10) then intensity = 10 end
  if(intensity < 0) then intensity = 0 end
  
  speed = 5 + 0.5*intensity
  
  setFogRange(0,2000 - intensity*100)
  setFogColor(100,100,100)

  setWeatherPr(intensity*250, speed*10, "../media/rain.png")
  
end

function setSnow(intensity)
  if(intensity > 10) then intensity = 10 end
  if(intensity < 0) then intensity = 0 end
  
  speed = 5 + 0.5*intensity
  
  setFogRange(0,2000 - intensity*100)
  setFogColor(255,255,255)

	setWeatherPr(intensity*2400, speed*0.5, "../media/snow.png")
  	--setWeatherPr(intensity*200, speed*0.5, "../media/rain.png")
end