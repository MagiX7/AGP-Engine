///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef MODEL

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(binding = 1, std140) uniform LocalParams
{
	mat4 model;
	mat4 mvp;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;

out vec3 vPosition;
out vec3 vWorldPosition;
out vec2 vTexCoords;
out vec3 vNormals;

//uniform mat4 model;
//uniform mat4 viewProj;
//uniform mat4 projection;

void main()
{
	vec3 pos = aPosition;

	gl_Position = mvp * vec4(pos, 1);
	vTexCoords = aTexCoord;
	vNormals = (model * vec4(aNormals, 0)).xyz;
	vPosition = aPosition;
	vWorldPosition = (model * vec4(aPosition, 1)).xyz;
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
	Light uLights[16];
};


layout(location = 0) uniform sampler2D uTexture;
uniform float smoothness;

in vec3 vPosition;
in vec3 vWorldPosition;
in vec2 vTexCoords;
in vec3 vNormals;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalsColor;
layout(location = 2) out vec4 positionColor;
layout(location = 3) out vec4 depthColor;
layout(location = 4) out vec4 specularColor;


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));	
}


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
	vec4 col = vec4(0);
	
	// Forward
	if (renderMode == 0)
	{
		vec4 tex = texture2D(uTexture, vTexCoords);
		//col = vec4(lightCol, 1) * tex;

		vec3 lightColor = vec3(0);
		vec3 viewDir = normalize(uCamPos - vPosition);

		for (int i = 0; i < uLightCount; ++i)
		{
			// Directional
			if(uLights[i].type == 0)
			{
				lightColor += CalcDirLight(uLights[i], vNormals, viewDir);
			}
			// Point
			else if(uLights[i].type == 1)
			{
				lightColor += CalcPointLight(uLights[i], vNormals, vWorldPosition, viewDir);
			}
		}

		col = tex * vec4(lightColor, 1);

	}
	// Deferred
	else
	{
		// If deferred, just output the albedo
		col = texture2D(uTexture, vTexCoords);
		//col.a = smoothness;
	}

	fragColor = col;
	normalsColor = vec4(vNormals, 1);
	positionColor = vec4(vWorldPosition, 1);

	float depth = LinearizeDepth(gl_FragCoord.z) / uFar;
	depthColor = vec4(vec3(depth), 1);
}

#endif
#endif