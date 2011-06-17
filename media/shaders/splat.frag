uniform sampler2D terrainLayer0;
uniform sampler2D terrainLayer1;
uniform sampler2D terrainLayer2;
uniform sampler2D terrainLayer3;
uniform sampler2D terrainLayer4;

uniform float plateau;
uniform int terrainTextureScale;
uniform int terrainScale;

uniform bool editingTerrain;

uniform vec4 AmbientLight;
uniform vec3 mLightPos;
uniform vec3 mCamPos;

varying vec3 normal;
varying vec3 position;
varying vec4 worldCoord;

void main() 
{
    int scale = terrainScale / 4;
	if (terrainScale==1)
		scale = 1;
		
	vec2 texCoord = vec2(gl_TexCoord[0]);
	
	vec4 tex0    = texture2D( terrainLayer0, texCoord.xy*terrainTextureScale );
	vec4 tex1    = texture2D( terrainLayer1, texCoord.xy*terrainTextureScale );
	vec4 tex2    = texture2D( terrainLayer2, texCoord.xy*terrainTextureScale );
	vec4 tex3    = texture2D( terrainLayer3, texCoord.xy*terrainTextureScale );

	tex1 = mix( tex1, tex0, min(1-normal.y,1) );
	tex2 = mix( tex1, tex2, (position.y/scale));
	
	vec4 tex10;
	
	if(position.y >= 0)
	{
	  tex10 = mix( tex1, tex3, (position.y/scale));
	  tex10 = mix( tex10, tex2, (min(1-normal.y-0.2,1)) );
	}
	else
	{
	  tex10 = mix( tex1, tex2, min(1-normal.y-0.2,1));
	  tex10 = mix( tex10, tex0, min(1,-(position.y/scale)*10));
	}
	
	if(position.y>(plateau-2.5) && position.y<(plateau+2.5) && editingTerrain) tex10*=vec4(1,0.6,0.4,1);
		
	// Point light with attenuation calculation	
	vec4 diffuse;
	vec3 norm = normalize(normal);
	vec3 lightVector = mLightPos - worldCoord;
	float dist = length(lightVector);
	float attenuation = 1.0 /( gl_LightSource[0].constantAttenuation +
								gl_LightSource[0].linearAttenuation * dist +
								gl_LightSource[0].quadraticAttenuation * dist * dist);
								
	lightVector = normalize(lightVector);
	float nxDir = max(0.0, dot(norm, lightVector));
	
	// Directional light no attenuation
	vec4 sunDiffuse;
	vec3 sunVector = normalize(vec3(2500,5500,-5500) - worldCoord);
	float sunDir = max(0.0, dot(norm, sunVector));
	
	//Seem to cause seams with lighting
	diffuse = (gl_LightSource[1].diffuse * sunDir) + (gl_LightSource[0].diffuse * nxDir * attenuation);
	//diffuse = (gl_LightSource[1].diffuse * sunDir);
	
	//diffuse = AmbientLight +(gl_LightSource[0].diffuse * nxDir * attenuation);
		
		

	
	// Flat rendering with FOG possible	
  	//vec4 finalColor = tex10;
	
	// Rendering with 1 point light source
	vec4 finalColor = gl_LightSource[0].ambient + (diffuse * vec4(tex10.rgb, 1.0));
	
	float fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
	gl_FragColor = mix(gl_Fog.color,finalColor, fog);
	//mix(gl_Fog.color,finalColor, fog);
}


