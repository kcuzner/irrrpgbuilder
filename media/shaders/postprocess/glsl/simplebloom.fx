uniform sampler2D texture0;
uniform float EffectStrength;

void main()
{
	vec4 finalCol = texture2D(texture0, gl_TexCoord[0].xy);
	vec4 col0 = finalCol;
	
	// higher contrast
	finalCol.rgb = pow(col0.rgb, EffectStrength);
	
	// mix colors
	finalCol.rgb *= col0;
	finalCol.rgb += col0;

	gl_FragColor = finalCol;
}
