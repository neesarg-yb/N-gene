// Renderer/shaderprogram.h
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/glfunctions.hpp"

class ShaderProgram
{
public:
	 ShaderProgram() {};
	~ShaderProgram() {};

	bool LoadFromFiles( char const *root ); // load a shader from file
	bool LoadFromStrings( const char* vs_program, const char* fs_program );
	static GLuint LoadShaderFromFile( char const *filename, GLenum type );
	static GLuint LoadShaderFromString( const char* shader_program, GLenum type );
	static void LogShaderError(GLuint shader_id);
	static GLuint CreateAndLinkProgram( GLint vs, GLint fs );
	static void LogProgramError(GLuint program_id);

	GLuint GetHandle() const;

	static const char* GetDefaultVertexShaderSource();
	static const char* GetDefaultFragmentShaderSource();

public:
	GLuint program_handle; // OpenGL handle for this program, default 0

	static const char* m_default_vsp;
	static const char* m_default_fsp;
};