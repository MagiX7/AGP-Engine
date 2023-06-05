///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef GEOMETRY_PASS

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

uniform bool uIsSSAOEnabled;

out vec3 vPosition;
out vec3 vWorldPosition;
out vec2 vTexCoords;
out vec3 vNormal;
out mat3 TBN;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition, 1);
	vTexCoords = aTexCoord;
	vNormal = vec3(model * vec4(aNormals, 0));
	vPosition = (model * vec4(aPosition, 1)).xyz;

    vec3 T = normalize(mat3(model) * aTangents);
    vec3 N = normalize(mat3(model) * aNormals);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

	TBN = mat3(T, B, N);
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

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
uniform bool uIsSSAOEnabled;

in vec3 vPosition;
in vec2 vTexCoords;
in vec3 vNormal;
in mat3 TBN;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalsColor;
layout(location = 2) out vec4 positionColor;
layout(location = 3) out vec4 metallicColor;
layout(location = 4) out vec4 roughnessColor;
layout(location = 5) out vec4 depthColor;


void main()
{
	vec3 normal = normalize(TBN * (texture2D(uNormalMap, vTexCoords).rgb * 2 - 1) * hasNormalMap
					+ (vNormal) * (1.0 - hasNormalMap));

	normal = normalize(normal);

	vec3 albedo = texture2D(uAlbedoMap, vTexCoords).rgb * uAlbedoColor * hasAlbedoMap + uAlbedoColor * (1 - hasAlbedoMap);
	float metallic = texture2D(uMetallicMap, vTexCoords).r * hasMetallicMap + 0.0 * (1 - hasMetallicMap);
	float roughness = texture2D(uRoughnessMap, vTexCoords).r * hasRoughnessMap + 0.0 * (1 - hasRoughnessMap);

	fragColor = vec4(albedo, 1);
	normalsColor = vec4(normal, 1);
	positionColor = vec4(vPosition, 1);
	metallicColor = vec4(vec3(metallic), 1);
	roughnessColor = vec4(vec3(roughness), 1);
}

#endif
#endif