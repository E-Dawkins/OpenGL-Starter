#version 410 core

uniform mat4 model;
uniform mat4 viewProjection;

layout (location = 0) in vec3 iPos;
layout (location = 1) in vec2 iUv;

out vec2 Uv;

void main()
{
   gl_Position = viewProjection * model * vec4(iPos, 1.0);
	Uv = iUv;
}