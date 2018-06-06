#pragma once

#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library
#include "Engine/Renderer/glfunctions.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"

GLuint		Renderer::s_default_vao			= NULL;
Sampler*	Renderer::s_defaultSampler		= nullptr;
Texture*	Renderer::s_defaultColorTarget	= nullptr;
Texture*	Renderer::s_defaultDepthTarget	= nullptr;
Camera*		Renderer::s_default_camera		= nullptr;
Camera*		Renderer::s_current_camera		= nullptr;

Texture*	Renderer::s_effectCurrentSource = nullptr;
Texture*	Renderer::s_effectCurrentTarget = nullptr;
Texture*	Renderer::s_sketchColorTarget	= nullptr;
Camera*		Renderer::s_effectsCamera		= nullptr;

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
bool Renderer::RendererStartup( void* hwnd_voidptr ) 
{
	HWND hwnd = static_cast<HWND>( hwnd_voidptr );

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

	PostStartup();

	return true; 
}

void Renderer::RendererShutdown()
{
	ReleaseDC( gGLwnd, gHDC );

	delete s_defaultSampler;
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
	// default_vao is a GLuint member variable
	glGenVertexArrays( 1, &s_default_vao ); 
	glBindVertexArray( s_default_vao ); 

	s_defaultSampler = new Sampler();

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

	// set our default camera to be our current camera
	SetCurrentCameraTo( nullptr ); 
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
	ShaderProgram* def_shaderProg = LoadShaderProgramFromStrings( "default", ShaderProgram::GetDefaultVertexShaderSource(), ShaderProgram::GetDefaultFragmentShaderSource() );
	
	m_defaultShader = def_shaderProg;
	m_currentShader = m_defaultShader;
}

ShaderProgram* Renderer::LoadShaderProgramFromStrings( const char* name, const char* vs_program, const char* fs_program )
{
	// If shader program exists, return the old one
	std::map< std::string, ShaderProgram* >::iterator it = m_shaderProgramPool.find( name );
	if( it != m_shaderProgramPool.end() )
	{
		return m_shaderProgramPool[ name ];
	}
	else
	{
		ShaderProgram* toReturn = new ShaderProgram();
		bool newShaderLoaded = toReturn->LoadFromStrings( vs_program, fs_program );

		GUARANTEE_RECOVERABLE( newShaderLoaded == true, std::string("Renderer: Failed to load ") + std::string(name) + std::string(" shader") );

		m_shaderProgramPool[ name ] = toReturn;
		return toReturn;
	}
}

ShaderProgram* Renderer::CreateOrGetShaderProgram( const char* name )
{
	// check for the name in pool
		// if found, return its pointer
		// else create a new one, add to pool and return appropriate pointer
			// newShader in success & defaultShader on failure
	ShaderProgram* toReturn = nullptr;
	bool foundShaderProgram = FindShaderProgramFromPool( name, toReturn );

	if( foundShaderProgram )
	{
		return toReturn;
	}
	else
	{
		std::string relativePath = "Data//Shaders//" + std::string(name);

		toReturn = new ShaderProgram();
		bool newShaderLoaded = toReturn->LoadFromFiles( relativePath.c_str() );

		if( newShaderLoaded )
		{
			m_shaderProgramPool[ name ] = toReturn;
			return toReturn;
		}
		else
		{
			delete toReturn;
			return m_defaultShader;
		}
	}

}

void Renderer::UseShaderProgram( ShaderProgram* useShader )
{
	m_currentShader = useShader != nullptr ? useShader : m_defaultShader;
}

void Renderer::SetPassFloatForShaderTo( float passFloatToShader )
{
	m_passFloatToShader = passFloatToShader;
}

void Renderer::ResetPassFloatForShaderToZero()
{
	m_passFloatToShader = 0.f;
}

void Renderer::ApplyEffect( ShaderProgram* effectShaderProgram )
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
	SetCurrentCameraTo( s_effectsCamera );
	UseShaderProgram( effectShaderProgram );

	DrawTexturedAABB( AABB2::NDC_SIZE, *s_effectCurrentSource, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
	
	std::swap( s_effectCurrentTarget, s_effectCurrentSource );
}

void Renderer::EndEffect()
{
	// s_effectCurrentTarget is pointing to s_defaultColorTarget i.e. the last effects were applied on s_sketchColorTarget
	if( s_effectCurrentTarget == s_defaultColorTarget )
	{
		s_effectsCamera->SetColorTarget( s_defaultColorTarget ); // Render back to Default Color Target because this target will finally render onto the backbuffer
		SetCurrentCameraTo( s_effectsCamera );
		UseShaderProgram( m_defaultShader );
		
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



int g_openGlPrimitiveTypes[ NUM_PRIMITIVE_TYPES ] =
{
	GL_POINTS,			// called PRIMITIVE_POINTS		in our engine
	GL_LINES,			// called PRIMITIVE_LINES		in our engine
	GL_TRIANGLES,		// called PRIMITIVE_TRIANGES	in our engine
};

int g_openGlDataTypes[ NUM_RDTs ] =
{
	GL_FLOAT,
	GL_UNSIGNED_BYTE
};


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

	// Setting up the default white texture
	Image white1x1Image( RGBA_WHITE_COLOR );
	m_defaultWhiteTexture = new Texture( white1x1Image );

	m_immediateMesh = new Mesh();
}

Renderer::Renderer( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& inkColor, float drawingThickness ) {
	framesBottomLeft = bottomLeft;
	framesTopRight = topRight;
	this->defaultInkColor = inkColor;
	this->defaultDrawingThickness = drawingThickness;

	glLineWidth( defaultDrawingThickness );
	glEnable( GL_BLEND );
	glEnable( GL_LINE_SMOOTH );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	LoadAllInbuiltShaders();

	m_temp_render_buffer = new RenderBuffer();

	// Setting up the default white texture
	Image white1x1Image( RGBA_WHITE_COLOR );
	m_defaultWhiteTexture = new Texture( white1x1Image );

	m_immediateMesh = new Mesh();
}

Renderer::~Renderer()
{
	TODO("Delete all loaded Textures of the texturePool..");

	if( m_immediateMesh != nullptr )
		delete m_immediateMesh;

	delete m_temp_render_buffer;
	delete m_defaultWhiteTexture;
}

void Renderer::BeginFrame() 
{
	UseShaderProgram( m_defaultShader );
}

void Renderer::EndFrame() 
{
	// copies the default camera's framebuffer to the "null" framebuffer, 
	// also known as the back buffer.
	if( s_current_camera != nullptr )
		CopyFrameBuffer( nullptr, &s_current_camera->m_output_framebuffer ); 
	else
		CopyFrameBuffer( nullptr, &s_default_camera->m_output_framebuffer );

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

void Renderer::SetProjectionMatrix( float screen_height, float screen_near, float screen_far )
{
	if( s_current_camera == nullptr )
		s_default_camera->SetProjectionOrtho( screen_height, screen_near, screen_far );
	else
		s_current_camera->SetProjectionOrtho( screen_height, screen_near, screen_far );
}

void Renderer::SetCurrentTexture( Texture const * newTexture )
{
	m_currentTexture = newTexture;
}

void Renderer::DrawMesh( Mesh const &mesh, const Matrix44 & modelMatrix /* = Matrix44() */ )
{
	if( m_currentTexture != nullptr )
		BindTexture2D( *m_currentTexture );					// If rendering a texture
	else
		BindTexture2D( *m_defaultWhiteTexture );			// If rendering solid color, bind white-texture(1x1)

	if( m_secondaryTexture != nullptr )
		BindTexture2D( *m_secondaryTexture, 1 );

	Camera* activeCamera = nullptr;
	if( s_current_camera != nullptr )
		activeCamera = s_current_camera;
	else
		activeCamera = s_default_camera;


	BindMeshToProgram( m_currentShader, &mesh );


	// Now that it is described and bound, draw using our program
	unsigned int program_handle = m_currentShader->GetHandle();
	glUseProgram( program_handle ); 

	// m_projection_matrix is a local variable;
	GLint proj_mat_loc = glGetUniformLocation( program_handle, "PROJECTION" );
	if (proj_mat_loc >= 0)
		glUniformMatrix4fv( proj_mat_loc, 1, GL_FALSE, (GLfloat*)&activeCamera->m_proj_matrix );

	GLint view_mat_loc = glGetUniformLocation( program_handle, "VIEW" );
	if (view_mat_loc >= 0)
		glUniformMatrix4fv( view_mat_loc, 1, GL_FALSE, (GLfloat*)&activeCamera->m_view_matrix );

	GLint model_mat_loc = glGetUniformLocation( program_handle, "MODEL" );
	if (model_mat_loc >= 0)
		glUniformMatrix4fv( model_mat_loc, 1, GL_FALSE, (GLfloat*)&modelMatrix );

	GLint pass_var_loc = glGetUniformLocation( program_handle, "PASSEDFLOAT" );
	if( pass_var_loc >= 0 )
		glUniform1f( pass_var_loc, m_passFloatToShader );


	glBindFramebuffer( GL_FRAMEBUFFER, activeCamera->GetFrameBufferHandle() );
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
		
		unsigned int bind = glGetAttribLocation( programHandle, attribute.name.c_str() );
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
}

void Renderer::DrawLine( const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float lineThickness ) {
// 	UNIMPLEMENTED();

	UNUSED( start );
	UNUSED( end );
	UNUSED( startColor );
	UNUSED( endColor );
	UNUSED( lineThickness );
	/*
	
	// Setting line's thickness
	glLineWidth(lineThickness);
	glDisable( GL_TEXTURE_2D );

	Vertex_3DPCU verts[] = { 
		Vertex_3DPCU( start.GetAsVector3(), startColor, Vector2::ZERO ),
		Vertex_3DPCU( end.GetAsVector3(),	endColor,	Vector2::ZERO )
	};

	DrawMeshImmediate( verts, 2, PRIMITIVE_LINES );

	// Reset ink color & thickness
	glColor4ub(defaultInkColor.r, defaultInkColor.g, defaultInkColor.b, defaultInkColor.a);
	glLineWidth(defaultDrawingThickness);

	*/
}

void Renderer::DrawFromVertexArray( const Vector2 vertex[], int arraySize, const Vector2& center, float orientationDegree, float scale ) {
// 	UNIMPLEMENTED();

	UNUSED( vertex );
	UNUSED( arraySize );
	UNUSED( center );
	UNUSED( orientationDegree );
	UNUSED( scale );
	/*

	glPushMatrix();
	glTranslatef(center.x, center.y, 0.f);
	glRotatef(orientationDegree, 0.f, 0.f, 1.f);
	glScalef(scale, scale, scale);

	// For loop for every line
	for(int i=0; i<arraySize-1; i++) {
		DrawLine(vertex[i], vertex[i+1], defaultInkColor, defaultInkColor, defaultDrawingThickness);
	} 
	DrawLine(vertex[arraySize-1], vertex[0], defaultInkColor, defaultInkColor, defaultDrawingThickness);

	glPopMatrix();

	*/
}

void Renderer::DrawPolygon( const Vector2& center, float radius, float sides, float orientationAngle ) {
	// For loop for every line
	for(float i=0; i<sides; i++) {
		float startAngle = ( i * (360.f/sides) );
		float endAngle = startAngle + (360.f/sides);

		Vector2 startPoint;
		Vector2 endPoint;

		startPoint.x = ( center.x + (radius * CosDegree(startAngle + orientationAngle)));
		startPoint.y = ( center.y + (radius * SinDegree(startAngle + orientationAngle)));
		endPoint.x   = ( center.x + (radius * CosDegree(endAngle + orientationAngle)));
		endPoint.y   = ( center.y + (radius * SinDegree(endAngle + orientationAngle)));

		DrawLine(startPoint, endPoint, defaultInkColor, defaultInkColor, defaultDrawingThickness);
	}
}

void Renderer::DrawDottedPolygon( const Vector2& center, float radius, float sides, float orientationAngle, const Rgba& color ) {
	// For loop for every line
	for(float i=0; i<sides; i+=2) {
		float startAngle = ( i * (360.f/sides) );
		float endAngle = startAngle + (360.f/sides);

		Vector2 startPoint;
		Vector2 endPoint;

		startPoint.x = ( center.x + (radius * CosDegree(startAngle + orientationAngle)));
		startPoint.y = ( center.y + (radius * SinDegree(startAngle + orientationAngle)));
		endPoint.x   = ( center.x + (radius * CosDegree(endAngle + orientationAngle)));
		endPoint.y   = ( center.y + (radius * SinDegree(endAngle + orientationAngle)));

		DrawLine(startPoint, endPoint, color, color, defaultDrawingThickness);
	}
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
	m_currentTexture = texture;
	m_secondaryTexture = secondaryTexture;

	DrawCube( center, dimensions, color, uv_top, uv_side, uv_bottom );
}

void Renderer::DrawTexturedAABBArray( const Vertex_3DPCU* vertexes, int numVertexes, const Texture& texture )
{
//	UNIMPLEMENTED();

	UNUSED( vertexes );
	UNUSED( numVertexes );
	UNUSED( texture );
	/*

	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, texture.m_textureID );

	DrawMeshImmediate( vertexes, numVertexes, PRIMITIVE_QUADS );

	glColor4ub( defaultColor.r, defaultColor.g, defaultColor.b, defaultColor.a );

	glDisable( GL_TEXTURE_2D );

	*/
}

void Renderer::DrawText2D( const Vector2& drawMins, const std::string& asciiText, float cellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */ )
{
	Vector2 newMins = drawMins;
	float cellWidth = cellHeight * font->GetGlyphAspect( asciiText.at(0) );
	Vector2 newMaxs = Vector2( drawMins.x + cellWidth , drawMins.y + cellHeight );
	AABB2 boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	// For every character of the string
	for( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		// Draw that character
		AABB2 textCoords = font->GetUVsForGlyph( asciiText.at(i) );
		DrawTexturedAABB( boundForNextCharacter , font->m_spriteSheet.m_spriteSheetTexture , textCoords.mins , textCoords.maxs, tint );

		// Calculate bounds to draw next character
		newMins = Vector2( newMins.x + cellWidth , newMins.y);
		newMaxs = Vector2( newMaxs.x + cellWidth , newMaxs.y);
		boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	}
}

void Renderer::DrawTextInBox2D( const std::string& asciiText, const Vector2& alignment, const AABB2& drawInBox, float desiredCellHeight, const Rgba& tint /* = RGBA_WHITE_COLOR */, const BitmapFont* font /* = nullptr */, TextDrawMode drawMode /* = TEXT_DRAW_OVERRUN */ )
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

void Renderer::BindTexture2D( const Texture& theTexture, unsigned int bindIndex /* = 0 */ )
{
	GLuint textureIndex = bindIndex; // to see how they tie together
	
	// Bind the sampler;
	glBindSampler( textureIndex, s_defaultSampler->GetHandle() ); 
	// Bind the texture
	glActiveTexture( GL_TEXTURE0 + textureIndex ); 
	glBindTexture( GL_TEXTURE_2D, theTexture.m_textureID ); 
}

void Renderer::DrawAABB( const AABB2& bounds, const Rgba& color ) 
{
	m_currentTexture = nullptr;

	Vertex_3DPCU verts[] = {
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), color, Vector2::ZERO ),	// Upper-left
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), color, Vector2::ZERO ),	// Upper-right
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), color, Vector2::ZERO ),	// Bottom-right

		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), color, Vector2::ZERO ),	// Bottom-right
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), color, Vector2::ZERO ),	// Bottom-left
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), color, Vector2::ZERO )	// Upper-left
	};

	DrawMeshImmediate( verts, 6, PRIMITIVE_TRIANGES );
}


void Renderer::DrawTexturedAABB( const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint ) 
{
	m_currentTexture = &texture;

	Vertex_3DPCU verts[] = {
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMaxs.y ) ),	// Upper-left
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMaxs.y ) ),	// Upper-right
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y ) ),	// Bottom-right

		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y ) ),	// Bottom-right
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMins.y ) ),	// Bottom-left
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMaxs.y ) )	// Upper-left
	};

	DrawMeshImmediate( verts, 6, PRIMITIVE_TRIANGES );
}

void Renderer::DrawTexturedAABB( const Matrix44 &transformMatrix, const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint )
{
	m_currentTexture = &texture;

	Vertex_3DPCU verts[] = {
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMaxs.y ) ),	// Upper-left
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMaxs.y ) ),	// Upper-right
		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y ) ),	// Bottom-right

		Vertex_3DPCU( Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y ) ),	// Bottom-right
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMins.y ) ),	// Bottom-left
		Vertex_3DPCU( Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3(), tint, Vector2( texCoordsAtMins.x, texCoordsAtMaxs.y ) )	// Upper-left
	};

	DrawMeshImmediate( verts, 6, PRIMITIVE_TRIANGES, transformMatrix );
}

Texture* Renderer::CreateOrGetTexture( const std::string& pathToImage ) {
	Texture* referenceToTexture = nullptr;															// to get texture from texturePool
	bool textureExistInPool = findTextureFromPool( pathToImage, referenceToTexture );				// find existing texture from texturePool

																									// if not found,
	if ( textureExistInPool == false ) {
		// create new texture and add it to the pool
		referenceToTexture = new Texture( pathToImage );

		LoadedTexturesData newTexData = LoadedTexturesData( pathToImage, referenceToTexture );
		texturePool.push_back(newTexData);
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
		 bitmapFontPool[ std::string(bitmapFontName) ] = fontToReturn;

		 // return the bitmapFont
		 return fontToReturn;
	 }
	 
 }

bool Renderer::findTextureFromPool( const std::string& pathToImage , Texture* &foundTexture ) {
	
	for(unsigned int i=0; i<texturePool.size(); i++) {

		if( pathToImage == texturePool[i].pathToImage ) {
			foundTexture = texturePool[i].texture;
			return true;
		}
	}

	return false;
}

bool Renderer::findBitmapFontFromPool( const std::string& nameOfFont , BitmapFont* &foundFont )
{
	std::map< std::string , BitmapFont* >::iterator it = bitmapFontPool.find( nameOfFont );

	if( it != bitmapFontPool.end() )
	{
		foundFont = it->second;
		return true;
	}

	return false;
}

bool Renderer::FindShaderProgramFromPool( const std::string& nameOfShaderProgram, ShaderProgram* &foundShader )
{
	std::map< std::string , ShaderProgram* >::iterator it = m_shaderProgramPool.find( nameOfShaderProgram );

	if( it != m_shaderProgramPool.end() )
	{
		foundShader = it->second;
		return true;
	}

	return false;
}

GLenum Renderer::GetAsOpenGLPrimitiveType( ePrimitiveType inPrimitive ) const
{
	return g_openGlPrimitiveTypes[ inPrimitive ];
}

GLenum Renderer::GetAsOpenGLDataType( eRenderDataType inDataType ) const
{
	return g_openGlDataTypes[ inDataType ];
}


void Renderer::GLPushMatrix() {
//	UNIMPLEMENTED();

	// glPushMatrix();
}

void Renderer::GLTranslate( float x, float y, float z) {
//	UNIMPLEMENTED();

	UNUSED( x );
	UNUSED( y );
	UNUSED( z );
	// glTranslatef(x, y, z);
}

void Renderer::GLRotate( float rotation, float x, float y, float z) {
//	UNIMPLEMENTED();

	UNUSED( rotation );
	UNUSED( x );
	UNUSED( y );
	UNUSED( z );
	// glRotatef(rotation, x, y, z);
}

void Renderer::GLScale( float x, float y, float z) {
// 	UNIMPLEMENTED();

	UNUSED( x );
	UNUSED( y );
	UNUSED( z );
	// glScalef(x, y, z);
}

void Renderer::GLPopMatrix() {
//	UNIMPLEMENTED();

	// glPopMatrix();
}

Vector3 Renderer::GetDrawPositionUsingAnchorPoint( const Vector3& position, const Vector3& dimensions, const Vector3& anchorPoint /* = Vector3::ZERO */ )
{
	Vector3 halfDimensions = dimensions * 0.5f;
	Vector3 anchorPoin_X_halfDimensions = Vector3( anchorPoint.x * halfDimensions.x, anchorPoint.y * halfDimensions.y, anchorPoint.z * halfDimensions.z );
	return position - anchorPoin_X_halfDimensions;
}

void Renderer::GLBlendChangeBeforeAnimation() {
//	UNIMPLEMENTED();

	// glBlendFunc( GL_SRC_ALPHA, GL_ONE );
}

void Renderer::GLBlendRestoreAfterAnimation() {
//	UNIMPLEMENTED();

	// glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

GLenum Renderer::ToGLCompare( eCompare compare ) 
{
	// Convert our engine to GL enum 
	switch ( compare )
	{
	case COMPARE_NEVER:
		return GL_NEVER;
		break;
	case COMPARE_LESS:
		return GL_LESS;
		break;
	case COMPARE_LEQUAL:
		return GL_LEQUAL;
		break;
	case COMPARE_GREATER:
		return GL_GREATER;
		break;
	case COMPARE_GEQUAL:
		return GL_GEQUAL;
		break;
	case COMPARE_EQUAL:
		return GL_EQUAL;
		break;
	case COMPARE_NOT_EQUAL:
		return GL_NOTEQUAL;
		break;
	case COMPARE_ALWAYS:
		return GL_ALWAYS;
		break;
	}

	// It should not reach here..
	return GL_NEVER;
}

//------------------------------------------------------------------------
void Renderer::EnableDepth( eCompare compare, bool should_write )
{
	// enable/disable the test
	glEnable( GL_DEPTH_TEST ); 
	glDepthFunc( ToGLCompare(compare) ); 

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
	glClearDepthf( depth );
	glClear( GL_DEPTH_BUFFER_BIT ); 
}

void Renderer::SetCurrentCameraTo( Camera* newCamera )
{
	s_current_camera = newCamera;

	if( s_current_camera != nullptr )
		s_current_camera->Finalize();
	else
		s_default_camera->Finalize();
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

Texture* Renderer::GetDefaultColorTarget()
{
	return s_defaultColorTarget;
}

Texture* Renderer::GetDefaultDepthTarget()
{
	return s_defaultDepthTarget;
}