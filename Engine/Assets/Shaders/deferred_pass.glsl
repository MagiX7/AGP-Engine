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

layout(location = 3) uniform sampler2D uMetallicMap;
layout(location = 4) uniform sampler2D uRoughnessMap;

layout(location = 5) uniform samplerCube uIrradianceMap;
layout(location = 6) uniform samplerCube uSkyboxPrefilterMap;
layout(location = 7) uniform sampler2D uSkyboxBrdf;
layout(location = 8) uniform sampler2D uSkybox;

layout(location = 9) uniform sampler2D uSSAOTexture;
layout(location = 10) uniform sampler2D uDepthTexture;

uniform bool uSsaoEnabled;

in vec3 vPosition;
in vec2 vTexCoords;


layout(location = 0) out vec4 fragColor;


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

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));
}

void main()
{	
	// Retrieve data from textures
	vec3 position = texture2D(uPositionTexture, vTexCoords).xyz;
	vec3 viewDir = normalize(uCamPos - position);
	
	float depth = LinearizeDepth(texture2D(uDepthTexture, vTexCoords).r) / uFar;
	if (depth >= 0.99)
	{
		fragColor = texture2D(uSkybox, vTexCoords);
		return;
	}

	vec3 normal = texture2D(uNormalsTexture, vTexCoords).rgb;
	vec3 albedo = texture2D(uColorTexture, vTexCoords).rgb;
	float metallic = texture2D(uMetallicMap, vTexCoords).r;
	float roughness = texture2D(uRoughnessMap, vTexCoords).r;
	vec3 irradiance = texture(uIrradianceMap, normal).rgb;
	float ssao = texture2D(uSSAOTexture, vTexCoords).r;

	vec3 col = vec3(0);

	// Calculate lighting
	vec3 F0 = vec3(0);

	for (int i = 0; i < uLightCount; ++i)
	{
		// Directional
		if (uLights[i].type == 0)
		{
			col += CalcDirLight(uLights[i], normal, viewDir, albedo, metallic, roughness, F0);
		}
		// Point
		else if (uLights[i].type == 1)
		{
			//col += CalcPointLight(uLights[i], normal, position, viewDir);
			col += CalcPointLight(uLights[i], normal, position, viewDir, albedo, metallic, roughness, F0);
		}
	}

	

	vec3 F = FresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
	vec3 ks = F;
	vec3 kd = 1.0 - ks;
	kd *= 1.0 - metallic;
	vec3 diffuse = irradiance * albedo;

	if (!uSsaoEnabled)
		ssao = 1.0;
	vec3 ambient = kd * diffuse * ssao;
	
	//if (uSsaoEnabled)
		//ambient *= (ssao);

	vec3 R = reflect(-viewDir, normal);
	vec3 prefilteredColor = textureLod(uSkyboxPrefilterMap, R, roughness * 2.2).rgb;
	vec2 brdf = texture2D(uSkyboxBrdf, vec2(max(dot(normal, viewDir), 0.0)), roughness).rg;
	vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);	

	ambient += specular;
	
	col += ambient;
	

	//col = vec3(ssao);
	fragColor = vec4(col, 1);
}

#endif
#endif