#pragma once
#include "DebugRenderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Core/DevConsole.hpp"

std::vector< DebugRenderObject* >	DebugRenderer::s_renderObjectQueue;

DebugRenderer::DebugRenderer( Renderer *activeRenderer, Camera *camera3D )
	: m_renderer( *activeRenderer )
	, m_camera3D( camera3D )
{
	m_camera2D = new Camera();

	m_camera2D->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera2D->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_camera2D->SetProjectionOrtho( (float) Window::GetInstance()->GetHeight(), -1.f, 1.f );

	m_font = m_renderer.CreateOrGetBitmapFont( "SquirrelFixedFont" );

	// Command Register
	CommandRegister( "clear_debug", DebugRenderer::ClearAllRenderingObjects );
}

DebugRenderer::~DebugRenderer()
{
	delete m_camera2D;
	m_camera2D = nullptr;

	EmptyTheRenderObjectQueue();
}

void DebugRenderer::BeginFrame()
{

}

void DebugRenderer::EndFrame()
{
	DeleteOverdueRenderObjects();
}

void DebugRenderer::Update( float deltaSeconds )
{
	for( DebugRenderObject* renderObject : s_renderObjectQueue )
		renderObject->Update( deltaSeconds );
}

void DebugRenderer::Render() const
{
	for( DebugRenderObject* renderObject : s_renderObjectQueue )
		renderObject->Render();
}

void DebugRenderer::ClearAllRenderingObjects( Command& cmd )
{
	UNUSED( cmd );

	while ( s_renderObjectQueue.size() > 0)
	{
		unsigned int lastIndex = (unsigned int)s_renderObjectQueue.size() - 1;

		delete s_renderObjectQueue[ lastIndex ];
		s_renderObjectQueue.pop_back();
	}
}

void DebugRenderer::DeleteOverdueRenderObjects()
{
	// Check if RenderObject is ready to be deleted..
	for( unsigned int i = 0; i < s_renderObjectQueue.size(); i++ )
	{
		if( s_renderObjectQueue[i]->m_deleteMe == true )
		{
			// Smart delete
			delete s_renderObjectQueue[i];
			s_renderObjectQueue[i] = nullptr;

			// Swap the last one with current one
			unsigned int lastIdx	= (unsigned int)s_renderObjectQueue.size() - 1;
			s_renderObjectQueue[i]	= s_renderObjectQueue[ lastIdx ];

			// Erase the last one
			s_renderObjectQueue.pop_back();
			i--;
		}
	}
}

void DebugRenderer::EmptyTheRenderObjectQueue()
{
	while( s_renderObjectQueue.size() > 0)
	{
		unsigned int sizeOfQueue = (unsigned int)s_renderObjectQueue.size();
		
		delete s_renderObjectQueue[ sizeOfQueue - 1 ];
		s_renderObjectQueue.pop_back();
	}
}

void DebugRenderer::AddTexturedAABBToMeshBuilder( MeshBuilder& mb, AABB2 const &bounds, Vector2 const &texCoordsAtMins, Vector2 const &texCoordsAtMaxs, Rgba const &tintColor )
{
	Vector3 bottomLeftPos	= Vector2( bounds.mins.x, bounds.mins.y ).GetAsVector3();
	Vector2 bottomLeftUVs	= Vector2( texCoordsAtMins.x, texCoordsAtMins.y );

	Vector3 bottomRightPos	= Vector2( bounds.maxs.x, bounds.mins.y ).GetAsVector3();
	Vector2 bottomRightUVs	= Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y );

	Vector3 upperRightPos	= Vector2( bounds.maxs.x, bounds.maxs.y ).GetAsVector3();
	Vector2 upperRightUVs	= Vector2( texCoordsAtMaxs.x, texCoordsAtMaxs.y );

	Vector3 upperLeftPos	= Vector2( bounds.mins.x, bounds.maxs.y ).GetAsVector3();
	Vector2 upperLeftUVs	= Vector2( texCoordsAtMins.x, texCoordsAtMaxs.y );

	// Bottom Triangle
	mb.SetColor( tintColor );
	mb.SetUV( bottomLeftUVs );
	mb.PushVertex( bottomLeftPos );

	mb.SetColor( tintColor );
	mb.SetUV( bottomRightUVs );
	mb.PushVertex( bottomRightPos );
	
	mb.SetColor( tintColor );
	mb.SetUV( upperRightUVs );
	mb.PushVertex( upperRightPos );

	// Upper Triangle
	mb.SetColor( tintColor );
	mb.SetUV( upperRightUVs );
	mb.PushVertex( upperRightPos );

	mb.SetColor( tintColor );
	mb.SetUV( upperLeftUVs );
	mb.PushVertex( upperLeftPos);

	mb.SetColor( tintColor );
	mb.SetUV( bottomLeftUVs );
	mb.PushVertex( bottomLeftPos );
}

void DebugRenderer::DebugRender2DQuad( float lifetime, AABB2 const &bounds, Rgba const &startColor, Rgba const &endColor )
{
	Vector3 const upperLeft		( bounds.mins.x, bounds.maxs.y, 0.f );
	Vector3 const upperRight	( bounds.maxs.x, bounds.maxs.y, 0.f );
	Vector3 const bottomRight	( bounds.maxs.x, bounds.mins.y, 0.f );
	Vector3 const bottomLeft	( bounds.mins.x, bounds.mins.y, 0.f );

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( bottomLeft );

	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( bottomRight );

	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( upperRight );
	
	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( upperLeft );

	mb.AddFace( 0, 1, 2 );
	mb.AddFace( 2, 3, 0 );
	mb.End();
	
	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, m_renderer, *m_camera2D, Matrix44(), mb.ConstructMesh(), nullptr, startColor, endColor );
	s_renderObjectQueue.push_back( quadObject );
}

void DebugRenderer::DebugRender2DLine( float lifetime, Vector2 const &p0, Rgba const &p0Color, Vector2 const &p1, Rgba const &p1Color, Rgba const &tintStartColor, Rgba const &tintEndColor )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );
	mb.SetColor( p0Color );
	mb.PushVertex( p0.GetAsVector3() );

	mb.SetColor( p1Color );
	mb.PushVertex( p1.GetAsVector3() );
	mb.End();

	DebugRenderObject *lineObject = new DebugRenderObject( lifetime, m_renderer, *m_camera2D, Matrix44(), mb.ConstructMesh(), nullptr, tintStartColor, tintEndColor );
	s_renderObjectQueue.push_back( lineObject );
}

void DebugRenderer::DebugRender2DText( float lifetime, Vector2 const &position, float const height, Rgba const &startColor, Rgba const &endColor, std::string asciiText )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, false );

	Vector2 newMins = position;
	float cellWidth = height * m_font->GetGlyphAspect( asciiText.at(0) );
	Vector2 newMaxs = Vector2( position.x + cellWidth , position.y + height );
	AABB2 boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	// For every character of the string
	for( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		// Draw that character
		AABB2 textCoords = m_font->GetUVsForGlyph( asciiText.at(i) );
		AddTexturedAABBToMeshBuilder( mb, boundForNextCharacter, textCoords.mins , textCoords.maxs, RGBA_WHITE_COLOR );

		// Calculate bounds to draw next character
		newMins = Vector2( newMins.x + cellWidth , newMins.y);
		newMaxs = Vector2( newMaxs.x + cellWidth , newMaxs.y);
		boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	}

	mb.End();
	DebugRenderObject *textObject = new DebugRenderObject( lifetime, m_renderer, *m_camera2D, Matrix44(), mb.ConstructMesh(), &m_font->m_spriteSheet.m_spriteSheetTexture, startColor, endColor );
	s_renderObjectQueue.push_back( textObject );
}

void DebugRenderer::DebugRenderPoint( float lifetime, Vector3 const &position, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_POINTS, false );
	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( position );
	mb.End();

	DebugRenderObject *pointObject = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, Matrix44(), mb.ConstructMesh(), nullptr, startColor, endColor, mode );
	s_renderObjectQueue.push_back( pointObject );
}

void DebugRenderer::DebugRenderLineSegment( float lifetime, Vector3 const &p0, Rgba const &p0Color, Vector3 const &p1, Rgba const &p1Color, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );
	mb.SetColor( p0Color );
	mb.PushVertex( p0 );

	mb.SetColor( p1Color );
	mb.PushVertex( p1 );
	mb.End();

	DebugRenderObject *lineObject = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, Matrix44(), mb.ConstructMesh(), nullptr, startColor, endColor, mode );
	s_renderObjectQueue.push_back( lineObject );
}

void DebugRenderer::DebugRenderBasis( float lifetime, Matrix44 const &basis, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Vector3 worldPosition = basis.GetTColumn();

	MeshBuilder mb;

	mb.Begin( PRIMITIVE_LINES, false );
	mb.SetColor		( RGBA_RED_COLOR );
	mb.PushVertex	( worldPosition );
	mb.SetColor		( RGBA_RED_COLOR );
	mb.PushVertex	( worldPosition + basis.GetIColumn() );


	mb.SetColor		( RGBA_GREEN_COLOR );
	mb.PushVertex	( worldPosition );
	mb.SetColor		( RGBA_GREEN_COLOR );
	mb.PushVertex	( worldPosition + basis.GetJColumn() );


	mb.SetColor		( RGBA_BLUE_COLOR );
	mb.PushVertex	( worldPosition );
	mb.SetColor		( RGBA_BLUE_COLOR );
	mb.PushVertex	( worldPosition + basis.GetKColumn() );
	mb.End();

	DebugRenderObject *basisLines = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, Matrix44(), mb.ConstructMesh(), nullptr, startColor, endColor, mode );
	s_renderObjectQueue.push_back( basisLines );
}

void DebugRenderer::DebugRenderSphere( float lifetime, Vector3 const &pos, float const radius, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Mesh* wireSphereMesh = MeshBuilder::CreateSphere( radius, 10, 6, pos );
	DebugRenderObject *wireSphereObject = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, Matrix44(), wireSphereMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_FILL );
	s_renderObjectQueue.push_back( wireSphereObject );
}

void DebugRenderer::DebugRenderWireSphere( float lifetime, Vector3 const &pos, float const radius, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Mesh* wireSphereMesh = MeshBuilder::CreateSphere( radius, 10, 6, pos );
	DebugRenderObject *wireSphereObject = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, Matrix44(), wireSphereMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_LINE );
	s_renderObjectQueue.push_back( wireSphereObject );
}

void DebugRenderer::DebugRenderWireCube( float lifetime, Vector3 const &bottomLeftFront, Vector3 const &topRightBack, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Vector3 center	= ( bottomLeftFront + topRightBack ) * 0.5f;
	Vector3 size	= Vector3( abs(topRightBack.x - bottomLeftFront.x), abs(topRightBack.y - bottomLeftFront.y), abs(topRightBack.z - bottomLeftFront.z) );
	Mesh* cubeMesh	= MeshBuilder::CreateCube( size, center );
	
	DebugRenderObject *wireCubeObject = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, Matrix44(), cubeMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_LINE );
	s_renderObjectQueue.push_back( wireCubeObject );
}

void DebugRenderer::DebugRenderQuad( float lifetime, Vector3 const &pos, Vector3 const &eulerRotation, Vector2 const &xySize, Texture *texture, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Transform modelTransform = Transform( pos, eulerRotation, Vector3::ONE_ALL );
	Mesh* quadMesh	= MeshBuilder::CreatePlane( xySize, pos );

	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, m_renderer, *m_camera3D, modelTransform.GetTransformMatrix(), quadMesh, texture, startColor, endColor, mode, FRONT_AND_BACK_FILL );
	s_renderObjectQueue.push_back( quadObject );
}