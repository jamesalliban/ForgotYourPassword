//#version 140
#extension GL_ARB_texture_rectangle : enable

uniform int isClipping;
uniform vec2 resolution;
uniform sampler2D tex0;
uniform float farClip;
uniform float nearClip;
uniform float nearFloorClip;
uniform float farFloorClip;
uniform float nearCeilingClip;
uniform float farCeilingClip;
uniform float nearLeftClip;
uniform float farLeftClip;
uniform float nearRightClip;
uniform float farRightClip;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp)
{
	if (abs(inputMin - inputMax) < 0.000000001){
		return outputMin;
	} else {
		float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
        
		if(clamp){
			if(outputMax < outputMin){
				if( outVal < outputMax )outVal = outputMax;
				else if( outVal > outputMin )outVal = outputMin;
			}else{
				if( outVal > outputMax )outVal = outputMax;
				else if( outVal < outputMin )outVal = outputMin;
			}
		}
		return outVal;
	}
}


float rand(vec2 n)
{
    return 0.5 + 0.5 * fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main(void) 
{
	vec4 baseColor = texture2D( tex0, gl_TexCoord[0].st );
	
	vec4 col = baseColor;
	col.rgb *= 10.0;
	
	vec2 normPos = gl_TexCoord[0].st;// / resolution;
	
	float grey = (col.r + col.g + col.b) / 3.0;
	float z = grey;
	
	if (isClipping == 1)
	{
		if (z > farClip) // clip back
			z = 0.0;
		else if (z < nearClip) // clip front
			z = 0.0;
		else
		{	
			// clip floor
			float floor = map(grey, nearClip, farClip, nearFloorClip, farFloorClip, true);
			if (normPos.y > floor)
				z = 0.0;
			
			// clip ceiling
			float ceiling = map(grey, nearClip, farClip, nearCeilingClip, farCeilingClip, true);
			if (normPos.y < ceiling)
				z = 0.0;
				
			// clip left
			float left = map(grey, nearClip, farClip, nearLeftClip, farLeftClip, true);
			if (normPos.x < left)
				z = 0.0;
				
			// clip right
			float right = map(grey, nearClip, farClip, nearRightClip, farRightClip, true);
			if (normPos.x > right)
				z = 0.0;
		}
	}	
	
	col.rgb = vec3(z, z, z);
	
	
	gl_FragColor = col;
}
