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
out mat3 TBN;
out mat4 vModel;

//uniform mat4 model;
//uniform mat4 viewProj;
//uniform mat4 projection;

void main()
{
	vec3 pos = aPosition;

	gl_Position = mvp * vec4(pos, 1);
	vTexCoords = aTexCoord;
	//vNormals = (model * vec4(normalize(aNormals), 0)).xyz;
	vNormals = aNormals;
	vPosition = aPosition;
	vWorldPosition = (model * vec4(aPosition, 1)).xyz;
	vModel = model;

	vec3 N = normalize(aNormals);
	vec3 T = aTangents;
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(T, N);
	TBN = mat3(T, B, N);

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


layout(location = 0) uniform sampler2D uAlbedoMap;
layout(location = 1) uniform sampler2D uNormalMap;
layout(location = 2) uniform sampler2D uMetallicMap;
layout(location = 3) uniform sampler2D uRoughnessMap;
uniform int hasNormalMap;
uniform int hasMetallicMap;
uniform int hasRoughnessMap;

in vec3 vPosition;
in vec3 vWorldPosition;
in vec2 vTexCoords;
in vec3 vNormals;
in mat3 TBN;
in mat4 vModel;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalsColor;
layout(location = 2) out vec4 positionColor;
layout(location = 3) out vec4 depthColor;
layout(location = 4) out vec4 specularColor;


const float PI = 3.14159265359;

// --------------------------------------------------------------------

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return  F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a2 = roughness * roughness;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return (a2) / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}


// --------------------------------------------------------------------

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));	
}


vec3 CalcDirLight(in Light light, in vec3 normal, in vec3 viewDir, in vec3 albedo, in float metallic, in float roughness, out vec3 F0)
{
	vec3 lightDir = normalize(light.position);
	vec3 halfway = normalize(viewDir + lightDir);

	float NdotL = max(dot(normal, lightDir), 0.0);
	float NdotV = max(dot(normal, viewDir), 0.0);
	float NdotH = max(dot(normal, halfway), 0.0);

	
	F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	float NDF = DistributionGGX(normal, halfway, roughness);
	float G = GeometrySmith(NdotV, NdotL, roughness);
	vec3 F = FresnelSchlick(max(dot(halfway, viewDir), 0.0), F0);

	vec3 numerator = NDF * F * G;
	float denominator = max((4 * NdotV * NdotL), 0.0001);
	vec3 specular = numerator / denominator;

	// Lambert Diffuse ========
	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= 1.0f - metallic;
	// Lambert Diffuse ========

	vec3 radiance = light.diffuse * light.intensity;

	return (kd * (albedo / PI) + specular) * radiance * NdotL;
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
	

	vec3 normal = normalize(TBN * (texture2D(uNormalMap, vTexCoords).rgb) * hasNormalMap
					+ vNormals * 1.0 - hasNormalMap);

	normal = normalize((vModel * vec4(normal, 0.0)).xyz);

	vec3 albedo = texture2D(uMetallicMap, vTexCoords).rgb;
	float metallic = texture2D(uMetallicMap, vTexCoords).r;
	float roughness = texture2D(uRoughnessMap, vTexCoords).r;

	// Forward
	if (renderMode == 0)
	{
		//vec4 tex = texture2D(uAlbedoMap, vTexCoords);
		//col = vec4(lightCol, 1) * tex;

		vec3 lightColor = vec3(0);
		vec3 viewDir = normalize(uCamPos - vPosition);

		vec3 F0 = vec3(0);

		for (int i = 0; i < uLightCount; ++i)
		{
			// Directional
			if (uLights[i].type == 0)
			{
				lightColor += CalcDirLight(uLights[i], normal, viewDir, albedo, metallic, roughness, F0);
			}
			// Point
			else if (uLights[i].type == 1)
			{
				lightColor += CalcPointLight(uLights[i], normal, vWorldPosition, viewDir);
			}
		}

		col = vec4(lightColor, 1);

	}
	// Deferred
	else
	{
		// If deferred, just output the albedo
		col = texture2D(uAlbedoMap, vTexCoords);
		//col.a = smoothness;
	}

	fragColor = col;
	normalsColor = vec4(normal, 1);
	positionColor = vec4(vWorldPosition, 1);

	//float depth = LinearizeDepth(gl_FragCoord.z) / uFar;
	//depthColor = vec4(vec3(depth), 1);
}

#endif
#endif