// define the shader version (this is required)
#version 420 core

// Attributes - input to this shasder stage (constant as far as the code is concerned)

// Create a "uniform", a variable that is constant/uniform 
// for the entirety of the draw call.  This one will
// store our projection, or "ViewToClip" matrix.
layout( binding = 2, std140 ) uniform uboCamera
{
   vec3    padding_04;
   float   USES_SHADOW;

   mat4    VIEW;
   mat4    PROJECTION;
};

uniform mat4 MODEL;

in vec3 POSITION;
in vec4 COLOR;       // GLSL will use a Vector4 for this; 
in vec2 UV;

out vec2 v_passUV; 
out vec4 v_passColor;
out vec4 v_worldPosition;

void main( void )
{
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 world_pos = MODEL * local_pos;
   vec4 clip_pos  = PROJECTION * VIEW * world_pos; 
   
   gl_Position       = clip_pos;
   v_passColor       = COLOR;
   v_passUV          = UV;
   v_worldPosition   = world_pos;
}