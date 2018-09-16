#version 420 core

layout(binding = 0) uniform sampler2D gTexBloom;

in vec2 passUV;

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

// Outputs
out vec4 outColor; 

// Entry Point
void main( void )
{   
	// Blur Effect
	vec2 tex_offset = 1.0 / textureSize(gTexBloom, 0); // gets size of single texel
   vec3 result = texture(gTexBloom, passUV).rgb * weight[0]; // current fragment's contribution
   
   // Horizontal Pass
   for(int i = 1; i < 5; ++i)
   {
       result += texture(gTexBloom, passUV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
       result += texture(gTexBloom, passUV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
   }
   
   // Verticle Pass
   for(int i = 1; i < 5; ++i)
   {
       result += texture(gTexBloom, passUV + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
       result += texture(gTexBloom, passUV - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
   }

   outColor = vec4(result, 1.0);
}