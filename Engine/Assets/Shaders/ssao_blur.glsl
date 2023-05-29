///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef SSAO_BLUR

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
	vec3[64] samples;
};

layout(location = 0) uniform sampler2D uSsaoTexture;

in vec2 vTexCoords;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(uSsaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(uSsaoTexture, vTexCoords + offset).r;
        }
    }
    fragColor = vec4(vec3(result / (4.0 * 4.0)), 1);
}

#endif
#endif