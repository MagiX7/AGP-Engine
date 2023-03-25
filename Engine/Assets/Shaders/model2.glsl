#ifdef vertex
#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;

out vec2 vTexCoords;
out vec3 vNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec3 pos = aPosition;
	//pos.z *= -1.0;

	gl_Position = projection * view * model * vec4(pos, 1);
	vTexCoords = aTexCoord;
	vNormals = aNormals;
}

#ifdef fragment
#version 430 core

layout(location = 0) uniform sampler2D uTexture;

in vec2 vTexCoords;
in vec3 vNormals;

layout(location = 0) out vec4 fragColor;

void main()
{
	fragColor = vec4(1,0,0,1);
	//fragColor = vec4(vTexCoords, 0, 1);
	fragColor = texture(uTexture, vTexCoords);
}
