#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D u_gTexDiffuse;

uniform vec3  u_indoorLightRgb 	= vec3( 0.00, 0.00, 0.00 );
uniform vec3  u_outdoorLightRgb = vec3( 0.00, 0.00, 0.00 );
uniform vec3  u_skyColor		= vec3( 0.00, 0.00, 0.00 );
uniform vec3  u_cameraPosition	= vec3( 0.00, 0.00, 0.00 );
uniform float u_fogNearDistance	= 100.0;
uniform float u_fogFarDistance	= 200.0;

in vec2 v_passUV;
in vec4 v_passColor;
in vec4 v_worldPosition;

// Outputs
out vec4 f_outColor; 

// Entry Point
void main( void )
{
   	vec4 diffuse = texture( u_gTexDiffuse, v_passUV ); 
  	
	float indoorLightLevel 	= v_passColor.r;			// Red   Channel == Indoor  Light Level
	float outdoorLightLevel	= v_passColor.g;			// Green Channel == Outdoor Light Level

	// Calculate final Rgb for the telex
	vec3 indoorLightRgb 	= indoorLightLevel  * u_indoorLightRgb;
	vec3 outdoorLightRgb 	= outdoorLightLevel * u_outdoorLightRgb;
	vec3 lightRgb 			= max( indoorLightRgb, outdoorLightRgb );
	vec4 shadedTexel 		= diffuse * vec4( lightRgb, v_passColor.a );
	
   	f_outColor = shadedTexel;
}