#version 420 core

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
uniform float PASSEDFLOAT;

out vec2 passUV; 
out vec4 passColor;
out float passTime;

void main( void )
{
   passColor = COLOR; 
   passUV = UV;

   gl_Position = vec4( POSITION, 1 );
   passTime = PASSEDFLOAT;
}