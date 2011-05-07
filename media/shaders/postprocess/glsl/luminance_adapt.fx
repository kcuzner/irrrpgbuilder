uniform sampler2D texture0; // PrevLumBuffer
uniform sampler2D texture1; // TargetLumBuffer
uniform float EffectStrength;

void main()
{
	vec4 prevLum = texture2D(texture0, gl_TexCoord[0].xy);
	vec4 targetLum = texture2D(texture1, gl_TexCoord[0].xy);
	gl_FragColor = saturate(prevLum+(targetLum-prevLum)*EffectStrength);
}
