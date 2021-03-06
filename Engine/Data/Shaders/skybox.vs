#version 420 core

// Attributes
in vec3 POSITION;

out vec3 passWorldPosition;

layout( binding = 2, std140 ) uniform uboCamera
{
   vec3    padding_04;
   float   USES_SHADOW;

   mat4    VIEW;
   mat4    PROJECTION;
};

uniform mat4 MODEL;


void main( void )
{
   // 1, since I don't want to translate
   vec4 local_pos    = vec4( POSITION, 0.0f );	

   vec4 world_pos    = MODEL * local_pos; // assume local is world for now; 
   passWorldPosition = world_pos.xyz;

   vec4 camera_pos   = VIEW * local_pos; 

   // projection relies on a 1 being present, so add it back
   vec4 clip_pos     = PROJECTION * vec4(camera_pos.xyz, 1); 

   // we only render where depth is 1.0 (equal, ie, where have we not drawn)
   // so z needs to be one for all these
   clip_pos.z = clip_pos.w; // z must equal w.  We set the z, not the w, so we don't affect the x and y as well

	gl_Position = clip_pos; // we pass out a clip coordinate
}
