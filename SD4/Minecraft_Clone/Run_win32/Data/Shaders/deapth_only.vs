#version 420 core

layout( binding = 2, std140 ) uniform uboCamera
{
   vec3    padding_04;
   float   USES_SHADOW;

   mat4    VIEW;
   mat4    PROJECTION;
};

uniform mat4 MODEL;

in vec3 POSITION;
in vec3 NORMAL; 
in vec4 TANGENT;

// Entry point - required.  What does this stage do?
void main( void )
{
   vec4 local_pos 	= vec4( POSITION, 1.0f );  
   vec4 world_pos 	= MODEL * local_pos ; 
   vec4 camera_pos 	= VIEW * world_pos; 
   vec4 clip_pos 	= PROJECTION * camera_pos; 
   
   gl_Position = clip_pos;
}