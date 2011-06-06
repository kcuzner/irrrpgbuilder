varying vec3 normal;
varying vec3 position;

void main(void)
{
	//gl_Position = ftransform();
	gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FogFragCoord = gl_Position.z;
	normal = gl_Normal.xyz;
	position = gl_Vertex.xyz;
}
