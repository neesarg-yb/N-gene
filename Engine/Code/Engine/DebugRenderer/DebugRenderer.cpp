#pragma once
#include "DebugRenderer.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Input/Command.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Profiler/Profiler.hpp"


Renderer									*debugRenderer				= nullptr;
Camera										*debugCamera2D				= nullptr;
BitmapFont									*debugFont					= nullptr;
std::vector< DebugRenderObject* >			 debugRenderObjectQueue;


void EmptyTheRenderObjectQueue();
void DeleteOverdueRenderObjects();

void DebugRendererStartup( Renderer *activeRenderer )
{
	debugRenderer = activeRenderer;
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

void DebugRendererBeginFrame( Clock const *clock )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
	
	Clock const *activeClock	= (clock != nullptr) ? clock : GetMasterClock();
	float const  deltaSeconds	= (float) activeClock->frame.seconds;

	for( DebugRenderObject* renderObject : debugRenderObjectQueue )
		renderObject->Update( deltaSeconds );

	DeleteOverdueRenderObjects();
}

void DebugRendererLateRender( Camera *camera3D )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	for( DebugRenderObject* renderObject : debugRenderObjectQueue )
	{
		Camera &activeCamera = (renderObject->m_cameraType == DEBUG_CAMERA_2D) ? *debugCamera2D : *camera3D;
		renderObject->Render( activeCamera );
	}
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
	uint deletedObjects = 0U;
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

			deletedObjects++;
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

void AddTexturedAABBToMeshBuilder( MeshBuilder& mb, Vector3 const &mins, float height ,float cellWidth, Vector3 upDirection, Vector3 rightDirection, Vector2 const &texCoordsAtMins, Vector2 const &texCoordsAtMaxs, Rgba const &tintColor )
{
	Vector3 bottomLeftPos	= mins;
	Vector2 bottomLeftUVs	= Vector2( texCoordsAtMins.x, texCoordsAtMins.y );

	Vector3 bottomRightPos	= bottomLeftPos + ( rightDirection * cellWidth );
	Vector2 bottomRightUVs	= Vector2( texCoordsAtMaxs.x, texCoordsAtMins.y );

	Vector3 upperRightPos	= bottomRightPos + ( upDirection * height );
	Vector2 upperRightUVs	= Vector2( texCoordsAtMaxs.x, texCoordsAtMaxs.y );

	Vector3 upperLeftPos	= bottomLeftPos + ( upDirection * height );
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
	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_2D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, DEBUG_RENDER_IGNORE_DEPTH );
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
	DebugRenderObject *lineObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_2D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, tintStartColor, tintEndColor, DEBUG_RENDER_IGNORE_DEPTH );
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
	DebugRenderObject *textObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_2D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), &debugFont->m_spriteSheet.m_spriteSheetTexture, startColor, endColor, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( textObject );
}

FloatRange DebugRenderXYCurve( float lifetime, AABB2 const &drawBounds, xyCurve_cb curveCB, FloatRange xRange, float step, Rgba const &curveColor, Rgba const &backgroundColor, Rgba const &gridlineColor )
{
	Vector3 const upperLeft	 ( drawBounds.mins.x, drawBounds.maxs.y, 0.f );
	Vector3 const upperRight ( drawBounds.maxs.x, drawBounds.maxs.y, 0.f );
	Vector3 const bottomRight( drawBounds.maxs.x, drawBounds.mins.y, 0.f );
	Vector3 const bottomLeft ( drawBounds.mins.x, drawBounds.mins.y, 0.f );
	
	//-------------------------
 	// Draw Background - Quad
 	MeshBuilder mb;
 	mb.Begin( PRIMITIVE_TRIANGES, true );
 	mb.SetColor( backgroundColor );
 	mb.PushVertex( bottomLeft );
 
 	mb.SetColor( backgroundColor );
 	mb.PushVertex( bottomRight );
 
 	mb.SetColor( backgroundColor );
 	mb.PushVertex( upperRight );
 
 	mb.SetColor( backgroundColor );
 	mb.PushVertex( upperLeft );
 
 	mb.AddFace( 0, 1, 2 );
 	mb.AddFace( 2, 3, 0 );
 	mb.End();
 
 	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_2D, Matrix44(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
 	debugRenderObjectQueue.push_back( quadObject );


	//-------------------------
	// Get points on the curve
	std::vector< Vector2 > pointsOnCurve;
	Vector2 firstPoint;
	firstPoint.x = xRange.min;
	firstPoint.y = curveCB( firstPoint.x );
	pointsOnCurve.push_back( firstPoint );

	FloatRange yRange = FloatRange( firstPoint.y, firstPoint.y );	// Min and Max

	for( float xVal = xRange.min; xVal <= xRange.max; xVal += step )
	{
		Vector2 thisPoint;
		thisPoint.x = xVal;
		thisPoint.y = curveCB( thisPoint.x );
		pointsOnCurve.push_back( thisPoint );

		yRange.min = (yRange.min < thisPoint.y) ? yRange.min : thisPoint.y;
		yRange.max = (yRange.max > thisPoint.y) ? yRange.max : thisPoint.y;
	}

	Vector2 lastPoint;
	lastPoint.x = xRange.max;
	lastPoint.y = curveCB( lastPoint.x );
	pointsOnCurve.push_back( lastPoint );

	yRange.min = (yRange.min < lastPoint.y) ? yRange.min : lastPoint.y;
	yRange.max = (yRange.max > lastPoint.y) ? yRange.max : lastPoint.y;


	//-----------
	// Draw Grid
	mb = MeshBuilder();
	mb.Begin( PRIMITIVE_LINES, false );

	// Lines parallel to Y-Axis
	float gridWidth = (xRange.max - xRange.min) / 10.f;
	for( float x = xRange.min; x <= xRange.max; x += gridWidth )
	{
		float xOnQuad = RangeMapFloat( x, xRange.min, xRange.max, drawBounds.mins.x, drawBounds.maxs.x );
		Vector3 bottomEnd = Vector3( xOnQuad, drawBounds.mins.y, 0.45f );
		Vector3 topEnd    = Vector3( xOnQuad, drawBounds.maxs.y, 0.45f );

		mb.SetColor( gridlineColor );
		mb.PushVertex( bottomEnd );
		mb.SetColor( gridlineColor );
		mb.PushVertex( topEnd );
	}

	// Lines parallel to X-Axis
	for( float y = yRange.min; y <= yRange.max; y += gridWidth )
	{
		float yOnQuad = RangeMapFloat( y, yRange.min, yRange.max, drawBounds.mins.y, drawBounds.maxs.y );
		Vector3 leftEnd  = Vector3( drawBounds.mins.x, yOnQuad, 0.045f );
		Vector3 rightEnd = Vector3( drawBounds.maxs.x, yOnQuad, 0.045f );

		mb.SetColor( gridlineColor );
		mb.PushVertex( leftEnd );
		mb.SetColor( gridlineColor );
		mb.PushVertex( rightEnd );
	}

	mb.End();
	DebugRenderObject *gridsObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_2D, Matrix44(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( gridsObject );
	
	// Draw the Curve
	mb = MeshBuilder();
	mb.Begin( PRIMITIVE_LINES, false );

	for( int i = 1; i < pointsOnCurve.size(); i++ )
	{
		Vector2 prevPoint = pointsOnCurve[i-1];
		Vector2 thisPoint = pointsOnCurve[i];

		Vector3 prevPointXYZ;
		prevPointXYZ.x = RangeMapFloat( prevPoint.x, xRange.min, xRange.max, drawBounds.mins.x, drawBounds.maxs.x );
		prevPointXYZ.y = RangeMapFloat( prevPoint.y, yRange.min, yRange.max, drawBounds.mins.y, drawBounds.maxs.y );
		prevPointXYZ.z = 0.4f;

		mb.SetColor( curveColor );
		mb.PushVertex( prevPointXYZ );

		Vector3 thisPointXYZ;
		thisPointXYZ.x = RangeMapFloat( thisPoint.x, xRange.min, xRange.max, drawBounds.mins.x, drawBounds.maxs.x );
		thisPointXYZ.y = RangeMapFloat( thisPoint.y, yRange.min, yRange.max, drawBounds.mins.y, drawBounds.maxs.y );
		thisPointXYZ.z = 0.4f;

		mb.SetColor( curveColor );
		mb.PushVertex( thisPointXYZ );
	}

	mb.End();
	DebugRenderObject *curveObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_2D, Matrix44(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( curveObject );

	return yRange;
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
	DebugRenderObject *pointObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
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
	DebugRenderObject *lineObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( lineObject );
}

void DebugRenderVector( float lifetime, Vector3 const &origin, Vector3 const &vector, Rgba const &color, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	float		length		 = vector.GetLength();
	Matrix44	lookAt		 = Matrix44::MakeLookAtView( vector, Vector3::ZERO );
	Quaternion	rotation	 = Quaternion::FromMatrix( lookAt ).GetInverse();

	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );

	// Body
	mb.SetColor( color );
	mb.PushVertex( Vector3::ZERO );
	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, 0.f, 1.f) );

	// Arrow(s)
	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, 0.f, 1.f) );
	mb.SetColor( color );
	mb.PushVertex( Vector3(-0.1f, 0.f, 0.9f) );
	
	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, 0.f, 1.f) );
	mb.SetColor( color );
	mb.PushVertex( Vector3(0.1f, 0.f, 0.9f) );

	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, 0.f, 1.f) );
	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, -0.1f, 0.9f) );

	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, 0.f, 1.f) );
	mb.SetColor( color );
	mb.PushVertex( Vector3(0.f, 0.1f, 0.9f) );
	
	mb.End();

	Transform modelTransform = Transform( origin, rotation, Vector3( 1.f, 1.f, length ) );
	DebugRenderObject *vectorObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh<Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( vectorObject );
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
	DebugRenderObject *basisLines = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( basisLines );
}

void DebugRenderSphere( float lifetime, Vector3 const &pos, float const radius, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Mesh* wireSphereMesh = MeshBuilder::CreateSphere( radius, 10, 6, Vector3::ZERO );

	Transform modelTransform = Transform( pos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *wireSphereObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), wireSphereMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_FILL );
	debugRenderObjectQueue.push_back( wireSphereObject );
}

void DebugRenderWireSphere( float lifetime, Vector3 const &pos, float const radius, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode const mode )
{
	Mesh* wireSphereMesh = MeshBuilder::CreateSphere( radius, 10, 6, Vector3::ZERO );

	Transform modelTransform = Transform( pos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *wireSphereObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), wireSphereMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_LINE );
	debugRenderObjectQueue.push_back( wireSphereObject );
}

void DebugRenderWireCube( float lifetime, Vector3 const &bottomLeftFront, Vector3 const &topRightBack, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Vector3 center	= ( bottomLeftFront + topRightBack ) * 0.5f;
	Vector3 size	= Vector3( abs(topRightBack.x - bottomLeftFront.x), abs(topRightBack.y - bottomLeftFront.y), abs(topRightBack.z - bottomLeftFront.z) );
	Mesh* cubeMesh	= MeshBuilder::CreateCube( size, Vector3::ZERO );
	
	Transform modelTransform = Transform( center, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *wireCubeObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), cubeMesh, nullptr, startColor, endColor, mode, FRONT_AND_BACK_LINE );
	debugRenderObjectQueue.push_back( wireCubeObject );
}

void DebugRenderQuad( float lifetime, Vector3 const &pos, Vector3 const &eulerRotation, Vector2 const &xySize, Texture *texture, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Transform modelTransform = Transform( pos, eulerRotation, Vector3::ONE_ALL );
	Mesh* quadMesh	= MeshBuilder::CreatePlane( xySize, Vector3::ZERO );

	DebugRenderObject *quadObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), quadMesh, texture, startColor, endColor, mode, FRONT_AND_BACK_FILL );
	debugRenderObjectQueue.push_back( quadObject );
}

void DebugRenderTag( float lifetime, float const height, Vector3 const &startPos, Vector3 const &upDirection, Vector3 const &rightDirection, Rgba const &startColor, Rgba const &endColor, std::string asciiText )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, false );

	Vector3 newMins = Vector3::ZERO;
	float cellWidth = height * debugFont->GetGlyphAspect( asciiText.at(0) );
	// For every character of the string
	for( unsigned int i = 0; i < asciiText.length(); i++ )
	{
		// Draw that character
		AABB2 textCoords = debugFont->GetUVsForGlyph( asciiText.at(i) );
		AddTexturedAABBToMeshBuilder( mb, newMins, height, cellWidth, upDirection, rightDirection, textCoords.mins , textCoords.maxs, RGBA_WHITE_COLOR );

		// Calculate bounds to draw next character
		newMins = newMins + (rightDirection * cellWidth);
	}

	mb.End();

	Transform modelTransform = Transform( startPos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *textObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), &debugFont->m_spriteSheet.m_spriteSheetTexture, startColor, endColor, DEBUG_RENDER_IGNORE_DEPTH );
	debugRenderObjectQueue.push_back( textObject );
}

void DebugRenderRaycast( float lifetime, Vector3 const &startPosition, RaycastResult const &raycastResult, float const impactPointSize, Rgba const &colorOnImpact, Rgba const &colorOnNoImpact, Rgba const &impactPositionColor, Rgba const &impactNormalColor, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );

	// Draw the Ray
	Vector3		localEndPosition = raycastResult.impactPosition - startPosition;
	Rgba const &rayColor		 = raycastResult.didImpact ? colorOnImpact : colorOnNoImpact;

	// Up until the impact point
	mb.SetColor( rayColor );
	mb.PushVertex( Vector3::ZERO );

	mb.SetColor( rayColor );
	mb.PushVertex( localEndPosition );

	// Draw other info. only on impact
	if( raycastResult.didImpact )
	{
		// Impact Position
		Vector3	impactPointPosLocal	= localEndPosition;
		float	halfSize			= impactPointSize * 0.5f;
		
		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal + Vector3( 0.f,  halfSize, 0.f ) );
		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal + Vector3( 0.f, -halfSize, 0.f ) );

		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal + Vector3(  halfSize, 0.f, 0.f ) );
		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal + Vector3( -halfSize, 0.f, 0.f ) );

		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal + Vector3( 0.f, 0.f,  halfSize ) );
		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal + Vector3( 0.f, 0.f, -halfSize ) );

		// Impact Normal
		mb.SetColor( impactPositionColor );
		mb.PushVertex( impactPointPosLocal );

		mb.SetColor( impactNormalColor );
		mb.PushVertex( impactPointPosLocal + raycastResult.impactNormal );

		// After the impact point
		Vector3 rayDirection	= localEndPosition;
		float	lengthAtImpact	= rayDirection.NormalizeAndGetLength();
		float	fullLength		= lengthAtImpact / raycastResult.fractionTravelled;
		mb.SetColor( colorOnNoImpact );
		mb.PushVertex( localEndPosition );
		
		mb.SetColor( colorOnNoImpact );
		mb.PushVertex( localEndPosition + (rayDirection * (fullLength - lengthAtImpact)) );
	}

	mb.End();

	Transform modelTransform = Transform( startPosition, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *raycastDebugObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( raycastDebugObject );
}

void DebugRenderCamera( float lifetime, Camera const &camera, float const cameraBodySize, Rgba const &frustumColor, Rgba const &startColor, Rgba const &endColor, eDebugRenderMode mode )
{
	Vector3 cameraWorldPos = camera.m_cameraTransform.GetWorldPosition();

	// World to Camera
	Matrix44 worldToCameraMat = camera.GetViewMatrix();
	// Camera to Clip
	Matrix44 cameraToClipMat = camera.GetProjectionMatrix();
	// So.. World to Clip
	Matrix44 worldToClipMatrix( cameraToClipMat );
	worldToClipMatrix.Append( worldToCameraMat );						// => WORLD_TO_CLIP = (CAMERA_TO_CLIP * WORLD_TO_CAMERA)

	Matrix44 clipToWorldMatrix;
	bool inverted = worldToClipMatrix.GetInverse( clipToWorldMatrix );	// => CLIP_TO_WORLD = inverse( WORLD_TO_CLIP )
	GUARANTEE_RECOVERABLE( inverted, "Warning: Couln't inverse the Matrix!!" );

	// Camera Frustum view of far plane - from backside
	//  
	// J      far plane     A
	//  |------------------|
	//  |                  |
	//  |        * C       | far plane
	//  |                  |
	//  |------------------|
	// K                    B
	// 
	Vector4 fJ4 = clipToWorldMatrix.Multiply( Vector4( -1.f,  1.f,  1.f,  1.f ) );
	Vector4 fA4 = clipToWorldMatrix.Multiply( Vector4(  1.f,  1.f,  1.f,  1.f ) );
	Vector4 fB4 = clipToWorldMatrix.Multiply( Vector4(  1.f, -1.f,  1.f,  1.f ) );
	Vector4 fK4 = clipToWorldMatrix.Multiply( Vector4( -1.f, -1.f,  1.f,  1.f ) );
	Vector4 nJ4 = clipToWorldMatrix.Multiply( Vector4( -1.f,  1.f, -1.f,  1.f ) );
	Vector4 nA4 = clipToWorldMatrix.Multiply( Vector4(  1.f,  1.f, -1.f,  1.f ) );
	Vector4 nB4 = clipToWorldMatrix.Multiply( Vector4(  1.f, -1.f, -1.f,  1.f ) );
	Vector4 nK4 = clipToWorldMatrix.Multiply( Vector4( -1.f, -1.f, -1.f,  1.f ) );

	Vector3 farJ  = (fJ4.IgnoreW() / fJ4.w) - cameraWorldPos;
	Vector3 farA  = (fA4.IgnoreW() / fA4.w) - cameraWorldPos;
	Vector3 farB  = (fB4.IgnoreW() / fB4.w) - cameraWorldPos;
	Vector3 farK  = (fK4.IgnoreW() / fK4.w) - cameraWorldPos;
	Vector3 nearJ = (nJ4.IgnoreW() / nJ4.w) - cameraWorldPos;
	Vector3 nearA = (nA4.IgnoreW() / nA4.w) - cameraWorldPos;
	Vector3 nearB = (nB4.IgnoreW() / nB4.w) - cameraWorldPos;
	Vector3 nearK = (nK4.IgnoreW() / nK4.w) - cameraWorldPos;

	// Add these four edges in the mesh builder
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_LINES, false );

	// Camera Pos. to Near
	// J Edge
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( Vector3::ZERO );
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( nearJ );
	// A Edge
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( Vector3::ZERO );
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( nearA );
	// B Edge
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( Vector3::ZERO );
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( nearB );
	// K Edge
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( Vector3::ZERO );
	mb.SetColor( RGBA_GRAY_COLOR );
	mb.PushVertex( nearK );

	// Camera Near to Far
	// J Edge
	mb.SetColor( frustumColor );
	mb.PushVertex( nearJ );
	mb.SetColor( frustumColor );
	mb.PushVertex( farJ );
	// A Edge
	mb.SetColor( frustumColor );
	mb.PushVertex( nearA );
	mb.SetColor( frustumColor );
	mb.PushVertex( farA );
	// B Edge
	mb.SetColor( frustumColor );
	mb.PushVertex( nearB );
	mb.SetColor( frustumColor );
	mb.PushVertex( farB );
	// K Edge
	mb.SetColor( frustumColor );
	mb.PushVertex( nearK );
	mb.SetColor( frustumColor );
	mb.PushVertex( farK );

	// Far Plane's rectangle
	// JA
	mb.SetColor( frustumColor );
	mb.PushVertex( farJ );
	mb.SetColor( frustumColor );
	mb.PushVertex( farA );

	// AB
	mb.SetColor( frustumColor );
	mb.PushVertex( farA );
	mb.SetColor( frustumColor );
	mb.PushVertex( farB );

	// BK
	mb.SetColor( frustumColor );
	mb.PushVertex( farB );
	mb.SetColor( frustumColor );
	mb.PushVertex( farK );

	// KJ
	mb.SetColor( frustumColor );
	mb.PushVertex( farK );
	mb.SetColor( frustumColor );
	mb.PushVertex( farJ );

	// Near Plane's rectangle
	mb.SetColor( frustumColor );
	mb.PushVertex( nearJ );
	mb.SetColor( frustumColor );
	mb.PushVertex( nearA );

	mb.SetColor( frustumColor );
	mb.PushVertex( nearA );
	mb.SetColor( frustumColor );
	mb.PushVertex( nearB );

	mb.SetColor( frustumColor );
	mb.PushVertex( nearB );
	mb.SetColor( frustumColor );
	mb.PushVertex( nearK );

	mb.SetColor( frustumColor );
	mb.PushVertex( nearK );
	mb.SetColor( frustumColor );
	mb.PushVertex( nearJ );

	mb.End();

	// Camera Frustum
	Transform modelTransform = Transform( cameraWorldPos, Vector3::ZERO, Vector3::ONE_ALL );
	DebugRenderObject *cameraFrustumDebugObject = new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( cameraFrustumDebugObject );

	// Camera Body
	mb = MeshBuilder();
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddCube( Vector3( 0.7f, 0.7f, 1.f ), Vector3( 0.f, 0.f, -0.5f ), frustumColor );
	mb.End();

	modelTransform = Transform( cameraWorldPos, camera.m_cameraTransform.GetRotation(), Vector3( cameraBodySize, cameraBodySize, cameraBodySize ) );
	DebugRenderObject* cameraBodyDebugObject =  new DebugRenderObject( lifetime, *debugRenderer, DEBUG_CAMERA_3D, modelTransform.GetTransformMatrix(), mb.ConstructMesh <Vertex_3DPCU>(), nullptr, startColor, endColor, mode );
	debugRenderObjectQueue.push_back( cameraBodyDebugObject );
}
