#version 410 core

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 Uv;

out vec4 FragColor;

void main()
{
   vec4 tex0 = texture(texture0, Uv);
   vec4 tex1 = texture(texture1, Uv);
   FragColor = mix(tex0, tex1, 0.5);
}