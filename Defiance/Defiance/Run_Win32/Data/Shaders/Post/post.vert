#version 410 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

in vec3 inPosition;
in vec2 inUV0;

out vec2 passUV0;

void main( void )
{
	passUV0 = vec2(inUV0.x, 1 - inUV0.y);

	vec4 pos = vec4(inPosition, 1.0f);

	// col major
	//pos = pos * uModel * uView * uProj;

	// row major 
	pos = uProj * uView * uModel * pos;

	gl_Position = pos;
}