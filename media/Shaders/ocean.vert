varying float posY;

void main(void)
{
	posY = gl_Vertex.y;
	
	gl_Vertex.y = -0.1;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = ftransform();

	gl_FogFragCoord = gl_Position.z;
}
