#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 texCords;
layout (location = 3) in vec3 inColor;

uniform mat4 pv;
uniform mat4 m;

void main()
{
	vec4 vertPos = m*vec4(aPos, 1.0f);
	gl_Position = pv*m*vec4(aPos, 1.0f);
	
}