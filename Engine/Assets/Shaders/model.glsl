///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef MODEL

#if defined(VERTEX) ///////////////////////////////////////////////////

layout(location = 0) in vec3 aPosition;
//layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
//layout(location = 3) in vec3 aTangents;
//layout(location = 4) in vec3 aBitangents;

out vec2 vTexCoords;

void main()
{
	vec3 pos = aPosition;
	pos.z *= -1.0;

	gl_Position = vec4(pos, 1);
	vTexCoords = aTexCoord;
}

#elif defined(FRAGMENT) ///////////////////////////////////////////////

layout(location = 0) uniform sampler2D uTexture;

in vec2 vTexCoords;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = texture(uTexture, vTexCoords);
}

#endif
#endif