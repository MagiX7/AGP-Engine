///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef DEFERRED

#if defined(VERTEX) ///////////////////////////////////////////////////

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

struct Light
{
	int type;
	vec3 diffuse;
	float intensity;
	vec3 position; // Or direction for dir lights
};

layout(binding = 0, std140) uniform GlobalParams
{
	int renderMode;
	float uNear;
	float uFar;
	vec3 uCamPos;
	unsigned int uLightCount;
	Light uLights[16];
};

layout(location = 0) uniform sampler2D uColorTexture;
layout(location = 1) uniform sampler2D uNormalsTexture;
layout(location = 2) uniform sampler2D uPositionTexture;
layout(location = 3) uniform sampler2D uDepthTexture;

uniform int renderTarget;

in vec3 vPosition;
in vec2 vTexCoords;

layout(location = 0) out vec4 fragColor;

void main()
{
	
	switch(renderTarget)
	{
		// Color
		case 0:
		{
			vec3 normals = texture2D(uNormalsTexture, vTexCoords).rgb;
			vec3 colorTexture = texture(uColorTexture, vTexCoords).rgb;

			vec3 lightCol = max(dot(uLights[0].position, normals), 0.0) * uLights[0].diffuse * uLights[0].intensity;
			fragColor = vec4(colorTexture * lightCol, 1);

			break;
		}

		// Normals
		case 1:
		{
			fragColor = texture(uNormalsTexture, vTexCoords);
			break;
		}

		// Positions
		case 2:
		{
			fragColor = texture(uPositionTexture, vTexCoords);
			break;
		}

		// Depth
		case 3:
		{
			fragColor = texture(uDepthTexture, vTexCoords);
			break;
		}
	}
}

#endif
#endif