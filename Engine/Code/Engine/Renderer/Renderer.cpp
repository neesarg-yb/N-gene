#pragma once

#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#include "Engine/Renderer/glfunctions.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Profiler/Profiler.hpp"

Renderer*	Renderer::s_renderer				= nullptr;

GLuint		Renderer::s_default_vao				= NULL;
Sampler*	Renderer::s_defaultNearestSampler	= nullptr;
Sampler*	Renderer::s_defaultLinearSampler	= nullptr;
Texture*	Renderer::s_defaultColorTarget		= nullptr;
Texture*	Renderer::s_defaultDepthTarget		= nullptr;
Camera*		Renderer::s_default_camera			= nullptr;
Camera*		Renderer::s_current_camera			= nullptr;

Texture*	Renderer::s_effectCurrentSource		= nullptr;
Texture*	Renderer::s_effectCurrentTarget		= nullptr;
Texture*	Renderer::s_sketchColorTarget		= nullptr;
Camera*		Renderer::s_effectsCamera			= nullptr;

unsigned int Renderer::s_maxConstantBufferBindings;
unsigned int Renderer::s_maxConstantBufferSize;

// Four needed variables.  Globals or private members of Renderer are fine; 
static HMODULE gGLLibrary  = NULL; 
static HWND gGLwnd         = NULL;    // window our context is attached to; 
static HDC gHDC            = NULL;    // our device context
static HGLRC gGLContext    = NULL;    // our rendering context; 

static HGLRC CreateOldRenderContext( HDC ) ;
static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor );
void BindNewWGLFunctions();
void BindGLFunctions();

// Rendering startup - called after we have created our window
// error checking has been removed for brevity, but you should check
// the return values at each step.
bool Renderer::RendererStartup() 
{
	HWND hwnd = static_cast<HWND>( Window::GetInstance()->m_hwnd );

	// load and get a handle to the opengl dll (dynamic link library)
	gGLLibrary = ::LoadLibraryA( "opengl32.dll" ); 

	// Get the Device Context (DC) - how windows handles the interface to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HDC hdc = ::GetDC( hwnd );       

	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very simiilar to SD1
	HGLRC temp_context = CreateOldRenderContext( hdc ); 

	::wglMakeCurrent( hdc, temp_context ); 
	BindNewWGLFunctions();  // find the functions we'll need to create the real context; 

	// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext( hdc, 4, 2 ); 

	// Set and cleanup
	::wglMakeCurrent( hdc, real_context ); 
	::wglDeleteContext( temp_context ); 

	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions(); 

	// set the globals
	gGLwnd = hwnd;
	gHDC = hdc; 
	gGLContext = real_context; 

	// For CubeMap
	glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

	PostStartup();

	return true; 
}

void Renderer::RendererShutdown()
{
	ReleaseDC( gGLwnd, gHDC );

	delete s_defaultNearestSampler;
	delete s_defaultLinearSampler;
}

void Renderer::GLShutdown()
{
	wglMakeCurrent( gHDC, NULL ); 

	::wglDeleteContext( gGLContext ); 
	::ReleaseDC( gGLwnd, gHDC ); 

	gGLContext = NULL; 
	gHDC = NULL;
	gGLwnd = NULL; 

	::FreeLibrary( gGLLibrary ); 
}

void Renderer::PostStartup()
{
	wglSwapIntervalEXT( 0 );

	// default_vao is a GLuint member variable
	glGenVertexArrays( 1, &s_default_vao ); 
	glBindVertexArray( s_default_vao ); 

	s_defaultNearestSampler = new Sampler( SAMPLER_NEAREST );
	s_defaultLinearSampler	= new Sampler( SAMPLER_LINEAR  );

	// the default color and depth should match our output window
	// so get width/height however you need to.
	unsigned int window_width	=	Window::GetInstance()->GetWidth(); 
	unsigned int window_height	=	Window::GetInstance()->GetHeight();

	// create our output textures
	s_defaultColorTarget = CreateRenderTarget( window_width, window_height );
	s_defaultDepthTarget = CreateRenderTarget( window_width, window_height, TEXTURE_FORMAT_D24S8 );

	// setup the default camera
	s_default_camera = new Camera();
	s_default_camera->SetColorTarget( s_defaultColorTarget );
	s_default_camera->SetDepthStencilTarget( s_defaultDepthTarget );
	s_default_camera->SetProjectionOrtho( 5.0f, 0.0f, 100.0f );  
	s_default_camera->LookAt( Vector3( 3.0f, 3.0f, -10.0f ), Vector3::ZERO ); 

	// create sketch color textures
	s_sketchColorTarget  = CreateRenderTarget( window_width, window_height );

	// setup the effect camera
	s_effectsCamera = new Camera();
	s_effectsCamera->SetColorTarget( s_defaultColorTarget );
	s_effectsCamera->SetDepthStencilTarget( s_defaultDepthTarget );

	// Checking how many UBOs I can bound..
	GLint ubo_vs, ubo_fs, ubo_gs;
	glGetIntegerv( GL_MAX_VERTEX_UNIFORM_BLOCKS, &ubo_vs );
	glGetIntegerv( GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &ubo_fs );
	glGetIntegerv( GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &ubo_gs );
	GLint minBlocks = ubo_vs	< ubo_fs ? ubo_vs : ubo_fs;
	minBlocks		= minBlocks < ubo_gs ? minBlocks : ubo_gs;
	s_maxConstantBufferBindings = (unsigned int) minBlocks;

	GLint uboSize;
	glGetIntegerv( GL_MAX_UNIFORM_BLOCK_SIZE, &uboSize );
	s_maxConstantBufferSize = (unsigned int) uboSize;

	GL_CHECK_ERROR(); 
}

bool Renderer::CopyFrameBuffer( FrameBuffer *dst, FrameBuffer *src )
{
	// we need at least the src.
	if (src == nullptr) {
		return false; 
	}

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint src_fbo = src->GetHandle();
	GLuint dst_fbo = NULL; 
	if (dst != nullptr) {
		dst_fbo = dst->GetHandle(); 
	}

	// can't copy onto ourselves
	if (dst_fbo == src_fbo) {
		return false; 
	}

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer( GL_READ_FRAMEBUFFER, src_fbo ); 

	// what are we copying to?
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dst_fbo ); 

	// blit it over - get the size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not
	unsigned int width	= src->GetWidth();     
	unsigned int height = src->GetHeight(); 

	// Copy it over
	glBlitFramebuffer( 0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
		GL_NEAREST );         // resize filtering rule (in case src/dst don't match)

							  // Make sure it succeeded
	GL_CHECK_ERROR(); 

	// cleanup after ourselves
	glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL ); 

	return GLSucceeded();
}

void Renderer::LoadAllInbuiltShaders()
{
	// Load the default shader
	
	m_defaultShader = CreateOrGetShader( "default" );
	m_currentShader = m_defaultShader;
}

Shader* Renderer::CreateOrGetShader( const char* shaderfileName )
{
	// check for the name in pool
		// if found, return its pointer
		// else create a new one, add to pool and return appropriate pointer
			// newShader in success & defaultShader on failure
	std::string name = shaderfileName + std::string(".shader");

	Shader* toReturn = nullptr;
	bool foundShaderProgram = FindShaderFromPool( name, toReturn );

	if( foundShaderProgram )
	{
		return toReturn;
	}
	else
	{
		std::string relativePath	= "Data//Shaders//" + std::string(name);
		toReturn					= Shader::CreateNewFromFile( relativePath );
		
		m_shaderPool[ name ] = toReturn;
		return toReturn;
	}

}

void Renderer::UseShader( Shader const *useShader )
{
	m_currentShader = (useShader != nullptr) ? useShader : m_defaultShader;

	GLuint programHandle = m_currentShader->m_program->GetHandle();
	glUseProgram( programHandle );

	BindRenderState( m_currentShader->m_renderState );

	GL_CHECK_ERROR();
}

void Renderer::BindRenderState( RenderState const &renderState )
{
	// Cull Mode
	SetCullingMode( renderState.m_cullMode );
	
	// Fill Mode
	glPolygonMode( GL_FRONT_AND_BACK, GetAsOpenGLDataType( renderState.m_fillMode ) );

	// Wind Order
	glFrontFace( GetAsOpenGLDataType( renderState.m_frontFace ) );

	// Depth State
	if ( renderState.m_depthEnabled != true )
		EnableDepth( COMPARE_ALWAYS, false );
	else
		EnableDepth( renderState.m_depthCompare, renderState.m_depthWrite );

	// Blending
	if (renderState.m_blendEnabled)
	{
		// Setting operation for color & alpha
		glBlendEquationSeparate( GetAsOpenGLDataType( renderState.m_colorBlendOp ),		// ColorBlending
							     GetAsOpenGLDataType( renderState.m_alphaBlendOp ) );	// AlphaBlending

		// Setting (src, dest) blendFactor for color & alpha
		glBlendFuncSeparate( GetAsOpenGLDataType( renderState.m_colorSrcFactor ),		// ColorBlending
							 GetAsOpenGLDataType( renderState.m_colorDstFactor ),
							 GetAsOpenGLDataType( renderState.m_alphaSrcFactor ),		// AlphaBlending
							 GetAsOpenGLDataType( renderState.m_alphaDstFactor ) );
	}
}

void Renderer::BindMaterialForShaderIndex( Material &material, uint shaderIndex /* = 0 */ )
{
	// Bind ShaderProgram and State
	Shader const	*thisShader			= material.GetShader( shaderIndex );
	unsigned int	 thisShaderHandle	= thisShader->m_program->GetHandle();
	UseShader( thisShader );

	// Bind the Material specific properties
	for ( std::map< unsigned int, Texture const* >::iterator	bindPointTexturePair  = material.m_textureBindingPairs.begin();
																bindPointTexturePair != material.m_textureBindingPairs.end();
																bindPointTexturePair++ )
	{
		unsigned int	thisBindingPoint	= bindPointTexturePair->first;
		Texture const*	thisTexture			= bindPointTexturePair->second;
		Sampler const*	thisSampler			= material.m_samplerBindingPairs[ thisBindingPoint ];

		BindTexture2D( thisBindingPoint, *thisTexture, thisSampler );
	}

	// Bind Uniform Properties
	for each ( MaterialProperty *prop in std::get<1>(material.m_shaderGroup[ shaderIndex ]) )
		prop->Bind( thisShaderHandle );
}

void Renderer::SetUniform( char const *name, float flt )
{
	GLint float_loc = glGetUniformLocation( m_currentShader->m_program->GetHandle(), name );
	if (float_loc >= 0)
		glUniform1fv( float_loc, 1, &flt );
}

void Renderer::SetUniform( char const *name, Vector3 const &vct )
{
	float values[3] = { vct.x, vct.y, vct.z	};

	GLint vec3_loc = glGetUniformLocation( m_currentShader->m_program->GetHandle(), name );
	if (vec3_loc >= 0)
		glUniform3fv( vec3_loc, 1, (GLfloat*)&values );
}

void Renderer::SetUniform( char const *name, Vector4 const &vct )
{
	float values[4] = { vct.x, vct.y, vct.z, vct.w	};

	GLint vec4_loc = glGetUniformLocation( m_currentShader->m_program->GetHandle(), name );
	if (vec4_loc >= 0)
		glUniform4fv( vec4_loc, 1, (GLfloat*)&values );
}

void Renderer::SetUniform( char const *name, Rgba const &clr )
{
	Vector4 colorNormalized;
	clr.GetAsFloats( colorNormalized.x, colorNormalized.y, colorNormalized.z, colorNormalized.w );

	GLint floats_loc = glGetUniformLocation( m_currentShader->m_program->GetHandle(), name );
	if (floats_loc >= 0)
		glUniform4fv( floats_loc, 1, (GLfloat*)&colorNormalized );
}

void Renderer::SetUniform( char const *name, Matrix44 const &mat44 )
{
	GLint mat_loc = glGetUniformLocation( m_currentShader->m_program->GetHandle(), name );
	if (mat_loc >= 0)
		glUniformMatrix4fv( mat_loc, 1, GL_FALSE, (GLfloat*)&mat44 );
}

void Renderer::SetUniform( char const *name, uint unsignedInt )
{
	GLint float_loc = glGetUniformLocation( m_currentShader->m_program->GetHandle(), name );
	if (float_loc >= 0)
		glUniform1uiv( float_loc, 1, &unsignedInt );
}

void Renderer::UpdateTime( float gameDeltaSeconds, float systemDeltaSeconds )
{
	UBOTimeData *timeUBO = m_timeUBO->As< UBOTimeData >();

	timeUBO->gameDeltaTime		 = gameDeltaSeconds;
	timeUBO->gameTotalTime		+= gameDeltaSeconds;
	timeUBO->systemDeltaTime	 = systemDeltaSeconds;
	timeUBO->systemTotalTime	+= systemDeltaSeconds;

	m_timeUBO->UpdateGPU();
}

void Renderer::ApplyEffect( Shader* effectShader )
{
	// Get current colorTarget
	EnableDepth( eCompare::COMPARE_ALWAYS, true );

	// Apply Effect on it
	if ( s_effectCurrentSource == nullptr )
	{
		s_effectCurrentSource = s_defaultColorTarget;
	}

	if( s_effectCurrentTarget == nullptr )
	{
		s_effectCurrentTarget = s_sketchColorTarget;
	//	s_sketchColorTarget is created at RendererStartup
	}

	s_effectsCamera->SetColorTarget( s_effectCurrentTarget );
	s_effectsCamera->Finalize();			// NOTE! BindCamera( s_effectsCamera ) was used before, instead of Finalize()..!
	UseShader( effectShader );

	DrawTexturedAABB( AABB2::NDC_SIZE, *s_effectCurrentSource, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
	
	std::swap( s_effectCurrentTarget, s_effectCurrentSource );
}

void Renderer::EndEffect()
{
	// s_effectCurrentTarget is pointing to s_defaultColorTarget i.e. the last effects were applied on s_sketchColorTarget
	if( s_effectCurrentTarget == s_defaultColorTarget )
	{
		s_effectsCamera->SetColorTarget( s_defaultColorTarget ); // Render back to Default Color Target because this target will finally render onto the backbuffer
		s_effectsCamera->Finalize();							 // NOTE! BindCamera( s_effectsCamera ) was used before, instead of Finalize()..!
		UseShader( m_defaultShader );
		
		DrawTexturedAABB( AABB2::NDC_SIZE, *s_sketchColorTarget, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
	}

	s_effectCurrentSource = nullptr;
	s_effectCurrentTarget = nullptr;
}

//------------------------------------------------------------------------
HGLRC CreateOldRenderContext( HDC hdc ) 
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(pfd) ); 
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
	if ( pixel_format == NULL ) {
		return NULL; 
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat( hdc, pixel_format, &pfd )) {
		return NULL; 
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext( hdc );
	if (context == NULL) {
		return NULL; 
	}

	// return the context; 
	return context; 
}

void BindNewWGLFunctions()
{
	GL_BIND_FUNCTION( wglGetExtensionsStringARB ); 
	GL_BIND_FUNCTION( wglChoosePixelFormatARB ); 
	GL_BIND_FUNCTION( wglCreateContextAttribsARB );
}

void BindGLFunctions()
{
	GL_BIND_FUNCTION( glClear );
	GL_BIND_FUNCTION( glClearColor );
	GL_BIND_FUNCTION( glEnable );
	GL_BIND_FUNCTION( glDisable );
	GL_BIND_FUNCTION( glBlendFunc );
	GL_BIND_FUNCTION( glLineWidth );
	
	GL_BIND_FUNCTION( glDeleteShader );
	GL_BIND_FUNCTION( glCreateShader );
	GL_BIND_FUNCTION( glShaderSource );
	GL_BIND_FUNCTION( glCompileShader );
	GL_BIND_FUNCTION( glGetShaderiv );
	GL_BIND_FUNCTION( glGetShaderInfoLog );
	GL_BIND_FUNCTION( glCreateProgram );
	GL_BIND_FUNCTION( glAttachShader );
	GL_BIND_FUNCTION( glLinkProgram );
	GL_BIND_FUNCTION( glGetProgramiv );
	GL_BIND_FUNCTION( glDeleteProgram );
	GL_BIND_FUNCTION( glDetachShader );
	GL_BIND_FUNCTION( glGetProgramInfoLog );

	GL_BIND_FUNCTION( glDeleteBuffers);
	GL_BIND_FUNCTION( glGenBuffers );
	GL_BIND_FUNCTION( glBindBuffer );
	GL_BIND_FUNCTION( glBufferData );
	GL_BIND_FUNCTION( glGenVertexArrays );
	GL_BIND_FUNCTION( glBindVertexArray );
	GL_BIND_FUNCTION( glUseProgram );

	GL_BIND_FUNCTION( glGetAttribLocation );
	GL_BIND_FUNCTION( glEnableVertexAttribArray );
	GL_BIND_FUNCTION( glVertexAttribPointer );
	GL_BIND_FUNCTION( glDrawArrays );
	GL_BIND_FUNCTION( glDrawElements );

	GL_BIND_FUNCTION( glGetUniformLocation );
	GL_BIND_FUNCTION( glUniformMatrix4fv );
	GL_BIND_FUNCTION( glUniform1f );

	GL_BIND_FUNCTION( glPixelStorei );	
	GL_BIND_FUNCTION( glGenTextures	);
	GL_BIND_FUNCTION( glBindTexture	);
	GL_BIND_FUNCTION( glTexImage2D );
	GL_BIND_FUNCTION( glTexParameteri );

	GL_BIND_FUNCTION( glGenSamplers	);
	GL_BIND_FUNCTION( glDeleteSamplers );
	GL_BIND_FUNCTION( glSamplerParameteri );
	GL_BIND_FUNCTION( glBindSampler );
	GL_BIND_FUNCTION( glActiveTexture );

	GL_BIND_FUNCTION( glDepthFunc );
	GL_BIND_FUNCTION( glDepthMask );
	GL_BIND_FUNCTION( glClearDepthf );

	GL_BIND_FUNCTION( glGetError );

	GL_BIND_FUNCTION( glGenFramebuffers );
	GL_BIND_FUNCTION( glDeleteFramebuffers );

	GL_BIND_FUNCTION( glBindFramebuffer	);
	GL_BIND_FUNCTION( glFramebufferTexture );
	GL_BIND_FUNCTION( glDrawBuffers );
	GL_BIND_FUNCTION( glCheckFramebufferStatus );
	GL_BIND_FUNCTION( glBlitFramebuffer );

	GL_BIND_FUNCTION( glPolygonMode );
	GL_BIND_FUNCTION( glCullFace );
	GL_BIND_FUNCTION( glFrontFace );
	GL_BIND_FUNCTION( glBlendEquationSeparate );
	GL_BIND_FUNCTION( glBlendFuncSeparate );

	GL_BIND_FUNCTION( glUniform1uiv );
	GL_BIND_FUNCTION( glUniform1fv );
	GL_BIND_FUNCTION( glUniform2fv );
	GL_BIND_FUNCTION( glUniform3fv );
	GL_BIND_FUNCTION( glUniform4fv );
	GL_BIND_FUNCTION( glDisableVertexAttribArray );
	GL_BIND_FUNCTION( glGetIntegerv );
	GL_BIND_FUNCTION( glBindBufferBase );

	GL_BIND_FUNCTION( glDeleteTextures );
	GL_BIND_FUNCTION( glTexStorage2D );
	GL_BIND_FUNCTION( glTexSubImage2D );
	GL_BIND_FUNCTION( glReadPixels );

	GL_BIND_FUNCTION( glViewport );
	GL_BIND_FUNCTION( glSamplerParameterfv );

	GL_BIND_FUNCTION( glGenerateMipmap );
	GL_BIND_FUNCTION( glSamplerParameterf );

	GL_BIND_FUNCTION( wglSwapIntervalEXT );
}
	
//------------------------------------------------------------------------
// Creates a real context as a specific version (major.minor)
HGLRC CreateRealRenderContext( HDC hdc, int major, int minor ) 
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,		// The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,		// ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,			// ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,	// ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,					// 24 bits for color (8 bits per channel)
		// WGL_DEPTH_BITS_ARB, 24,				// if you wanted depth a default depth buffer...
		// WGL_STENCIL_BITS_ARB, 8,				// ...you could set these to get a 24/8 Depth/Stencil.
		NULL, NULL,								// Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;

	BOOL succeeded = wglChoosePixelFormatARB( 
		hdc, 
		format_attribs, 
		nullptr, 
		MAX_PIXEL_FORMATS, 
		formats, 
		(UINT*)&format_count );

	if (!succeeded) {
		return NULL; 
	}

	// Loop through returned formats, till we find one that works
	for (UINT i = 0; i < format_count; ++i) {
		pixel_format = formats[i];
		succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context; 
		if (succeeded) {
			break;
		} else {
			DWORD error = GetLastError();
			DebuggerPrintf( "Failed to set the format: %u", error ); 
		}
	}

	if (!succeeded) {
		return NULL; 
	}

	// Okay, HDC is setup to the right format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0; 
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; 
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
	if (context == NULL) {
		return NULL; 
	}

	return context;
}


Renderer::Renderer()
{
	framesBottomLeft = Vector2(0.f, 0.f);
	framesTopRight = Vector2(1000.f, 1000.f);
	defaultInkColor = Rgba(255, 255, 255, 255);
	defaultColor = Rgba(255, 255, 255, 255);
	defaultDrawingThickness = 1.5f;

	
	glLineWidth( defaultDrawingThickness );
	glEnable( GL_BLEND );
	glEnable( GL_LINE_SMOOTH );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	// bool loaded = m_defaultShader.LoadFromFiles("Data/Shaders/default");
	LoadAllInbuiltShaders();

	m_temp_render_buffer = new RenderBuffer();

	// Setting up the default texture(s)
	Image white1x1Image( RGBA_WHITE_COLOR );
	m_defaultWhiteTexture = new Texture( white1x1Image );			// Diffuse Texture : WHITE
	Image normal1x1Image( Rgba( 127, 127, 255, 255 ) );
	m_defaultNormalTexture = new Texture( normal1x1Image );			// Normal Texture  : RGB( 0.5, 0.5, 1.0 )
	Image emessive1x1Image( RGBA_BLACK_COLOR );
	m_defaultEmissiveTexture = new Texture( emessive1x1Image );		// Emissive Texture: BLACK

	m_immediateMesh = new Mesh();

	// Creating the UBO
	UBOTimeData timeStructToCopy;
	m_timeUBO = UniformBuffer::For< UBOTimeData >( timeStructToCopy );
	glBindBufferBase( GL_UNIFORM_BUFFER, 1, m_timeUBO->GetHandle() );

	UBOObjectLightData objectLightDataToCopy;
	m_objectLightDataUBO = UniformBuffer::For< UBOObjectLightData >( objectLightDataToCopy );
	glBindBufferBase( GL_UNIFORM_BUFFER, 3, m_objectLightDataUBO->GetHandle() );

	UBOLightsBlock lightsBlockToCopy;
	m_lightsBlockUBO = UniformBuffer::For< UBOLightsBlock >( lightsBlockToCopy );
	glBindBufferBase( GL_UNIFORM_BUFFER, 4, m_lightsBlockUBO->GetHandle() );

	// set our default camera to be our current camera
	BindCamera( nullptr );

	s_renderer = this;
}

Renderer::~Renderer()
{
	if( s_renderer != nullptr )
		s_renderer = nullptr;

	// Empty the TexturePool
	while ( m_texturePool.size() > 0 )
	{
		delete m_texturePool.back().texture;
		m_texturePool.back().texture = nullptr;
		m_texturePool.pop_back();
	}

	// Empty the ShaderPool
	for ( std::map< std::string, Shader*>::iterator it  = m_shaderPool.begin(); 
													it != m_shaderPool.end(); 
													it++ )
	{
		delete it->second;
		it->second = nullptr;
	}
	m_shaderPool.clear();

	// Other delete(s)
	delete m_lightsBlockUBO;
	delete m_objectLightDataUBO;
	delete m_timeUBO;

	if( m_immediateMesh != nullptr )
		delete m_immediateMesh;

	delete m_temp_render_buffer;
	delete m_defaultEmissiveTexture;
	delete m_defaultNormalTexture;
	delete m_defaultWhiteTexture;
}

Renderer* Renderer::GetInstance()
{
	return s_renderer;
}

void Renderer::BeginFrame() 
{
	UseShader( m_defaultShader );

	m_lightsBlockUBO->UpdateGPU();
	m_objectLightDataUBO->UpdateGPU();

	// To prevent RenderDoc from crashing..
	for (int i = 0; i < 16; i++)
		glDisableVertexAttribArray( (GLint) i );
}

void Renderer::EndFrame() 
{
	PROFILE_SCOPE_FUNCTION();

	// copies the default camera's framebuffer to the "null" framebuffer, 
	// also known as the back buffer.
	if( s_current_camera != nullptr )
		CopyFrameBuffer( nullptr, &s_current_camera->m_outputFramebuffer ); 
	else
		CopyFrameBuffer( nullptr, &s_default_camera->m_outputFramebuffer );

	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	SwapBuffers( gHDC ); 
}

void Renderer::ClearScreen( const Rgba& clearColor ) {

	float r, g, b, a;
	clearColor.GetAsFloats(r, g, b, a);					// [0, 255] to [0.f, 1.f]
	
	// Clear all screen (backbuffer) pixels to given color
	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::ClearColor( const Rgba& clearColor )
{
	glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::SetAmbientLight( Vector4 normalizedAmbientLight )
{
	normalizedAmbientLight.Clamp01();
	
	UBOLightsBlock *thelightsBlock = m_lightsBlockUBO->As< UBOLightsBlock >();
	thelightsBlock->ambientLight = normalizedAmbientLight;
}

void Renderer::SetAmbientLight( float intensity, Rgba const &color )
{
	UBOLightsBlock *theLightsBlock = m_lightsBlockUBO->As< UBOLightsBlock >();

	Vector3 normalizedColor			= color.GetAsNormalizedRgba().IgnoreW();
	theLightsBlock->ambientLight.x	= normalizedColor.x;
	theLightsBlock->ambientLight.y	= normalizedColor.y;
	theLightsBlock->ambientLight.z	= normalizedColor.z;
	theLightsBlock->ambientLight.w	= ClampFloat01( intensity );
}

void Renderer::DisableAllLights()
{
	UBOLightsBlock *theLightsBlock = m_lightsBlockUBO->As< UBOLightsBlock >();

	for (unsigned int i = 0; i < s_maxLights; i++)
		theLightsBlock->lights[i].colorAndIntensity = Vector4::ZERO;
}

void Renderer::EnableLight(unsigned int idx, Light const &theLight)
{
	if( idx >= s_maxLights )
		return;

	UBOLightsBlock	*theLightsBlock = m_lightsBlockUBO->As< UBOLightsBlock >();
	LightData		&lightToChange	= theLightsBlock->lights[idx];
	
	lightToChange					= theLight.GetLightDataForUBO();
}

void Renderer::SetSpecularConstants( float specAmount, float specPower )
{
	UBOObjectLightData *theObjectLightData = m_objectLightDataUBO->As< UBOObjectLightData >();

	theObjectLightData->specularPower	= specPower;
	theObjectLightData->specularAmount	= specAmount;
}

void Renderer::UpdateLightUBOs()
{
	// Binding the objectLightData
	m_objectLightDataUBO->UpdateGPU();

	// Binding the lightsBlock
	m_lightsBlockUBO->UpdateGPU();
}

void Renderer::SetCurrentDiffuseTexture( Texture const * newTexture )
{
	if( newTexture != nullptr )
		BindTexture2D( 0, *newTexture );
	else
		BindTexture2D( 0, *m_defaultWhiteTexture );
}

void Renderer::SetCurrentNormalTexture( Texture const * newNormalTexture )
{
	if( newNormalTexture != nullptr )
		BindTexture2D( 1, *newNormalTexture );
	else
		BindTexture2D( 1, *m_defaultNormalTexture );
}

void Renderer::SetCurrentEmissiveTexture( Texture const * newEmessiveTexture )
{
	if( newEmessiveTexture != nullptr )
		BindTexture2D( 2, *newEmessiveTexture );
	else
		BindTexture2D( 2, *m_defaultEmissiveTexture );
}

void Renderer::BindCamera( Camera *camera )
{
	// Set the Current Camera
	if( camera != nullptr )
		s_current_camera = camera;
	else
		s_current_camera = s_default_camera;

	// Framebuffer & UBO updation
	s_current_camera->Finalize();															// Bind Camera's Framebuffer
	s_current_camera->UpdateUBO();															// Update UBO
	glBindBufferBase( GL_UNIFORM_BUFFER, 2, s_current_camera->m_cameraUBO->GetHandle() );	// Bind UBO
	
	// Set glVewPort
	uint minX	= 0U;
	uint minY	= 0U;
	uint width	= s_current_camera->m_outputFramebuffer.GetWidth();
	uint height = s_current_camera->m_outputFramebuffer.GetHeight();
	glViewport( minX, minY, width, height );

	GL_CHECK_ERROR();
}

void Renderer::DrawMesh( Mesh const &mesh, const Matrix44 & modelMatrix /* = Matrix44() */ )
{
	Camera *activeCamera = ( s_current_camera != nullptr ) ? s_current_camera : s_default_camera;
	
	BindMeshToProgram( m_currentShader->m_program, &mesh );
	
	// Bind all the Uniforms
	SetUniform( "MODEL", modelMatrix );
	SetUniform( "EYE_POSITION", activeCamera->m_cameraTransform.GetWorldPosition() );

	// Update the Light UBO
	UpdateLightUBOs();

	GLenum glPrimitiveType = GetAsOpenGLPrimitiveType( mesh.m_drawCallInstruction.primitiveType );

	if( mesh.m_drawCallInstruction.isUsingIndices == true )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.m_ibo->GetHandle() );
		glDrawElements( glPrimitiveType, mesh.m_drawCallInstruction.elementCount, GL_UNSIGNED_INT, 0 );
	}
	else
	{
		glDrawArrays( glPrimitiveType, 0, mesh.m_drawCallInstruction.elementCount );
	}
	
	GL_CHECK_ERROR();
}

void Renderer::BindMeshToProgram( ShaderProgram const *shaderProgram, Mesh const *mesh )
{
	glBindBuffer( GL_ARRAY_BUFFER, mesh->m_vbo->GetHandle() );

	unsigned int	vertexStride	= mesh->GetVertexStride();
	GLuint			programHandle	= shaderProgram->GetHandle();
	unsigned int	attributeCount	= mesh->m_layout->GetAttributeCount();

	for( unsigned int attributeIdx = 0; attributeIdx < attributeCount; attributeIdx++ )
	{
		VertexAttribute const &attribute = mesh->m_layout->GetAttributeAtIndex( attributeIdx );
		
		int bind = glGetAttribLocation( programHandle, attribute.name.c_str() );
		if( bind >= 0 )
		{
			glEnableVertexAttribArray( bind );

			glVertexAttribPointer( 
				bind,
				attribute.elementsCount,
				GetAsOpenGLDataType( attribute.type ),
				attribute.normalize,
				vertexStride,
				(GLvoid*) attribute.memberOffset );
		}
	}
	
	GL_CHECK_ERROR();
}

void Renderer::DrawCube( const Vector3& center, const Vector3& dimensions, // width, height, depth
							const Rgba&  color		/* = RGBA_WHITE_COLOR  */, 
							const AABB2& uv_top		/* = AABB2::ONE_BY_ONE */, 
							const AABB2& uv_side	/* = AABB2::ONE_BY_ONE */, 
							const AABB2& uv_bottom	/* = AABB2::ONE_BY_ONE */ )
{	
	/*
	      h_________ g			VERTEX[8] ORDER:
		  /|       /|				( a, b, c, d, e, f, g, h )  is,
		 / | top  / |				( 0, 1, 2, 3, 4, 5, 6, 7 )  .
	   e/__|_____/f |			
		|  |_____|__|			   y|   
		| d/     |  /c				|  / z
		| /  bot | /				| /
		|/_______|/					|/______ x
		a         b
	*/

	const Vector3 half_dimensions = dimensions / 2.f;

	// Get coordinates for all vertexes
	const Vector3 bot_center	( center.x,		center.y - half_dimensions.y,		center.z );
	const Vector3 top_center	( center.x,		center.y + half_dimensions.y,		center.z );

	const Vector3 vertexPos[8] = {
		Vector3( bot_center.x - half_dimensions.x, bot_center.y, bot_center.z - half_dimensions.z ),
		Vector3( bot_center.x + half_dimensions.x, bot_center.y, bot_center.z - half_dimensions.z ),
		Vector3( bot_center.x + half_dimensions.x, bot_center.y, bot_center.z + half_dimensions.z ),
		Vector3( bot_center.x - half_dimensions.x, bot_center.y, bot_center.z + half_dimensions.z ),
		Vector3( top_center.x - half_dimensions.x, top_center.y, top_center.z - half_dimensions.z ),
		Vector3( top_center.x + half_dimensions.x, top_center.y, top_center.z - half_dimensions.z ),
		Vector3( top_center.x + half_dimensions.x, top_center.y, top_center.z + half_dimensions.z ),
		Vector3( top_center.x - half_dimensions.x, top_center.y, top_center.z + half_dimensions.z )
	};

	const Vertex_3DPCU vertexes[24] = {
		// Front Face
		Vertex_3DPCU( vertexPos[0], color, Vector2( uv_side.mins ) ),							// a, 0		e f
		Vertex_3DPCU( vertexPos[1], color, Vector2( uv_side.maxs.x, uv_side.mins.y ) ),			// b, 1		a b
		Vertex_3DPCU( vertexPos[5], color, Vector2( uv_side.maxs ) ),							// f, 2

																								// f
		Vertex_3DPCU( vertexPos[4], color, Vector2( uv_side.mins.x, uv_side.maxs.y ) ),			// e, 3
																								// a
		
		// Back Face
		Vertex_3DPCU( vertexPos[2], color, Vector2( uv_side.mins ) ),							// c, 4		g h
		Vertex_3DPCU( vertexPos[3], color, Vector2( uv_side.maxs.x, uv_side.mins.y ) ),			// d, 5		c d
		Vertex_3DPCU( vertexPos[7], color, Vector2( uv_side.maxs ) ),							// h, 6

																								// h
		Vertex_3DPCU( vertexPos[6], color, Vector2( uv_side.mins.x, uv_side.maxs.y ) ),			// g, 7
																								// c

		// Left Face
		Vertex_3DPCU( vertexPos[3], color, Vector2( uv_side.mins ) ),							// d, 8		h e
		Vertex_3DPCU( vertexPos[0], color, Vector2( uv_side.maxs.x, uv_side.mins.y ) ),			// a, 9		d a
		Vertex_3DPCU( vertexPos[4], color, Vector2( uv_side.maxs ) ),							// e, 10

																								// e
		Vertex_3DPCU( vertexPos[7], color, Vector2( uv_side.mins.x, uv_side.maxs.y ) ),			// h, 11
																								// d

		// Right Face
		Vertex_3DPCU( vertexPos[1], color, Vector2( uv_side.mins ) ),							// b, 12	f g
		Vertex_3DPCU( vertexPos[2], color, Vector2( uv_side.maxs.x, uv_side.mins.y ) ),			// c, 13	b c
		Vertex_3DPCU( vertexPos[6], color, Vector2( uv_side.maxs ) ),							// g, 14

																								// g
		Vertex_3DPCU( vertexPos[5], color, Vector2( uv_side.mins.x, uv_side.maxs.y ) ),			// f, 15
																								// b

		// Top Face
		Vertex_3DPCU ( vertexPos[4], color, Vector2( uv_top.mins ) ),							// e, 16	 h g
		Vertex_3DPCU ( vertexPos[5], color, Vector2( uv_top.maxs.x, uv_top.mins.y ) ),			// f, 17	 e f
		Vertex_3DPCU ( vertexPos[6], color, Vector2( uv_top.maxs ) ),							// g, 18

																								// g
		Vertex_3DPCU ( vertexPos[7], color, Vector2( uv_top.mins.x, uv_top.maxs.y ) ),			// h, 19
																								// e


		// Bottom Face
		Vertex_3DPCU ( vertexPos[3], color, Vector2( uv_bottom.mins ) ),						// d, 20	 a b
		Vertex_3DPCU ( vertexPos[2], color, Vector2( uv_bottom.maxs.x, uv_bottom.mins.y ) ),	// c, 21	 d c
		Vertex_3DPCU ( vertexPos[1], color, Vector2( uv_bottom.maxs ) ),						// b, 22
		
																								// b
		Vertex_3DPCU ( vertexPos[0], color, Vector2( uv_bottom.mins.x, uv_bottom.maxs.y ) ),	// a, 23
																								// d

		/*
				VERTEX[8] ORDER:
				 ( a, b, c, d, e, f, g, h )  is,
				 ( 0, 1, 2, 3, 4, 5, 6, 7 )  .
		*/
	};

	unsigned int indices[ 36 ] = {
		( 0 + 0), ( 0 + 1), ( 0 + 2), ( 0 + 2), ( 0 + 3), ( 0 + 0),
		( 4 + 0), ( 4 + 1), ( 4 + 2), ( 4 + 2), ( 4 + 3), ( 4 + 0),
		( 8 + 0), ( 8 + 1), ( 8 + 2), ( 8 + 2), ( 8 + 3), ( 8 + 0),
		(12 + 0), (12 + 1), (12 + 2), (12 + 2), (12 + 3), (12 + 0),
		(16 + 0), (16 + 1), (16 + 2), (16 + 2), (16 + 3), (16 + 0),
		(20 + 0), (20 + 1), (20 + 2), (20 + 2), (20 + 3), (20 + 0)
	};

	DrawMeshImmediate( vertexes, 24, indices, 36, PRIMITIVE_TRIANGES );
}

void Renderer::DrawTexturedCube( const Vector3& center, const Vector3& dimensions, 
								 const Rgba& color				 /* = RGBA_WHITE_COLOR */, 
								 const Texture* texture			 /* = nullptr */, 
								 const AABB2& uv_top			 /* = AABB2::ONE_BY_ONE */, 
								 const AABB2& uv_side			 /* = AABB2::ONE_BY_ONE */, 
								 const AABB2& uv_bottom			 /* = AABB2::ONE_BY_ONE */, 
								 const Texture* secondaryTexture /* = nullptr */           )
{
	m_secondaryTexture = secondaryTexture;
	
	Transform	modelTransform	= Transform( center, Vector3::ZERO, dimensions );
	Mesh*		cubeMesh		= MeshBuilder::CreateCube( Vector3::ONE_ALL, Vector3::ZERO, color, uv_top, uv_side, uv_bottom );
	
	SetCurrentDiffuseTexture( texture );
	DrawMesh( *cubeMesh, modelTransform.GetTransformMatrix() );

	delete cubeMesh;
}

void Renderer::DrawText2D( const Vector2& drawMins, const std::string& asciiText, float cellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	PROFILE_SCOPE_FUNCTION();

	Vector2 newMins = drawMins;
	float cellWidth = cellHeight * font->GetGlyphAspect( asciiText.at(0) );
	Vector2 newMaxs = Vector2( drawMins.x + cellWidth , drawMins.y + cellHeight );
	AABB2 boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );

	// To construct the Mesh
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );

	// For every character of the string
	for( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		// Draw that character
		AABB2 textCoords = font->GetUVsForGlyph( asciiText.at(i) );
		mb.AddPlane( boundForNextCharacter, 0.f, textCoords, tint );
//		DrawTexturedAABB( boundForNextCharacter, font->m_spriteSheet.m_spriteSheetTexture , textCoords.mins , textCoords.maxs, tint );

		// Calculate bounds to draw next character
		newMins = Vector2( newMins.x + cellWidth , newMins.y);
		newMaxs = Vector2( newMaxs.x + cellWidth , newMaxs.y);
		boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	}

	mb.End();
	
	Mesh *textMesh = mb.ConstructMesh<Vertex_Lit>();
	SetCurrentDiffuseTexture( &font->m_spriteSheet.m_spriteSheetTexture );
	DrawMesh( *textMesh );
	delete textMesh;
}

void Renderer::DrawTextInBox2D( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */, eTextDrawMode drawMode /* = TEXT_DRAW_OVERRUN */ )
{
	switch (drawMode)
	{
		case TEXT_DRAW_OVERRUN:
			DrawTextAsOverrun( asciiText, alignment, drawInBox, desiredCellHeight, tint, font );
			break;
		case TEXT_DRAW_SHRINK_TO_FIT:
			DrawTextAsShrinkToFit( asciiText, alignment, drawInBox, desiredCellHeight, tint, font );
			break;
		case TEXT_DRAW_WORD_WRAP:
			DrawTextAsWordWrap( asciiText, alignment, drawInBox, desiredCellHeight, tint, font );
			break;
		default:
			GUARANTEE_RECOVERABLE( false, std::string("No supported TextDrawMode found!") );
			break;
	}
}

void Renderer::DrawTextAsOverrun( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	std::vector< std::string > textLines;
	SetFromText( textLines, "\n", asciiText.c_str() );

	// For each line
	for( unsigned int i = 0; i < textLines.size(); i++ )
	{
		std::string thisLine = textLines[i];

		// Calculate Padding
		float boxWidth = drawInBox.maxs.x - drawInBox.mins.x;
		float widthOfLine = ( desiredCellHeight * font->GetGlyphAspect( asciiText.at(0) ) ) * thisLine.length();
		float totalPaddingWidth = boxWidth - widthOfLine;

		float boxHeight = drawInBox.maxs.y - drawInBox.mins.y;
		float heightOfLine = desiredCellHeight;
		float totalPaddingHeight = boxHeight - heightOfLine;

		// Calculate drawMins considering the padding
		Vector2 drawMins = Vector2( drawInBox.mins.x + (totalPaddingWidth * alignment.x) , drawInBox.mins.y + (totalPaddingHeight * alignment.y) );
		// Modify drawMins's y-axis accoring to the line number
		drawMins.y -= heightOfLine * i;

		DrawText2D( drawMins, thisLine, desiredCellHeight, tint, font );
	}

}

void Renderer::DrawTextAsShrinkToFit( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	std::vector< std::string > textLines;
	SetFromText( textLines, "\n", asciiText.c_str() );
	
	
	std::string thisLine = textLines[0];
	float maxLineWidth = ( desiredCellHeight * font->GetGlyphAspect( asciiText.at(0) ) ) * thisLine.length();
	// Get height of the whole text
	float totalTextHeight = textLines.size() * desiredCellHeight;
	
	// For each line
	for( unsigned int i = 1; i < textLines.size(); i++ )
	{
		thisLine = textLines[i];

		// Check for each line.. if they have larger width than maxLineWidth
		float widthOfLine = ( desiredCellHeight * font->GetGlyphAspect( asciiText.at(0) ) ) * thisLine.length();
		maxLineWidth = widthOfLine > maxLineWidth ? widthOfLine : maxLineWidth;
	}

	float boxWidth		= drawInBox.maxs.x - drawInBox.mins.x;
	float boxHeight		= drawInBox.maxs.y - drawInBox.mins.y;
	float scaleWidth	= boxWidth / maxLineWidth;
	float scaleHeight	= boxHeight / totalTextHeight;

	// Set final scale based on width & height
	float finalScale = 1.f;
	if( scaleWidth < 1.f || scaleHeight < 1.f )
		finalScale = scaleWidth < scaleHeight ? scaleWidth : scaleHeight;
	
	// Calculate shifting for vertical alignment of the text-paragraph
	float heightToShift = (totalTextHeight-desiredCellHeight) * finalScale * (1 - alignment.y);		// (1 - alignment.y) because: if alignment.y == 0, draw as overrun will start drawing first line at bottom of the drawInBox
	AABB2 adjustedDrawBox = AABB2(  drawInBox.mins.x, drawInBox.mins.y + heightToShift,
									drawInBox.maxs.x, drawInBox.maxs.y + heightToShift );

	DrawTextAsOverrun( asciiText, alignment, adjustedDrawBox, desiredCellHeight*finalScale, tint, font );
}

void Renderer::DrawTextAsWordWrap( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	// TODO: Make this function work for multiple textLines

	// Make vector of words to combine them
	std::vector< std::string > allWords;
	SetFromText( allWords, " ", asciiText.c_str() );	// This function will split by " ", so  " " itself won't be included in each word
	for( std::string& thisWord : allWords )				// Append " " at back on every word, for simplicity
		thisWord += " ";

	// Make newLines according to words
	std::vector< std::string > newLines;
	newLines.push_back("");								// I'm doing this for upcoming newLines.back() function call on empty vector.
	
	float boxWidth = drawInBox.maxs.x - drawInBox.mins.x;

	for( std::string thisWord : allWords )
	{
		std::string& lastString_newLines = newLines.back();
		unsigned int totalChars_lastString = (int) lastString_newLines.length();
		float totalWidth_newLine = (float)(thisWord.length() + totalChars_lastString) * desiredCellHeight * 1.f /* aspect ratio */;

		// If newLines last string's length is less than box width, append thisWord
		if( totalWidth_newLine <= boxWidth )
		{
			lastString_newLines += thisWord;
		}
		// Else pushBack thisWord
		else
		{
			newLines.push_back( thisWord );
		}
	}

	// Combine all newLines into one string separated by '\n'
	std::string bakedString;
	for( std::string aLine : newLines )
	{
		bakedString += aLine;
		bakedString += "\n";
	}

	// Remove the last, unnecessary "\n"
	bakedString = bakedString.substr(0, bakedString.size()-2);

	// Pass it to DrawTextAsShrinkToFit
	DrawTextAsShrinkToFit( bakedString, alignment, drawInBox, desiredCellHeight, tint, font );
}

void Renderer::BindTexture2D( unsigned int bindIndex, const Texture& theTexture, Sampler const *theSampler /* = nullptr */ )
{
	GLuint textureIndex = bindIndex; // to see how they tie together
	
	// Bind the sampler;
	if( theSampler == nullptr )
		glBindSampler( textureIndex, s_defaultNearestSampler->GetHandle() ); 
	else
		glBindSampler( textureIndex, theSampler->GetHandle() );

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture( GL_TEXTURE_2D, theTexture.m_textureID ); 
}

void Renderer::BindTexture2D( unsigned int bindIndex, const uint textureHandle, Sampler const *theSampler /*= nullptr */ )
{
	GLuint textureIndex = bindIndex; // to see how they tie together

	// Bind the sampler;
	if( theSampler == nullptr )
		glBindSampler( textureIndex, s_defaultNearestSampler->GetHandle() ); 
	else
		glBindSampler( textureIndex, theSampler->GetHandle() );

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture( GL_TEXTURE_2D, textureHandle ); 
}

void Renderer::BindTextureCube( unsigned int bindIndex, const TextureCube& texCube, Sampler const *theSampler /*= nullptr */ )
{
	GLuint textureIndex = bindIndex; // to see how they tie together

	// Bind the sampler;
	if( theSampler == nullptr )
		glBindSampler( textureIndex, s_defaultLinearSampler->GetHandle() ); 
	else
		glBindSampler( textureIndex, theSampler->GetHandle() );

	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture( GL_TEXTURE_CUBE_MAP, texCube.m_handle ); 
}

void Renderer::DrawAABB( const AABB2& bounds, const Rgba& color ) 
{
	Vector2		xyPosition		= ( bounds.maxs + bounds.mins ) * 0.5f;
	Vector2		xyDimension		= Vector2( bounds.maxs.x - bounds.mins.x, bounds.maxs.y - bounds.mins.y );
	Transform	modelTransform	= Transform( xyPosition.GetAsVector3(), Vector3::ZERO, xyDimension.GetAsVector3() );
	Mesh*		planeMesh		= MeshBuilder::CreatePlane( Vector2::ONE_ONE, Vector3::ZERO, color );

	SetCurrentDiffuseTexture( nullptr );
	DrawMesh( *planeMesh, modelTransform.GetTransformMatrix() );
	delete planeMesh;
}


void Renderer::DrawTexturedAABB( const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint ) 
{
	Vector2		xyPosition		= ( bounds.maxs + bounds.mins ) * 0.5f;
	Vector2		xyDimension		= Vector2( bounds.maxs.x - bounds.mins.x, bounds.maxs.y - bounds.mins.y );
	Transform	modelTransform	= Transform( xyPosition.GetAsVector3(), Vector3::ZERO, xyDimension.GetAsVector3() );
	Mesh*		planeMesh		= MeshBuilder::CreatePlane( Vector2::ONE_ONE, Vector3::ZERO, tint, AABB2( texCoordsAtMins, texCoordsAtMaxs) );
	
	SetCurrentDiffuseTexture( &texture );
	DrawMesh( *planeMesh, modelTransform.GetTransformMatrix() );
	delete planeMesh;
}

void Renderer::DrawTexturedAABB( const Matrix44 &transformMatrix, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint )
{
	Mesh* planeMesh	= MeshBuilder::CreatePlane( Vector2::ONE_ONE, Vector3::ZERO, tint, AABB2( texCoordsAtMins, texCoordsAtMaxs) );
	
	SetCurrentDiffuseTexture( &texture );
	DrawMesh( *planeMesh, transformMatrix );
	delete planeMesh;
}

Texture* Renderer::CreateOrGetTexture( const std::string& pathToImage ) {
	Texture* referenceToTexture = nullptr;															// to get texture from texturePool
	bool textureExistInPool = findTextureFromPool( pathToImage, referenceToTexture );				// find existing texture from texturePool

																									// if not found,
	if ( textureExistInPool == false ) {
		// create new texture and add it to the pool
		referenceToTexture = new Texture( pathToImage );

		LoadedTexturesData newTexData = LoadedTexturesData( pathToImage, referenceToTexture );
		m_texturePool.push_back(newTexData);
	}

	return referenceToTexture;
}
 
 BitmapFont* Renderer::CreateOrGetBitmapFont( const char* bitmapFontName )
 {
	 BitmapFont* fontToReturn = nullptr;

	 // If font already in the pool, return its pointer
	 bool fontExistsInPool = findBitmapFontFromPool( std::string(bitmapFontName) , fontToReturn );
	 if( fontExistsInPool )
	 {
		 return fontToReturn;
	 }
	 else
	 {
		 // Create path according to the string..
		 std::string pathToPNG = std::string("Data//Fonts//");
		 pathToPNG += bitmapFontName;
		 pathToPNG += ".png";

		 // Get Texture of fonts' sheet
		 Texture* bitmapFontTexture = CreateOrGetTexture(pathToPNG);

		 // Make a spritesheet out of it
		 SpriteSheet* bitmapFontSpritesheet = new SpriteSheet( *bitmapFontTexture , 16 , 16 );

		 // Make a bitmapFont out of it
		 fontToReturn = new BitmapFont( std::string(bitmapFontName) , *bitmapFontSpritesheet , 1.f );

		 // Add it to pool
		 m_bitmapFontPool[ std::string(bitmapFontName) ] = fontToReturn;

		 // return the bitmapFont
		 return fontToReturn;
	 }
	 
 }

bool Renderer::findTextureFromPool( const std::string& pathToImage , Texture* &foundTexture ) {
	
	for(unsigned int i=0; i<m_texturePool.size(); i++) {

		if( pathToImage == m_texturePool[i].pathToImage ) {
			foundTexture = m_texturePool[i].texture;
			return true;
		}
	}

	return false;
}

bool Renderer::findBitmapFontFromPool( const std::string& nameOfFont , BitmapFont* &foundFont )
{
	std::map< std::string , BitmapFont* >::iterator it = m_bitmapFontPool.find( nameOfFont );

	if( it != m_bitmapFontPool.end() )
	{
		foundFont = it->second;
		return true;
	}

	return false;
}

bool Renderer::FindShaderFromPool( const std::string& nameOfShaderProgram, Shader* &foundShader )
{
	std::map< std::string , Shader* >::iterator it = m_shaderPool.find( nameOfShaderProgram );

	if( it != m_shaderPool.end() )
	{
		foundShader = it->second;
		return true;
	}

	return false;
}

Vector3 Renderer::GetDrawPositionUsingAnchorPoint( const Vector3& position, const Vector3& dimensions, const Vector3& anchorPoint /* = Vector3::ZERO */ )
{
	Vector3 halfDimensions = dimensions * 0.5f;
	Vector3 anchorPoin_X_halfDimensions = Vector3( anchorPoint.x * halfDimensions.x, anchorPoint.y * halfDimensions.y, anchorPoint.z * halfDimensions.z );
	return position - anchorPoin_X_halfDimensions;
}

//------------------------------------------------------------------------
void Renderer::EnableDepth( eCompare compare, bool should_write )
{
	// enable/disable the test
	glEnable( GL_DEPTH_TEST ); 
	glDepthFunc( GetAsOpenGLDataType(compare) ); 

	// enable/disable write
	glDepthMask( should_write ? GL_TRUE : GL_FALSE ); 
}

//------------------------------------------------------------------------
void Renderer::DisableDepth() 
{
	// You can glDisable( GL_DEPTH_TEST ) as well, 
	// but that prevents depth clearing from working, 
	// so I prefer to just use these settings which is 
	// effectively not using the depth buffer.
	EnableDepth( COMPARE_ALWAYS, false ); 
}

//------------------------------------------------------------------------
void Renderer::ClearDepth( float depth /* = 1.0f */ )
{
	glDepthMask( GL_TRUE );
	glClearDepthf( depth );
	glClear( GL_DEPTH_BUFFER_BIT ); 
}

void Renderer::SetCullingMode( eCullMode newCullMode )
{
	if (newCullMode != CULLMODE_NONE)
	{
		glEnable( GL_CULL_FACE );
		glCullFace( GetAsOpenGLDataType( newCullMode ) );
	}
	else
		glDisable( GL_CULL_FACE );
}

Texture* Renderer::CreateRenderTarget( unsigned int width, unsigned int height, eTextureFormat fmt /* = TEXTURE_FORMAT_RGBA8 */ )
{
	Texture *tex = new Texture();
	tex->CreateRenderTarget( width, height, fmt ); 

	return tex;
}

Texture* Renderer::CreateDepthStencilTarget( unsigned int width, unsigned int height )
{
	return CreateRenderTarget( width, height, TEXTURE_FORMAT_D24S8 ); 
}

Sampler const* Renderer::GetDefaultSampler( eSamplerType type /* = SAMPLER_NEAREST */ )
{
	if( type == SAMPLER_LINEAR )
		return s_defaultLinearSampler;
	else
		return s_defaultNearestSampler;
}

Texture* Renderer::GetDefaultColorTarget()
{
	return s_defaultColorTarget;
}

Texture* Renderer::GetDefaultDepthTarget()
{
	return s_defaultDepthTarget;
}