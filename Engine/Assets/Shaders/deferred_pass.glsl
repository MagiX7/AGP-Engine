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
	vec3 position; // Or direction for dir lights
	float intensity;
};

layout(binding = 0, std140) uniform GlobalParams
{
	int renderMode;
	float uNear;
	float uFar;
	vec3 uCamPos;
	unsigned int uLightCount;
	Light uLights[32];
};

layout(location = 0) uniform sampler2D uColorTexture;
layout(location = 1) uniform sampler2D uNormalsTexture;
layout(location = 2) uniform sampler2D uPositionTexture;
layout(location = 3) uniform sampler2D uDepthTexture;

uniform int renderTarget;

in vec3 vPosition;
in vec2 vTexCoords;

layout(location = 0) out vec4 fragColor;


vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//spec = step(0.5f, spec);
	
	vec3 diffuse = diff * light.diffuse * light.intensity;
	return diffuse;
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//spec = step(0.5f, spec);

	// Attenuation
	float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (distance * distance);
	
	attenuation *= light.intensity;

	vec3 diffuse = diff * light.diffuse * light.intensity;
	diffuse *= attenuation;

	return diffuse;
}

void main()
{
	switch (renderTarget)
	{
		// Color
		case 0:
		{
			// If background, discard
			if (texture2D(uDepthTexture, vTexCoords).x == 1.0)
				discard;

			// Retrieve data from textures
			vec3 position = texture2D(uPositionTexture, vTexCoords).xyz;
			vec3 viewDir = normalize(uCamPos - position);
			
			vec3 normals = texture2D(uNormalsTexture, vTexCoords).rgb;
			vec3 colorTexture = texture2D(uColorTexture, vTexCoords).rgb;
			float specular = texture2D(uColorTexture, vTexCoords).a;

			vec3 lightColor = vec3(0);

			// Calculate lighting
			for (int i = 0; i < uLightCount; ++i)
			{
				// Directional
				if(uLights[i].type == 0)
				{
					lightColor += CalcDirLight(uLights[i], normals, viewDir);
				}
				// Point
				else if(uLights[i].type == 1)
				{
					lightColor += CalcPointLight(uLights[i], normals, position, viewDir);
				}
			}

			fragColor = vec4(colorTexture * lightColor, 1);

			break;
		}

		// Normals
		case 1:
		{
			fragColor = texture2D(uNormalsTexture, vTexCoords);
			break;
		}

		// Positions
		case 2:
		{
			fragColor = texture2D(uPositionTexture, vTexCoords);
			break;
		}

		// Depth
		case 3:
		{
			fragColor = vec4(vec3(texture2D(uDepthTexture, vTexCoords).r), 1.0);
			break;
		}
	}
}

#endif
#endif