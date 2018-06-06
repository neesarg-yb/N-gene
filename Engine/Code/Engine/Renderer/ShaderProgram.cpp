#pragma once
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/File/File.hpp"

const char* ShaderProgram::m_default_fsp = R"(
#version 420 core

// create a uniform for our sampler2D.
// layout binding = 0 is us binding it to texture slot 0.  
layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec4 passColor;
in vec2 passUV;

// Outputs
out vec4 outColor; 

// Entry Point
void main( void )
{
   // sample (gather) our texel colour for this UV
   vec4 diffuse = texture( gTexDiffuse, passUV ); 
   
   // multiply is component-wise
   // so this gets (diff.x * passColor.x, ..., diff.w * passColor.w)
   outColor = diffuse * passColor; 
}
)";

const char* ShaderProgram::m_default_vsp = R"(
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
)";

bool ShaderProgram::LoadFromFiles( char const *root )
{
	std::string vs_file = root;
	vs_file += ".vs"; 

	std::string fs_file = root; 
	fs_file += ".fs";  

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShaderFromFile( vs_file.c_str(), GL_VERTEX_SHADER ); 
	GLuint frag_shader = LoadShaderFromFile( fs_file.c_str(), GL_FRAGMENT_SHADER ); 

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram( vert_shader, frag_shader ); 
	glDeleteShader( vert_shader ); 
	glDeleteShader( frag_shader ); 

	return (program_handle != NULL); 
};

bool ShaderProgram::LoadFromFiles( std::string vertexShaderPath, std::string fragmentShaderPath )
{
	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShaderFromFile( vertexShaderPath.c_str(), GL_VERTEX_SHADER );
	GLuint frag_shader = LoadShaderFromFile( fragmentShaderPath.c_str(), GL_FRAGMENT_SHADER );

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram( vert_shader, frag_shader );
	glDeleteShader( vert_shader );
	glDeleteShader( frag_shader );

	return (program_handle != NULL);
}

bool ShaderProgram::LoadFromStrings( const char* vs_program, const char* fs_program )
{

	// Compile the two stages we're using (all shaders will implement the vertex and fragment stages)
	// later on, we can add in more stages;
	GLuint vert_shader = LoadShaderFromString( vs_program, GL_VERTEX_SHADER ); 
	GLuint frag_shader = LoadShaderFromString( fs_program, GL_FRAGMENT_SHADER ); 

	// Link the program
	// program_handle is a member GLuint. 
	program_handle = CreateAndLinkProgram( vert_shader, frag_shader ); 
	glDeleteShader( vert_shader ); 
	glDeleteShader( frag_shader ); 

	return (program_handle != NULL); 
}

GLuint ShaderProgram::LoadShaderFromFile( char const *filename, GLenum type )
{
	char *src = (char*)FileReadToNewBuffer(filename);
	GUARANTEE_RECOVERABLE(src != nullptr, "LoadShader: FAILED TO LOAD A FILE!");


	// Create a shader
	GLuint shader_id = glCreateShader(type);
	GUARANTEE_RECOVERABLE(shader_id != NULL, "shader_id == NULL");

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(src);
	glShaderSource(shader_id, 1, &src, &shader_length);
	glCompileShader(shader_id);

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}

	free(src);

	return shader_id;
}

GLuint ShaderProgram::LoadShaderFromString( const char* shader_program, GLenum type )
{
	const char *src = shader_program;
	GUARANTEE_RECOVERABLE(src != nullptr, "LoadShader: FAILED TO LOAD FROM STRING!");


	// Create a shader
	GLuint shader_id = glCreateShader(type);
	GUARANTEE_RECOVERABLE(shader_id != NULL, "shader_id == NULL");

	// Bind source to it, and compile
	// You can add multiple strings to a shader – they will 
	// be concatenated together to form the actual source object.
	GLint shader_length = (GLint)strlen(src);
	glShaderSource(shader_id, 1, &src, &shader_length);
	glCompileShader(shader_id);

	// Check status
	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		LogShaderError(shader_id); // function we write
		glDeleteShader(shader_id);
		shader_id = NULL;
	}

	return shader_id;
}

void ShaderProgram::LogShaderError(GLuint shader_id)
{
	// figure out how large the buffer needs to be
	GLint length;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

	// make a buffer, and copy the log to it. 
	char *buffer = new char[length + 1];
	glGetShaderInfoLog(shader_id, length, &length, buffer);

	// Print it out (may want to do some additional formatting)
	buffer[length] = NULL;
	DebuggerPrintf("\n\nLogShaderError: \n");
	DebuggerPrintf( buffer );
	GUARANTEE_RECOVERABLE( false, "LogShaderError: See console output..\n" + std::string(buffer) );

	// free up the memory we used. 
	delete buffer;
}

GLuint ShaderProgram::CreateAndLinkProgram( GLint vs, GLint fs )
{
	// create the program handle - how you will reference
	// this program within OpenGL, like a texture handle
	GLuint program_id = glCreateProgram();
	GUARANTEE_RECOVERABLE( program_id != 0, "program_id == 0" );

	// Attach the shaders you want to use
	glAttachShader( program_id, vs );
	glAttachShader( program_id, fs );

	// Link the program (create the GPU program)
	glLinkProgram( program_id );

	// Check for link errors - usually a result
	// of incompatibility between stages.
	GLint link_status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	if (link_status == GL_FALSE) {
		LogProgramError(program_id);
		glDeleteProgram(program_id);
		program_id = 0;
	} 

	// no longer need the shaders, you can detach them if you want
	// (not necessary)
	glDetachShader( program_id, vs );
	glDetachShader( program_id, fs );

	return program_id;
}

void ShaderProgram::LogProgramError(GLuint program_id)
{
	// get the buffer length
	GLint length;
	glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

	// copy the log into a new buffer
	char *buffer = new char[length + 1];
	glGetProgramInfoLog(program_id, length, &length, buffer);

	// print it to the output pane
	buffer[length] = NULL;
	DebuggerPrintf("\n\nLogProgramError: \n");
	DebuggerPrintf( buffer );
	GUARANTEE_RECOVERABLE( false, "LogProgramError: See console output.." );

	// cleanup
	delete buffer;
}

GLuint ShaderProgram::GetHandle() const
{
	return program_handle;
}

const char* ShaderProgram::GetDefaultVertexShaderSource()
{
	return m_default_vsp;
}

const char* ShaderProgram::GetDefaultFragmentShaderSource()
{
	return m_default_fsp;
}