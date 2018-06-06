// define the shader version (this is required)
#version 420 core

// Attributes - input to this shasder stage (constant as far as the code is concerned)

// Create a "uniform", a variable that is constant/uniform 
// for the entirety of the draw call.  This one will
// store our projection, or "ViewToClip" matrix.
uniform mat4 MODEL;
uniform mat4 PROJECTION;
uniform mat4 VIEW;

in vec3 POSITION;
in vec4 COLOR;       // GLSL will use a Vector4 for this; 
in vec2 UV;

// NEW: pass variables or "varying" variables
// These are variables output from this stage for use by the next stage
// These are called "varying" because for a triangle, we'll only output 3, and 
// the Raster stage may output hundreds of pixels, each a blended value
// of the initial 3, hence - they "vary"
out vec2 passUV; 
out vec4 passColor;  // to use it in the pixel stage, we must pass it.

// Entry point - required.  What does this stage do?
void main( void )
{
   // multiply it through - for now, local_pos
   // is being treated as view space position
   // this will be updated later once we introduce the other matrices
   vec4 local_pos = vec4( POSITION, 1 ); 
   vec4 clip_pos = PROJECTION * VIEW * MODEL * local_pos; 
   
   passColor = COLOR; // pass it on. 
   passUV = UV;
   gl_Position = clip_pos;
}