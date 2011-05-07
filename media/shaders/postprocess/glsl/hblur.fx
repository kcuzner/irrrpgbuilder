uniform sampler2D texture0;
uniform float BufferWidth;
 
void main()
{
	float blurSize = 1.0/BufferWidth; 
	vec4 finalCol = vec4(0.0);

	// gauss distribution with mean:0 std:2
	float weight[9] = {
		0.0295,
		0.0673,
		0.1235,
		0.1786,
		0.2022,
		0.1786,
		0.1235,
		0.0673,
		0.0295
	};

	// blur in x (horizontal)
	// take the samples with the distance blurSize between them
	for (int i=0; i<9; i++)
		finalCol += texture2D(texture0, vec2(gl_TexCoord[0].x+(i-4)*blurSize, gl_TexCoord[0].y))*weight[i];

	gl_FragColor = finalCol;
} 

void main7x7()
{
	float blurSize = 1.0/BufferWidth; 
	vec4 finalCol = vec4(0.0);

	// gauss distribution with mean:0 std:1
	float weight[7] = {
		0.0050,
		0.0540,
		0.2410,
		0.4000,
		0.2410,
		0.0540,
		0.0050
	};

	// blur in x (horizontal)
	// take the samples with the distance blurSize between them
	for (int i=0; i<7; i++)
		finalCol += texture2D(texture0, vec2(gl_TexCoord[0].x+(i-3)*blurSize, gl_TexCoord[0].y))*weight[i];

	gl_FragColor = finalCol;
} 