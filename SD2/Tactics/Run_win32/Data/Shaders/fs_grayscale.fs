#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor;
in vec2 passUV;
in float passTime;

// Outputs
out vec4 outColor; 

// Entry Point
void main( void )
{
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV ); 
   
   // multiply is component-wise
   // so this gets (diff.x * passColor.x, ..., diff.w * passColor.w)
   vec4 afterTintColor = diffuse * passColor;
   float grayScaleFraction = (afterTintColor.r + afterTintColor.g + afterTintColor.b ) / 3;
   vec4 grayScaleColor = vec4( grayScaleFraction, grayScaleFraction, grayScaleFraction, afterTintColor.a );
   outColor = grayScaleColor * sin(passTime); 
}