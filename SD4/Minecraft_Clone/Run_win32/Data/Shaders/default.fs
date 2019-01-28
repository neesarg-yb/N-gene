#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1, std140) uniform uboTimeClock
{
	float GAME_DELTA_TIME;
	float GAME_TOTAL_TIME;
	float SYSTEM_DELTA_TIME;
	float SYSTEM_TOTAL_TIME;
};

in vec4 passColor;
in vec2 passUV;

// Outputs
out vec4 outColor; 

// Entry Point
void main( void )
{
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV ); 
   
   // multiply is component-wise
   // so this gets (diff.x * passColor.x, ..., diff.w * passColor.w)
   vec4 color = diffuse * passColor; 

   outColor = color;
}