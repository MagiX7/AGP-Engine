///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef FORWARD_PASS

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(binding = 1, std140) uniform LocalParams
{
	mat4 model;
	mat4 view;
	mat4 projection;
};

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;

out vec3 vWorldPosition;
out vec2 vTexCoords;
out vec3 vNormal;
out mat3 TBN;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1);
	vTexCoords = aTexCoord;
	vNormal = vec3(model * vec4(aNormals, 0));
	vWorldPosition = (model * vec4(aPosition, 1)).xyz;

    vec3 T = normalize(mat3(model) * aTangents);
    vec3 N = normalize(mat3(model) * aNormals);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

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
	bool uRenderSkybox;
	bool uEnableSkyboxReflections;
	unsigned int uLightCount;
	Light uLights[32];
};


uniform vec3 uAlbedoColor;
layout(location = 0) uniform sampler2D uAlbedoMap;
layout(location = 1) uniform sampler2D uNormalMap;
layout(location = 2) uniform sampler2D uMetallicMap;
layout(location = 3) uniform sampler2D uRoughnessMap;
layout(location = 5) uniform samplerCube irradianceMap;
layout(location = 6) uniform samplerCube skyboxPrefilterMap;
layout(location = 7) uniform sampler2D skyboxBrdf;

uniform int hasAlbedoMap;
uniform int hasNormalMap;
uniform int hasMetallicMap;
uniform int hasRoughnessMap;

in vec3 vWorldPosition;
in vec2 vTexCoords;
in vec3 vNormal;
in mat3 TBN;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalsColor;
layout(location = 2) out vec4 positionColor;
layout(location = 3) out vec4 metallicColor;
layout(location = 4) out vec4 roughnessColor;
layout(location = 5) out vec4 albedoColor;
layout(location = 6) out vec4 depthColor;


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
	denom = max(denom, 0.00001);

	return a2 / denom;
}

float GeometrySchlickGGX(float X, float roughness)
{
	float k = roughness / 2.0;
	float denom = X * (1.0 - k) + k;
	denom = max(denom, 0.00001);

	return X / denom;
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
	float denominator = 4 * max((NdotV * NdotL), 0.0001);
	vec3 specular = numerator / denominator;


	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= (1.0f - metallic);


	vec3 radiance = light.diffuse * light.intensity;

	return (kd * (albedo / PI) + specular) * radiance * NdotL;
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, in vec3 albedo, in float metallic, in float roughness, out vec3 F0)
{
	vec3 lightDir = normalize(light.position - fragPos);

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
	float denominator = 4 * max((NdotV * NdotL), 0.0001);
	vec3 specular = numerator / denominator;


	vec3 ks = F;
	vec3 kd = vec3(1.0) - ks;
	kd *= (1.0f - metallic);

	float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (distance * distance);
	attenuation *= light.intensity;
	
	vec3 radiance = light.diffuse * light.intensity * attenuation;

	return (kd * (albedo / PI) + specular) * radiance * NdotL;
}


void main()
{
	vec3 normal = normalize(TBN * (texture2D(uNormalMap, vTexCoords).rgb * 2 - 1) * hasNormalMap
					+ (vNormal) * (1.0 - hasNormalMap));

	normal = normalize(normal);

	vec3 albedo = texture2D(uAlbedoMap, vTexCoords).rgb * uAlbedoColor * hasAlbedoMap + uAlbedoColor * (1 - hasAlbedoMap);
	float metallic = texture2D(uMetallicMap, vTexCoords).r * hasMetallicMap + 0.0 * (1 - hasMetallicMap);
	float roughness = texture2D(uRoughnessMap, vTexCoords).r * hasRoughnessMap + 0.0 * (1 - hasRoughnessMap);
	vec3 irradiance = texture(irradianceMap, normal).rgb;


	vec3 viewDir = normalize(uCamPos - vWorldPosition);

	vec3 col = vec3(0);
	vec3 F0 = vec3(0);

	for (int i = 0; i < uLightCount; ++i)
	{
		// Directional
		if (uLights[i].type == 0)
			col += CalcDirLight(uLights[i], normal, viewDir, albedo, metallic, roughness, F0);

		// Point
		else if (uLights[i].type == 1)
			col += CalcPointLight(uLights[i], normal, vWorldPosition, viewDir, albedo, metallic, roughness, F0);
	}

	if (uRenderSkybox && uEnableSkyboxReflections)
	{		
		vec3 F = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
		vec3 ks = F;
		vec3 kd = 1.0 - ks;
		kd *= 1.0 - metallic;
		vec3 diffuse = irradiance * albedo;
		vec3 ambient = kd * diffuse;
	
		vec3 R = reflect(-viewDir, normal);
		vec3 prefilteredColor = textureLod(skyboxPrefilterMap, R, roughness * 2.2).rgb;
		vec2 brdf = texture2D(skyboxBrdf, vec2(max(dot(normal, viewDir), 0.0)), roughness).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

		ambient += specular;
		col += ambient;
	}
	
	fragColor = vec4(col, 1);
	albedoColor = vec4(albedo, 1);
	normalsColor = vec4(normal, 1);
	positionColor = vec4(vWorldPosition, 1);
	metallicColor = vec4(vec3(metallic), 1);
	roughnessColor = vec4(vec3(roughness), 1);
}

#endif
#endif