#version 420 core

// Attributes
in vec3 POSITION;
in vec4 COLOR;
in vec2 UV; 

out vec2 passUV; 
out vec2 passBorderUV; 
out vec4 passColor; 

uniform mat4 MODEL;
uniform mat4 PROJECTION;
uniform mat4 VIEW;

vec2 BORDER_UVS[] = { vec2(0.0), vec2(1.0f, 0.0f), vec2(1.0f, 1.0f), vec2(0.0f, 1.0f) };


void main( void )
{
   vec4 local_pos = vec4( POSITION, 1.0f );	
   vec4 clip_pos = PROJECTION * VIEW * MODEL * local_pos; 

   passUV = UV; 
   passBorderUV = BORDER_UVS[gl_VertexID % 4]; 
   passColor = COLOR; 
   gl_Position = clip_pos; // we pass out a clip coordinate
}
