// based on the work of Shawn Hargreaves
// http://www.talula.demon.co.uk/postprocessing/postprocessing.html

uniform sampler2D texture0; //sceneBuffer
uniform sampler2D texture1; //pencil
uniform float Brightness;
uniform float EffectStrength;
uniform float ElapsedTime;

void main ()
{
	vec4 col0 = vec4(1.0, 0.0, 0.0, 0.0);
	vec4 col1 = vec4(0.0, 0.0, 1.0, 0.0);
	
	// noise
	vec2 noiseCoord;
	noiseCoord.x = 0.4*sin(ElapsedTime*50.0);
	noiseCoord.y = 0.4*cos(ElapsedTime*50.0);

	vec4 texCol = Brightness * texture2D(texture0, gl_TexCoord[0].xy);
	vec4 pen0 = texture2D(texture1, (gl_TexCoord[0].xy*EffectStrength)+noiseCoord);
	
	vec4 col2 = (1.0-texCol)*(pen0);
	
	float pen1 = dot(col2, col0);
	float pen2 = dot(col2, col1);
	
	vec4 finalCol=vec4(pen2, pen2, pen2, pen1);
	finalCol = (1-finalCol)*(1-finalCol.a);
	finalCol = saturate((finalCol -0.5)*2.0*texCol)*vec4(1.0, 0.9, 0.8, 1.0);
	gl_FragColor = finalCol;
}
