#version 410 core

uniform sampler2D uTexDiffuse;

in vec2 passUV0;

out vec4 outColor;

void main( void )
{
	vec4 diffuse = texture(uTexDiffuse, passUV0);
	outColor = diffuse;
}