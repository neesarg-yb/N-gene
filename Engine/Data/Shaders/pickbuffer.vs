#version 420 core

layout( binding = 2, std140 ) uniform uboCamera
{
   vec3    padding_04;
   float   USES_SHADOW;

   mat4    VIEW;
   mat4    PROJECTION;
};

uniform mat4 MODEL;
uniform vec4 PICK_ID;

in vec3 POSITION;
out vec4 passPickID;

void main( void )
{
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos  = PROJECTION * VIEW * MODEL * local_pos; 
   passPickID     = PICK_ID;

   gl_Position    = clip_pos;
}