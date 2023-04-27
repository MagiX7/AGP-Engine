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

in vec3 vPosition;
in vec2 vTexCoords;

uniform int renderTarget;

layout(location = 0) out vec4 fragColor;
//layout(location = 1) out vec4 normalsColor;
//layout(location = 2) out vec4 positionColor;
//layout(location = 3) out vec4 depthColor;

void main()
{
	switch(renderTarget)
	{
		// Color
		case 0:
		{
			vec3 colorTexture = texture(uColorTexture, vTexCoords).rgb;
			fragColor = vec4(colorTexture * vec3(0.8, 0.2, 0.2), 1);
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

	/*vec3 colorTexture = texture(uColorTexture, vTexCoords).rgb;
	fragColor = vec4(colorTexture * vec3(0.8, 0.2, 0.2), 1);

	depthColor = texture(uDepthTexture, vTexCoords);
	normalsColor = texture(uNormalsTexture, vTexCoords);
	positionColor = texture(uPositionTexture, vTexCoords);*/
}

#endif
#endif