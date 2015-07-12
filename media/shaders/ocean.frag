uniform sampler2D oceanNormalMap;
uniform sampler2D oceanReflection;

uniform float waterTime;
uniform int terrainScale;
uniform vec4 AmbientLight;

varying float posY;


void main()
{
	//float scale = float(terrainScale) * 0.145;
	float scale = 148.48;
	vec2 texCoord = vec2(gl_TexCoord[0]);
	
	vec4 tex0    = texture2D( oceanNormalMap, vec2(texCoord.x*float(terrainScale/50) + waterTime,texCoord.y*float(terrainScale/50)));
	vec4 tex1    = texture2D( oceanReflection, texCoord.xy*2.0 + tex0.r*0.5 );
	
	float transparent = ((-posY-45.0)/(scale/1.8));
	transparent = min (transparent, 1.0);

 	vec4 finalColor = tex1 * AmbientLight;
		
	float fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
	finalColor = mix(gl_Fog.color,finalColor, fog);
	finalColor.a = transparent;
	//finalColor.a = 1.0;
	
	gl_FragColor = finalColor;

	
}


