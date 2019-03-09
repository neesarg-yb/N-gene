#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D u_gTexDiffuse;

uniform float u_lightningStength	 = 0.f;
uniform float u_glowStrength		 = 1.f;
uniform float u_daylightFraction	 = 1.f;
uniform vec3  u_indoorLightRgb 		 = vec3( 0.00, 0.00, 0.00 );
uniform vec3  u_outdoorLightRgb 	 = vec3( 0.00, 0.00, 0.00 );
uniform vec3  u_skyColor			 = vec3( 0.00, 0.00, 0.00 );
uniform vec3  u_cameraPosition		 = vec3( 0.00, 0.00, 0.00 );
uniform float u_fogNearDistance		 = 100.0;
uniform float u_fogFarDistance		 = 200.0;

in vec2 v_passUV;
in vec4 v_passColor;
in vec3 v_worldPosition;

// Outputs
out vec4 f_outColor; 

// Entry Point
void main( void )
{
   	vec4 diffuse = texture( u_gTexDiffuse, v_passUV ); 

	float indoorLightLevel 	= v_passColor.r;			// Red   Channel == Indoor  Light Level
	float outdoorLightLevel	= v_passColor.g;			// Green Channel == Outdoor Light Level

	// Calculate shaded telex Rgb
	vec3 indoorLightRgb 	= indoorLightLevel  * u_indoorLightRgb * u_glowStrength;
	vec3 outdoorLightRgb 	= outdoorLightLevel * u_outdoorLightRgb * u_daylightFraction;
	outdoorLightRgb 		= mix( outdoorLightRgb, vec3(1.f, 1.f, 1.f), u_lightningStength );
	vec3 lightRgb 			= max( indoorLightRgb, outdoorLightRgb );
	vec4 shadedTexel 		= diffuse * vec4( lightRgb, v_passColor.a );

	// Apply Fog
	float dist 			= distance( u_cameraPosition, v_worldPosition );
	float fogFraction 	= (dist - u_fogNearDistance) / (u_fogFarDistance - u_fogNearDistance);
	fogFraction 		= clamp( fogFraction, 0.0, 1.0 );
	vec3 finalColor 	= mix( shadedTexel.rgb, u_skyColor, fogFraction );
	
   	f_outColor = vec4( finalColor, shadedTexel.a );
}
