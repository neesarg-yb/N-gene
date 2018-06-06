#pragma once

#include "Engine/Renderer/External/glcorearb.h"
#include "Engine/Renderer/External/glext.h"
#include "Engine/Renderer/External/wglext.h"

#define GL_CHECK_ERROR()  GLCheckError( __FILE__, __LINE__ )

bool GLCheckError( char const *file, int line );
bool GLFailed();
bool GLSucceeded();

extern PFNWGLGETEXTENSIONSSTRINGARBPROC		wglGetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC		wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC	wglCreateContextAttribsARB;
extern PFNGLCLEARPROC						glClear;
extern PFNGLCLEARCOLORPROC					glClearColor;

extern PFNGLLINEWIDTHPROC					glLineWidth; 
extern PFNGLENABLEPROC						glEnable;
extern PFNGLDISABLEPROC						glDisable;
extern PFNGLBLENDFUNCPROC					glBlendFunc;

extern PFNGLDELETESHADERPROC				glDeleteShader;
extern PFNGLCREATESHADERPROC				glCreateShader;
extern PFNGLSHADERSOURCEPROC				glShaderSource;
extern PFNGLCOMPILESHADERPROC				glCompileShader;
extern PFNGLGETSHADERIVPROC					glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
extern PFNGLCREATEPROGRAMPROC				glCreateProgram;
extern PFNGLATTACHSHADERPROC				glAttachShader;
extern PFNGLLINKPROGRAMPROC					glLinkProgram;
extern PFNGLGETPROGRAMIVPROC				glGetProgramiv;
extern PFNGLDELETEPROGRAMPROC				glDeleteProgram;
extern PFNGLDETACHSHADERPROC				glDetachShader;
extern PFNGLGETPROGRAMINFOLOGPROC			glGetProgramInfoLog;

extern PFNGLDELETEBUFFERSPROC				glDeleteBuffers;
extern PFNGLGENBUFFERSPROC					glGenBuffers;
extern PFNGLBINDBUFFERPROC					glBindBuffer;
extern PFNGLBUFFERDATAPROC					glBufferData;
extern PFNGLGENVERTEXARRAYSPROC				glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC				glBindVertexArray;
extern PFNGLUSEPROGRAMPROC					glUseProgram;

extern PFNGLGETATTRIBLOCATIONPROC			glGetAttribLocation;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC		glEnableVertexAttribArray;
extern PFNGLVERTEXATTRIBPOINTERPROC			glVertexAttribPointer;
extern PFNGLDRAWARRAYSPROC					glDrawArrays;
extern PFNGLDRAWELEMENTSPROC				glDrawElements;

extern PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv;
extern PFNGLUNIFORM1FPROC					glUniform1f;

extern PFNGLPIXELSTOREIPROC					glPixelStorei;
extern PFNGLGENTEXTURESPROC					glGenTextures;
extern PFNGLBINDTEXTUREPROC					glBindTexture;
extern PFNGLTEXIMAGE2DPROC					glTexImage2D;
extern PFNGLTEXPARAMETERIPROC				glTexParameteri;

extern PFNGLGENSAMPLERSPROC					glGenSamplers;
extern PFNGLDELETESAMPLERSPROC				glDeleteSamplers;
extern PFNGLSAMPLERPARAMETERIPROC			glSamplerParameteri;
extern PFNGLBINDSAMPLERPROC					glBindSampler;
extern PFNGLACTIVETEXTUREPROC				glActiveTexture;

extern PFNGLDEPTHFUNCPROC					glDepthFunc;
extern PFNGLDEPTHMASKPROC					glDepthMask;
extern PFNGLCLEARDEPTHFPROC					glClearDepthf;

extern PFNGLGETERRORPROC					glGetError;

extern PFNGLGENFRAMEBUFFERSPROC				glGenFramebuffers;
extern PFNGLDELETEFRAMEBUFFERSPROC			glDeleteFramebuffers;

extern PFNGLBINDFRAMEBUFFERPROC				glBindFramebuffer;
extern PFNGLFRAMEBUFFERTEXTUREPROC			glFramebufferTexture;
extern PFNGLDRAWBUFFERSPROC					glDrawBuffers;
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC		glCheckFramebufferStatus;
extern PFNGLBLITFRAMEBUFFERPROC				glBlitFramebuffer;

extern PFNGLPOLYGONMODEPROC					glPolygonMode;