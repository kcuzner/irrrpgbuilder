uniform sampler2D texture0; //sceneBuffer
uniform float BufferWidth;
uniform float BufferHeight;

void main ()
{
	vec2 texSize = vec2(1.0/BufferWidth, 1.0/BufferHeight);
	vec2 texSamples[8] = {
		-1.0, -1.0,
		 0.0, -1.0,
		 1.0, -1.0,
		-1.0,  0.0,
		 1.0,  0.0,
		-1.0,  1.0,
		 0.0,  1.0,
	     1.0,  1.0,
	};

	vec4 finalCol = 9.0*texture2D(texture0, gl_TexCoord[0].xy);

	for(int i=0; i<8; i++)
		finalCol -= texture2D(texture0, gl_TexCoord[0].xy+texSize*texSamples[i]);

	gl_FragColor = finalCol;
}
