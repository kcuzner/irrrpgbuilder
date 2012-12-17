uniform sampler2D terrainLayer0;
uniform sampler2D terrainLayer1;
uniform sampler2D terrainLayer2;
uniform sampler2D terrainLayer3;
uniform sampler2D terrainLayer4;

uniform float plateau;
uniform int terrainTextureScale;
uniform int terrainScale;

uniform int editingTerrain;

uniform vec4 AmbientLight;
uniform vec3 mLightPos;
uniform vec3 mCamPos;

varying vec3 normal;
varying vec3 position;
varying vec4 worldCoord;

void main() 
{
    float scale = float(terrainScale) / 4.0;
	if (terrainScale==1)
		scale = 1.0;
		
	vec2 texCoord = vec2(gl_TexCoord[0]);
	
	vec4 tex0    = texture2D( terrainLayer0, texCoord.xy*float(terrainTextureScale) );
	vec4 tex1    = texture2D( terrainLayer1, texCoord.xy*float(terrainTextureScale) );
	vec4 tex2    = texture2D( terrainLayer2, texCoord.xy*float(terrainTextureScale) );
	vec4 tex3    = texture2D( terrainLayer3, texCoord.xy*float(terrainTextureScale) );

	tex1 = mix( tex1, tex0, min(1.0-normal.y,1.0) );
	tex2 = mix( tex1, tex2, (position.y/float(scale)));
	
	vec4 tex10;
	
	float pos2 = (position.y/scale)*10;
	
	if(position.y >= 0.0)
	{
	  tex10 = mix( tex1, tex3, (position.y/float(scale)));
	  tex10 = mix( tex10, tex2, (min(1.0-normal.y-0.2,1.0)) );
	}
	else
	{
	  tex10 = mix( tex1, tex2, min(1.0-normal.y-0.2,1.0));
	  tex10 = mix( tex10, tex0, min(1.0,-(position.y/float(scale))*10.0));
	}
	
	if(position.y>(plateau-2.5) && position.y<(plateau+2.5) && editingTerrain==1) tex10*=vec4(1.0,0.6,0.4,1.0);
	float fog = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale;
	gl_FragColor = mix(gl_Fog.color,(tex10*AmbientLight), fog);
}


