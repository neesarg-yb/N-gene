#pragma once
#include "Engine/Renderer/glfunctions.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

//------------------------------------------------------------------------
bool GLCheckError( char const *file, int line )
{
#if defined(_DEBUG)
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		DebuggerPrintf( "GL ERROR [0x%04x] at [%s(%i)]", error, file, line );
		return true; 
	}
#endif
	return false; 
}

bool GLFailed()
{
	return GL_CHECK_ERROR(); 
}

bool GLSucceeded()
{
	return !GLFailed();
}

//------------------------------------------------------------------------
PFNWGLGETEXTENSIONSSTRINGARBPROC		wglGetExtensionsStringARB	= nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC			wglChoosePixelFormatARB		= nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC		wglCreateContextAttribsARB	= nullptr;
PFNGLCLEARPROC							glClear						= nullptr;
PFNGLCLEARCOLORPROC						glClearColor				= nullptr;

PFNGLLINEWIDTHPROC						glLineWidth					= nullptr; 
PFNGLENABLEPROC							glEnable					= nullptr;
PFNGLDISABLEPROC						glDisable					= nullptr;
PFNGLBLENDFUNCPROC						glBlendFunc					= nullptr;

PFNGLDELETESHADERPROC					glDeleteShader				= nullptr; 
PFNGLCREATESHADERPROC					glCreateShader				= nullptr; 
PFNGLSHADERSOURCEPROC					glShaderSource				= nullptr; 
PFNGLCOMPILESHADERPROC					glCompileShader				= nullptr; 
PFNGLGETSHADERIVPROC					glGetShaderiv				= nullptr; 
PFNGLGETSHADERINFOLOGPROC				glGetShaderInfoLog			= nullptr; 
PFNGLCREATEPROGRAMPROC					glCreateProgram				= nullptr; 
PFNGLATTACHSHADERPROC					glAttachShader				= nullptr; 
PFNGLLINKPROGRAMPROC					glLinkProgram				= nullptr; 
PFNGLGETPROGRAMIVPROC					glGetProgramiv				= nullptr; 
PFNGLDELETEPROGRAMPROC					glDeleteProgram				= nullptr; 
PFNGLDETACHSHADERPROC					glDetachShader				= nullptr; 
PFNGLGETPROGRAMINFOLOGPROC				glGetProgramInfoLog			= nullptr; 

PFNGLDELETEBUFFERSPROC					glDeleteBuffers				= nullptr;
PFNGLGENBUFFERSPROC						glGenBuffers				= nullptr;
PFNGLBINDBUFFERPROC						glBindBuffer				= nullptr;
PFNGLBUFFERDATAPROC						glBufferData				= nullptr;
PFNGLGENVERTEXARRAYSPROC				glGenVertexArrays			= nullptr;
PFNGLBINDVERTEXARRAYPROC				glBindVertexArray			= nullptr;
PFNGLUSEPROGRAMPROC						glUseProgram				= nullptr;

PFNGLGETATTRIBLOCATIONPROC				glGetAttribLocation			= nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC		glEnableVertexAttribArray	= nullptr;
PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer		= nullptr;
PFNGLDRAWARRAYSPROC						glDrawArrays				= nullptr;
PFNGLDRAWELEMENTSPROC					glDrawElements				= nullptr;

PFNGLGETUNIFORMLOCATIONPROC				glGetUniformLocation		= nullptr;
PFNGLUNIFORMMATRIX4FVPROC				glUniformMatrix4fv			= nullptr;
PFNGLUNIFORM1FPROC						glUniform1f					= nullptr;

PFNGLPIXELSTOREIPROC					glPixelStorei				= nullptr;
PFNGLGENTEXTURESPROC					glGenTextures				= nullptr;
PFNGLBINDTEXTUREPROC					glBindTexture				= nullptr;
PFNGLTEXIMAGE2DPROC						glTexImage2D				= nullptr;
PFNGLTEXPARAMETERIPROC					glTexParameteri				= nullptr;

PFNGLGENSAMPLERSPROC					glGenSamplers				= nullptr;
PFNGLDELETESAMPLERSPROC					glDeleteSamplers			= nullptr;
PFNGLSAMPLERPARAMETERIPROC				glSamplerParameteri			= nullptr;
PFNGLBINDSAMPLERPROC					glBindSampler				= nullptr;
PFNGLACTIVETEXTUREPROC					glActiveTexture				= nullptr;

PFNGLDEPTHFUNCPROC						glDepthFunc					= nullptr;
PFNGLDEPTHMASKPROC						glDepthMask					= nullptr;
PFNGLCLEARDEPTHFPROC					glClearDepthf				= nullptr;

PFNGLGETERRORPROC						glGetError					= nullptr;

PFNGLGENFRAMEBUFFERSPROC				glGenFramebuffers			= nullptr;
PFNGLDELETEFRAMEBUFFERSPROC				glDeleteFramebuffers		= nullptr;

PFNGLBINDFRAMEBUFFERPROC				glBindFramebuffer			= nullptr;
PFNGLFRAMEBUFFERTEXTUREPROC				glFramebufferTexture		= nullptr;
PFNGLDRAWBUFFERSPROC					glDrawBuffers				= nullptr;
PFNGLCHECKFRAMEBUFFERSTATUSPROC			glCheckFramebufferStatus	= nullptr;
PFNGLBLITFRAMEBUFFERPROC				glBlitFramebuffer			= nullptr;

PFNGLPOLYGONMODEPROC					glPolygonMode				= nullptr;
PFNGLCULLFACEPROC						glCullFace					= nullptr;
PFNGLFRONTFACEPROC						glFrontFace					= nullptr;
PFNGLBLENDEQUATIONSEPARATEPROC			glBlendEquationSeparate		= nullptr;
PFNGLBLENDFUNCSEPARATEPROC				glBlendFuncSeparate			= nullptr;

PFNGLUNIFORM1UIVPROC					glUniform1uiv				= nullptr;
PFNGLUNIFORM1FVPROC						glUniform1fv				= nullptr;
PFNGLUNIFORM2FVPROC						glUniform2fv				= nullptr;
PFNGLUNIFORM3FVPROC						glUniform3fv				= nullptr;
PFNGLUNIFORM4FVPROC						glUniform4fv				= nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC		glDisableVertexAttribArray	= nullptr;
PFNGLGETINTEGERVPROC					glGetIntegerv				= nullptr;
PFNGLBINDBUFFERBASEPROC					glBindBufferBase			= nullptr;

PFNGLDELETETEXTURESPROC					glDeleteTextures			= nullptr;
PFNGLTEXSTORAGE2DPROC					glTexStorage2D				= nullptr;
PFNGLTEXSUBIMAGE2DPROC					glTexSubImage2D				= nullptr;
PFNGLREADPIXELSPROC						glReadPixels				= nullptr;

PFNGLVIEWPORTPROC						glViewport					= nullptr;