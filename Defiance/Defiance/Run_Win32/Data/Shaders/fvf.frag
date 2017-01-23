#version 410 core

uniform sampler2D uTexDiffuse;
uniform vec4 uColor;

in vec4 passColor;
in vec2 passUV0;

out vec4 outColor;

void main( void )
{
	vec4 diffuse = texture(uTexDiffuse, passUV0);
	outColor = uColor * passColor * diffuse;
}