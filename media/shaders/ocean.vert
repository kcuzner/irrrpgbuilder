uniform float time;
uniform int terrainScale;

varying float posY;

void main(void)
{
	//float scale = (float(terrainScale) * 0.05);
			
	posY = gl_Vertex.y;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	vec4 vertex = gl_Vertex;
	
	vertex.y = sin( (vertex.y * 0.75)* (time/7) * 1.0) * cos( (vertex.y * 0.75) * (time/7)) * 3.0;
	
	vertex.y = vertex.y - 45.0;
	
	
	gl_Position = gl_ModelViewProjectionMatrix * vertex;

	gl_FogFragCoord = gl_Position.z;
}
