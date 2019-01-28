#version 420 core

out vec2 passUV;

// POSITIONS & UVS are way big because we're covering the whole NDC quad with just one Triangle
const vec3 POSITIONS[4] = vec3[] (
	vec3( -1.f, -1.f, 0.f ),
	vec3(  1.f, -1.f, 0.f ),
	vec3(  1.f,  1.f, 0.f ),
	vec3( -1.f,  1.f, 0.f )
	);
const vec2 UVS[4] = vec2[] (
	vec2( 0.f, 0.f ),
	vec2( 1.f, 0.f ),
	vec2( 1.f, 1.f ),
	vec2( 0.f, 1.f )
	);

void main( void )
{
	int idx 	= gl_VertexID;
	passUV 		= UVS[idx]; 
	gl_Position = vec4( POSITIONS[idx], 1.f );	// we pass out a clip coordinate
}