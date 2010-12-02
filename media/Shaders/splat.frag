uniform sampler2D terrainLayer0;
uniform sampler2D terrainLayer1;
uniform sampler2D terrainLayer2;
uniform sampler2D terrainLayer3;
uniform sampler2D terrainLayer4;


uniform int terrainTextureScale;
uniform int terrainScale;

uniform bool editingTerrain;

uniform vec4 AmbientLight;

varying vec4 texCoord;

varying vec4 normalFace;

varying float posY;

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
	//vec4 tex4    = texture2D( terrainLayer4, texCoord.xy );//ShadowMap

	//tex1 = mix( tex0, tex1, min(1-normalFace.y,1) );
	//tex2 = mix( tex1, tex2, posY );//a1.g
	
	vec4 tex10;
	
	if(posY >= 0)
	{
	  tex10 = mix( tex1, tex3, (posY/scale));
	  tex10 = mix( tex10, tex2, (min(1-normalFace.y-0.2,1)/scale) );
	}
	else
	{
	  tex10 = mix( tex1, tex2, (min(1-(normalFace.y)-(0.2),1)) );
	  tex10 = mix( tex10, tex0, min(1,-(posY/scale)*10));
	}
	
	if(posY>(0.01 * scale) && editingTerrain) tex10*=vec4(1,0.6,0.4,1);
	if(posY<-(0.01 * scale) && editingTerrain) tex10*=vec4(1,0.6,0.4,1);
	
	//tex2 = mix( tex3, tex2, 1-a1.b);
	
	tex10 *= gl_Color;

  	vec4 finalColor = tex10 * AmbientLight;

	float fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
	gl_FragColor = mix(gl_Fog.color,finalColor, fog);
}


