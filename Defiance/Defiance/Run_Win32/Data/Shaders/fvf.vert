#version 410 core

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

uniform vec3 uWBufferParams;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;

out vec4 passColor;
out vec2 passUV0;

void main( void )
{
	passColor = inColor;
	passUV0 = inUV0;

	vec4 pos = vec4(inPosition, 1.0);
	vec4 clipSpacePos = uProj * uView * uModel * pos;

	// Mapping depth
	vec2 bufferParams = uWBufferParams.xy;
	float linearZ = dot(bufferParams, vec2(clipSpacePos.w, 1.0f));
	clipSpacePos.z = linearZ * clipSpacePos.w;	

	gl_Position = clipSpacePos;
}