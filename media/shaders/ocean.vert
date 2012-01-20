varying float posY;
uniform int terrainScale;

void main(void)
{
	float scale = (terrainScale * 0.05);
			
	posY = gl_Vertex.y;
	
	//gl_Vertex.y = -scale;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	//gl_Position = ftransform();
	vec4 vertex = gl_Vertex;
	vertex.y = -scale;
	gl_Position = gl_ModelViewProjectionMatrix * vertex;

	gl_FogFragCoord = gl_Position.z;
}
