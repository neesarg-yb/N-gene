#version 420 core

layout(binding = 0) uniform sampler2D gTexBloom;

in vec2 passUV;

out vec4 outColor; 

void main( void ) 
{
	vec4 b = texture( gTexBloom, passUV );

	outColor = b;
}