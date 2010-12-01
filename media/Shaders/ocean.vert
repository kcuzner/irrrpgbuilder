varying float posY;
uniform int terrainScale;

void main(void)
{
	int scale = (terrainScale * 0.05);
	posY = gl_Vertex.y;
	
	gl_Vertex.y = -(scale);
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = ftransform();

	gl_FogFragCoord = gl_Position.z;
}
