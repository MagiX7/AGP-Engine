///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef SSAO

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

out vec2 vTexCoords;

void main()
{
	gl_Position = vec4(aPosition, 1);
	vTexCoords = aTexCoord;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

layout(binding = 2, std140) uniform SSAOParams
{
	vec2 viewportSize;
	mat4 projection;
	int noiseSize;
	int kernelSize;
	float radius;
	float bias;
	float strength;
	float uNear;
	float uFar;
	vec3[64] samples;
};

layout(location = 0) uniform sampler2D uNormalsTexture;
layout(location = 1) uniform sampler2D uPositionTexture;
layout(location = 2) uniform sampler2D uNoiseTexture;
layout(location = 3) uniform sampler2D uDepthTexture;


in vec2 vTexCoords;

layout(location = 0) out vec4 fragColor;


float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * uNear * uFar) / (uFar + uNear - z * (uFar - uNear));	
}

vec3 GetPosition(in float depth, in vec2 v)
{
	float xndc = gl_FragCoord.x / v.x * 2.0 - 1.0;
	float yndc = gl_FragCoord.y / v.y * 2.0 - 1.0;
	float zndc = depth * 2.0 - 1.0;
	vec4 posNDC = vec4(xndc, yndc, yndc, 1.0);
	vec4 posView = inverse(projection) * posNDC;
	return posView.xyz / posView.w;
}

void main()
{
	const vec2 noiseScale = viewportSize * noiseSize;

	vec3 fragPos = texture2D(uPositionTexture, vTexCoords).xyz;
	vec3 normal = normalize(texture2D(uNormalsTexture, vTexCoords).rgb /** 2.0 - 1.0*/);
	vec3 randomVec = normalize(texture2D(uNoiseTexture, vTexCoords * noiseScale).xyz); 

	vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN       = mat3(tangent, bitangent, normal);  


	float occlusion = 0.0;
	for( int i = 0; i < kernelSize; ++i)
	{
		// Get sample position
		vec3 samplePos = TBN * samples[i]; // from tangent to view-space
		samplePos = fragPos + samplePos * radius; 
    
		vec4 offset = vec4(samplePos, 1.0);
		offset      = projection * offset;    // from view to clip-space
		offset.xyz /= offset.w;               // perspective divide
		offset.xyz  = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0  

		float sampleDepth = texture2D(uPositionTexture, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;

		
	}  

	occlusion = 1.0 - (occlusion / kernelSize);
	
	//fragColor = vec4(vec3(occlusion * occlusion), 1);
	fragColor = vec4(vec3(pow(occlusion, strength)), 1);
}

#endif
#endif