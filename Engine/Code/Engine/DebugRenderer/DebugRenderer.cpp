#pragma once
#include "DebugRenderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Core/DevConsole.hpp"


Renderer									*debugRenderer				= nullptr;
Camera										*debugCamera3D				= nullptr;
Camera										*debugCamera2D				= nullptr;
BitmapFont									*debugFont					= nullptr;
std::vector< DebugRenderObject* >			 debugRenderObjectQueue;


void EmptyTheRenderObjectQueue();
void DeleteOverdueRenderObjects();

void DebugRendererStartup( Renderer *activeRenderer, Camera *camera3D )
{
	debugRenderer = activeRenderer;
	debugCamera3D = camera3D;
	debugCamera2D = new Camera();

	debugCamera2D->SetColorTarget( Renderer::GetDefaultColorTarget() );
	debugCamera2D->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	debugCamera2D->SetProjectionOrtho( (float) Window::GetInstance()->GetHeight(), -1.f, 1.f );

	debugFont = debugRenderer->CreateOrGetBitmapFont( "SquirrelFixedFont" );

	// Command Register
	CommandRegister( "clear_debug", ClearAllRenderingObjects );
}

void DebugRendererShutdown()
{
	delete debugCamera2D;
	debugCamera2D = nullptr;

	EmptyTheRenderObjectQueue();
}

void DebugRendererUpdate( float deltaSeconds )
{
	DeleteOverdueRenderObjects();

	for( DebugRenderObject* renderObject : debugRenderObjectQueue )
		renderObject->Update( deltaSeconds );
}

void DebugRendererRender()
{
	for( DebugRenderObject* renderObject : debugRenderObjectQueue )
		renderObject->Render();
}

void ClearAllRenderingObjects( Command& cmd )
{
	UNUSED( cmd );

	while ( debugRenderObjectQueue.size() > 0)
	{
		unsigned int lastIndex = (unsigned int)debugRenderObjectQueue.size() - 1;

		delete debugRenderObjectQueue[ lastIndex ];
		debugRenderObjectQueue.pop_back();
	}
}

void DeleteOverdueRenderObjects()
{
	// Check if RenderObject is ready to be deleted..
	for( unsigned int i = 0; i < debugRenderObjectQueue.size(); i++ )
	{
		if( debugRenderObjectQueue[i]->m_deleteMe == true )
		{
			// Smart delete
			delete debugRenderObjectQueue[i];
			debugRenderObjectQueue[i] = nullptr;

			// Swap the last one with current one
			unsigned int lastIdx	= (unsigned int)debugRenderObjectQueue.size() - 1;
			debugRenderObjectQueue[i]	= debugRenderObjectQueue[ lastIdx ];

			// Erase the last one
			debugRenderObjectQueue.pop_back();
			i--;
		}
	}
}

void EmptyTheRenderObjectQueue()
{
	while( debugRenderObjectQueue.size() > 0)
	{
		unsigned int sizeOfQueue = (unsigned int)debugRenderObjectQueue.size();
		
		delete debugRenderObjectQueue[ sizeOfQueue - 1 ];
		debugRenderObjectQueue.pop_back();
	}
}

void AddTexturedAABBToMeshBuilder( MeshBuilder& mb, AABB2 const &bounds, Vector2 const &texCoordsAtMins, Vector2 const &texCoordsAtMaxs, Rgba const &tintColor )
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

void DebugRender2DQuad( float lifetime, AABB2 const &bounds, Rgba const &startColor, Rgba const &endColor )
{
	Vector3 const upperLeft		( bounds.mins.x, bounds.maxs.y, 0.f );
	Vector3 const upperRight	( bounds.maxs.x, bounds.maxs.y, 0.f );
	Vector3 const bottomRight	( bounds.maxs.x, bounds.mins.y, 0.f );
	Vector3 const bottomLeft	( bounds.mins.x, bounds.mins.y, 0.f );
	Vector3 const centerPos		( (upperRight + bottomLeft) * 0.5f );

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( bottomLeft - centerPos );

	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( bottomRight - centerPos );

	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( upperRight - centerPos );
	
	mb.SetColor( RGBA_WHITE_COLOR );
	mb.PushVertex( upperLeft - centerPos );

	mb.AddFace( 0, 1, 2 );
	mb.AddFace( 2, 3, 0 );
	mb.End();
	
	Transform modelTransform = Transform( centerPos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera2D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( quadObject );
}

void DebugRender2DLine( float lifetime, Vector2 const &p0, Rgba const &p0Color, Vector2 const &p1, Rgba const &p1Color, Rgba const &tintStartColor, Rgba const &tintEndColor )
{
	Vector2 centerPos = ( p0 + p1 ) * 0.5f;

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );
	mb.SetColor( p0Color );
	mb.PushVertex( (p0 - centerPos).GetAsVector3() );

	mb.SetColor( p1Color );
	mb.PushVertex( (p1 - centerPos).GetAsVector3() );
	mb.End();

	Transform modelTransform = Transform( centerPos.GetAsVector3(), Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *lineObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera2D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, tintStartColor, tintEndColor, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( lineObject );
}

void DebugRender2DText( float lifetime, Vector2 const &position, float const height, Rgba const &startColor, Rgba const &endColor, std::string asciiText )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, false );

	Vector2 newMins = Vector2::ZERO;
	float cellWidth = height * debugFont->GetGlyphAspect( asciiText.at(0) );
	Vector2 newMaxs = Vector2( cellWidth , height );
	AABB2 boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	// For every character of the string
	for( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		// Draw that character
		AABB2 textCoords = debugFont->GetUVsForGlyph( asciiText.at(i) );
		AddTexturedAABBToMeshBuilder( mb, boundForNextCharacter, textCoords.mins , textCoords.maxs, RGBA_WHITE_COLOR );

		// Calculate bounds to draw next character
		newMins = Vector2( newMins.x + cellWidth , newMins.y);
		newMaxs = Vector2( newMaxs.x + cellWidth , newMaxs.y);
		boundForNextCharacter = AABB2( newMins.x , newMins.y , newMaxs.x , newMaxs.y );
	}

	mb.End();

	Transform modelTransform = Transform( position.GetAsVector3(), Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *textObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera2D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), &debugFont->m_spriteSheet.m_spriteSheetTexture, startColor, endColor, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( textObject );
}

void DebugRenderPoint( float lifetime, float size, Vector3 const &position, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	float halfSize = size * 0.5f;

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );

	mb.PushVertex( Vector3( 0.f,  halfSize, 0.f ) );
	mb.PushVertex( Vector3( 0.f, -halfSize, 0.f ) );

	mb.PushVertex( Vector3(  halfSize, 0.f, 0.f ) );
	mb.PushVertex( Vector3( -halfSize, 0.f, 0.f ) );

	mb.PushVertex( Vector3( 0.f, 0.f,  halfSize ) );
	mb.PushVertex( Vector3( 0.f, 0.f, -halfSize ) );
	mb.End();

	Transform modelTransform = Transform( position, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *pointObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( pointObject );
}

void DebugRenderLineSegment( float lifetime, Vector3 const &p0, Rgba const &p0Color, Vector3 const &p1, Rgba const &p1Color, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Vector3 centerPos = ( p0 + p1 ) * 0.5f;

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );
	mb.SetColor( p0Color );
	mb.PushVertex( p0 - centerPos );

	mb.SetColor( p1Color );
	mb.PushVertex( p1 - centerPos );
	mb.End();

	Transform modelTransform = Transform( centerPos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *lineObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( lineObject );
}

void DebugRenderBasis( float lifetime, Matrix44 const &basis, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Vector3 localPosition = Vector3::ZERO;

	MeshBuilder mb;

	mb.Begin( PRIMITIVE_LINES, false );
	mb.SetColor		( RGBA_RED_COLOR );
	mb.PushVertex	( localPosition );
	mb.SetColor		( RGBA_RED_COLOR );
	mb.PushVertex	( localPosition + basis.GetIColumn() );


	mb.SetColor		( RGBA_GREEN_COLOR );
	mb.PushVertex	( localPosition );
	mb.SetColor		( RGBA_GREEN_COLOR );
	mb.PushVertex	( localPosition + basis.GetJColumn() );


	mb.SetColor		( RGBA_BLUE_COLOR );
	mb.PushVertex	( localPosition );
	mb.SetColor		( RGBA_BLUE_COLOR );
	mb.PushVertex	( localPosition + basis.GetKColumn() );
	mb.End();

	Transform modelTransform = Transform( basis.GetTColumn(), Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *basisLines = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( basisLines );
}

void DebugRenderSphere( float lifetime, Vector3 const &pos, float const radius, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Mesh* wireSphereMesh = MeshBuilder::CreateSphere( radius, 10, 6, Vector3::ZERO );

	Transform modelTransform = Transform( pos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *wireSphereObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), wireSphereMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_FILL );
	debugRenderObjectQueue.push_back( wireSphereObject );
}

void DebugRenderWireSphere( float lifetime, Vector3 const &pos, float const radius, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Mesh* wireSphereMesh = MeshBuilder::CreateSphere( radius, 10, 6, Vector3::ZERO );

	Transform modelTransform = Transform( pos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *wireSphereObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), wireSphereMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_LINE );
	debugRenderObjectQueue.push_back( wireSphereObject );
}

void DebugRenderWireCube( float lifetime, Vector3 const &bottomLeftFront, Vector3 const &topRightBack, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Vector3 center	= ( bottomLeftFront + topRightBack ) * 0.5f;
	Vector3 size	= Vector3( abs(topRightBack.x - bottomLeftFront.x), abs(topRightBack.y - bottomLeftFront.y), abs(topRightBack.z - bottomLeftFront.z) );
	Mesh* cubeMesh	= MeshBuilder::CreateCube( size, Vector3::ZERO );
	
	Transform modelTransform = Transform( center, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *wireCubeObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), cubeMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_LINE );
	debugRenderObjectQueue.push_back( wireCubeObject );
}

void DebugRenderQuad( float lifetime, Vector3 const &pos, Vector3 const &eulerRotation, Vector2 const &xySize, Texture *texture, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Transform modelTransform = Transform( pos, eulerRotation, Vector3::ONE_ALL );
	Mesh* quadMesh	= MeshBuilder::CreatePlane( xySize, Vector3::ZERO );

	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, *debugRenderer, *debugCamera3D, modelTransform.GetTransformMatrix(), quadMesh, texture, startColor, endColor, mode, FRONT_AND_BACK_FILL );
	debugRenderObjectQueue.push_back( quadObject );
}