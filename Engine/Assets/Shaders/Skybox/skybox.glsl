///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
#ifdef SKYBOX

#if defined(VERTEX) ///////////////////////////////////////////////////


layout(location = 0) in vec3 position;

out vec3 localPosition;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	mat4 rotView = mat4(mat3(view)); // Remove translation
	vec4 clipPos = projection * rotView * vec4(position, 1.0);
	gl_Position = clipPos.xyww;
	
	localPosition = position;
}


#elif defined(FRAGMENT) ///////////////////////////////////////////////


in vec3 localPosition;
out vec4 fragColor;

layout(location = 0) uniform samplerCube skybox;

const float PI = 3.14159265359;

void main()
{
	vec3 color = texture(skybox, localPosition).rgb;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	fragColor = vec4(color, 1);
}

#endif
#endif