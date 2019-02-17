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

in vec4 COLOR; 
in vec2 UV;

out vec2  passUV; 
out vec4  passColor;
out vec3  passWorldPos;
out vec3  passWorldNormal;
out vec3  passWorldTangent;
out vec3  passWorldBitangent;
out float passCameraUsesShadows;

// Entry point - required.  What does this stage do?
void main( void )
{
   vec4 local_pos 	= vec4( POSITION, 1.0f );  
   vec4 world_pos 	= MODEL * local_pos ; 
   vec4 camera_pos 	= VIEW * world_pos; 
   vec4 clip_pos 	= PROJECTION * camera_pos; 

   passUV = UV; 
   passColor = COLOR; 

   passWorldPos = world_pos.xyz;  
   passWorldNormal = (MODEL * vec4( NORMAL, 0.0f )).xyz; 
   passWorldTangent = normalize( (vec4( TANGENT.xyz, 0.0f ) * MODEL).xyz ); 
   passWorldBitangent = normalize( cross( passWorldTangent, passWorldNormal ) * TANGENT.w ); 
   passCameraUsesShadows = USES_SHADOW;

   gl_Position = clip_pos;
}