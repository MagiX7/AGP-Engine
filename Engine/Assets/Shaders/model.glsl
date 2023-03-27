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

out vec2 vTexCoords;
out vec3 vNormals;

//uniform mat4 model;
//uniform mat4 viewProj;
//uniform mat4 projection;

void main()
{
	vec3 pos = aPosition;
	//pos.z *= -1.0;

	gl_Position = mvp * vec4(pos, 1);
	vTexCoords = aTexCoord;
	vNormals = (model * vec4(aNormals, 0)).xyz;
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
	vec3 uCamPos;
	unsigned int uLightCount;
	Light uLights[16];
};


layout(location = 0) uniform sampler2D uTexture;

in vec2 vTexCoords;
in vec3 vNormals;

layout(location = 0) out vec4 fragColor;

void main()
{
	//fragColor = vec4(normalize(vNormals), 1);

	vec3 lightCol = max(dot(uLights[0].position, vNormals), 0.0) * uLights[0].diffuse * uLights[0].intensity;
	vec3 tex = texture(uTexture, vTexCoords).rgb;

	vec3 finalColor = lightCol * tex;
	//finalColor = vNormals;
	fragColor = vec4(finalColor, 1);
}

#endif
#endif