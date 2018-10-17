#version 420 core

in vec4 passPickID;
out vec4 outColor;

void main( void )
{
   outColor = passPickID;
}