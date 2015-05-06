//#version 120
#extension GL_ARB_texture_rectangle : enable

uniform sampler2D tex0;
uniform float blurAmount;
uniform int numSamples;
uniform vec2 resolution;

void main()
{
    vec2 st = gl_TexCoord[0].st;
   
    vec4 color = texture2D(tex0, st) * float(numSamples);
	float pixUnit = 1.0 / resolution.x;
	float divideByThis = float(numSamples);
	for (int i = 1; i < numSamples; i++)
	{
		float sampleStrength = abs(float(i) - (float(numSamples) - 1.0)); 
		color += texture2D(tex0, st + vec2(pixUnit * float(i) * blurAmount, 0.0)) * sampleStrength;
		color += texture2D(tex0, st + vec2(pixUnit * float(-i) * blurAmount, 0.0)) * sampleStrength;
		divideByThis += sampleStrength * 2.0;
	}

	color /= divideByThis;
    gl_FragColor = color;
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}