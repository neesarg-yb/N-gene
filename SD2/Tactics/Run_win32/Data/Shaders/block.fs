#version 420 core

// Suggest always manually setting bindings - again, consitancy with 
// other rendering APIs and well as you can make assumptions in your
// engine without having to query
layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexBorder;


in vec2 passUV; 
in vec2 passBorderUV; 
in vec4 passColor; 

out vec4 outColor; 

void main( void )
{
   vec4 tex_color = texture( gTexDiffuse, passUV ); 
   vec4 border_color = texture( gTexBorder, passBorderUV ); 

   // alpha blend in the border
   vec3 color = mix( tex_color.xyz, border_color.xyz, border_color.w ); 

   outColor = vec4( color, tex_color.w ) * passColor;
}
