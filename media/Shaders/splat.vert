uniform mat4 mWorldViewProj;
uniform mat4 mInvWorld;
uniform mat4 mTransWorld;
uniform vec3 mLightPos;
uniform vec4 mLightColor;

varying vec4 normalFace;
varying float posY;


void main(void)
{
	gl_Position = ftransform();
	
	posY = gl_Vertex.y;
	
	vec4 normal = vec4(gl_Normal, 0.0);
	normalFace = normal;
	normal = mInvWorld * normal;
	normal = normalize(normal);
	
	vec4 worldpos = gl_Vertex * mTransWorld;
	
	vec4 lightVector = worldpos - vec4(mLightPos,1.0);
	lightVector = normalize(lightVector);
	
	float tmp2 = dot(-lightVector, normal);
	
	vec4 tmp = mLightColor * tmp2;
	//gl_FrontColor = gl_BackColor = vec4(tmp.x, tmp.y, tmp.z, 1.0);
	gl_FrontColor = gl_BackColor = vec4(1, 1, 1, 1.0);
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_FogFragCoord = gl_Position.z;
	
	gl_TexCoord[1] = (mWorldViewProj * vec4(gl_Vertex.x,gl_Vertex.y,gl_Vertex.z,1));

	gl_FogFragCoord = gl_Position.z;
}
