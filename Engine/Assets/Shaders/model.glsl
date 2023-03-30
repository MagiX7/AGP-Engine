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
	vWorldPosition = (model * vec4(aPosition, 0)).xyz;
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

in vec3 vPosition;
in vec3 vWorldPosition;
in vec2 vTexCoords;
in vec3 vNormals;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalsColor;
layout(location = 2) out vec4 depthColor;
//layout(location = 3) out float depth;
//layout(depth_equal) out float gl_FragDepth;


const float near = 0.1;
const float far = 1000;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{
	//fragColor = vec4(normalize(vNormals), 1);

	vec3 lightCol = max(dot(uLights[0].position, vNormals), 0.0) * uLights[0].diffuse * uLights[0].intensity;
	vec3 tex = texture(uTexture, vTexCoords).rgb;

	vec3 finalColor = lightCol * tex;
	fragColor = vec4(finalColor, 1);
	normalsColor = vec4(vNormals, 1);

	//float depth = (vPosition / (uCamPos - vPosition)).z;
	//depthColor = vec4(vec3((uCamPos - vPosition).z), 1);
	//depthColor = vec4(vec3(normalize(vPosition).z), 1);

	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	depthColor = vec4(vec3(depth), 1);

	//depth = gl_FragDepth;

	//gl_FragDepth = vPosition.z;
	
}

#endif
#endif