///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef POST_PROCESS

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(binding = 1, std140) uniform LocalParams
{
	mat4 model;
	mat4 mvp;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec3 vPosition;
out vec2 vTexCoords;

void main()
{
	gl_Position = vec4(aPosition, 1);
	vTexCoords = aTexCoord;
	vPosition = aPosition;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////


layout(location = 0) uniform sampler2D uColorTexture;
layout(location = 1) uniform sampler2D uNormalsTexture;
layout(location = 2) uniform sampler2D uPositionTexture;
layout(location = 3) uniform sampler2D uDepthTexture;
layout(location = 4) uniform sampler2D uSsaoTexture;

in vec3 vPosition;
in vec2 vTexCoords;

uniform int renderTarget;
uniform float uNear;
uniform float uFar;
uniform bool uSsaoEnabled;

layout(location = 0) out vec4 fragColor;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));	
}

void main()
{
	vec4 color = vec4(0);

	float depth = LinearizeDepth(texture2D(uDepthTexture, vTexCoords).r) / uFar;
	int isBg = int((depth >= 0.99));
	switch (renderTarget)
	{
		// Color
		case 0:
		{
			vec3 colorTexture = texture(uColorTexture, vTexCoords).rgb;
			float ssao = texture2D(uSsaoTexture, vTexCoords).r;
			if (!uSsaoEnabled || isBg == 1) ssao = 1.0;

			color = vec4(colorTexture * ssao, 1);
			break;
		}

		// Normals
		case 1:
		{
			color = texture(uNormalsTexture, vTexCoords) * (1 - isBg) + vec4(0,0,0,1) * isBg;
			break;
		}

		// Positions
		case 2:
		{
			color = texture(uPositionTexture, vTexCoords) * (1 - isBg) + vec4(0,0,0,1) * isBg;
			break;
		}

		// Depth
		case 3:
		{
			float depth = LinearizeDepth(texture2D(uDepthTexture, vTexCoords).r) / uFar;
			color = vec4(vec3(depth), 1);
			break;
		}
	}

	fragColor = color;
}

#endif
#endif