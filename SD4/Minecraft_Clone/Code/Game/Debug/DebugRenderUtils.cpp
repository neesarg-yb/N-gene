#pragma once
#include "DebugRenderUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"

void MDebugUtils::RenderCubeWireframe( AABB3 const &worldBounds, Rgba const &color, bool useXRay )
{
	Vector3 const blockWorldCenter		= worldBounds.GetCenter();
	Vector3 const blockHalfDimensions	= worldBounds.GetSize() * 0.5f;

	/*
	      7_________ 6			VERTEX[8] ORDER:
		  /|       /|				( 0, 1, 2, 3, 4, 5, 6, 7 )
		 / | top  / |				
	   4/__|_____/5 |			
		|  |_____|__|			   z|   
		| 3/     |  /2				|  / x
		| /  bot | /				| /
		|/_______|/			y ______|/ 
		0         1
	*/
	Vector3 const vertexPos[8] = {
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z - blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x - blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y - blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z ),
		Vector3( blockWorldCenter.x + blockHalfDimensions.x,	blockWorldCenter.y + blockHalfDimensions.y,	blockWorldCenter.z + blockHalfDimensions.z )
	};

	constexpr int vBufferSize = 24;
	Vertex_3DPCU vBuffer[vBufferSize];
	// Bottom Side
	// Line 01
	vBuffer[0].m_color = color;
	vBuffer[0].m_position = vertexPos[0];
	vBuffer[1].m_color = color;
	vBuffer[1].m_position = vertexPos[1];

	// Line 12
	vBuffer[2].m_color = color;
	vBuffer[2].m_position = vertexPos[1];
	vBuffer[3].m_color = color;
	vBuffer[3].m_position = vertexPos[2];

	// Line 23
	vBuffer[4].m_color = color;
	vBuffer[4].m_position = vertexPos[2];
	vBuffer[5].m_color = color;
	vBuffer[5].m_position = vertexPos[3];

	// Line 30
	vBuffer[6].m_color = color;
	vBuffer[6].m_position = vertexPos[3];
	vBuffer[7].m_color = color;
	vBuffer[7].m_position = vertexPos[0];

	// Top Side
	// Line 45
	vBuffer[8].m_color = color;
	vBuffer[8].m_position = vertexPos[4];
	vBuffer[9].m_color = color;
	vBuffer[9].m_position = vertexPos[5];

	// Line 56
	vBuffer[10].m_color = color;
	vBuffer[10].m_position = vertexPos[5];
	vBuffer[11].m_color = color;
	vBuffer[11].m_position = vertexPos[6];

	// Line 67
	vBuffer[12].m_color = color;
	vBuffer[12].m_position = vertexPos[6];
	vBuffer[13].m_color = color;
	vBuffer[13].m_position = vertexPos[7];

	// Line 74
	vBuffer[14].m_color = color;
	vBuffer[14].m_position = vertexPos[7];
	vBuffer[15].m_color = color;
	vBuffer[15].m_position = vertexPos[4];
	
	// Vertical Four Edges
	// Line 40
	vBuffer[16].m_color = color;
	vBuffer[16].m_position = vertexPos[4];
	vBuffer[17].m_color = color;
	vBuffer[17].m_position = vertexPos[0];

	// Line 51
	vBuffer[18].m_color = color;
	vBuffer[18].m_position = vertexPos[5];
	vBuffer[19].m_color = color;
	vBuffer[19].m_position = vertexPos[1];

	// Line 62
	vBuffer[20].m_color = color;
	vBuffer[20].m_position = vertexPos[6];
	vBuffer[21].m_color = color;
	vBuffer[21].m_position = vertexPos[2];

	// Line 73
	vBuffer[22].m_color = color;
	vBuffer[22].m_position = vertexPos[7];
	vBuffer[23].m_color = color;
	vBuffer[23].m_position = vertexPos[3];

	// Render Normally
	g_theRenderer->BindMaterialForShaderIndex( *g_defaultMaterial );
	g_theRenderer->EnableDepth( COMPARE_LESS, false );
	g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, vBufferSize, PRIMITIVE_LINES );

	if( useXRay )
	{
		// Change vertex color to be a little dimmer
		for( int i = 0; i < vBufferSize; i++ )
		{
			vBuffer[i].m_color.r = (uchar)( (float)vBuffer[i].m_color.r * 0.5f );
			vBuffer[i].m_color.g = (uchar)( (float)vBuffer[i].m_color.g * 0.5f );
			vBuffer[i].m_color.b = (uchar)( (float)vBuffer[i].m_color.b * 0.5f );
		}

		// Render only if hidden!
		g_theRenderer->EnableDepth( COMPARE_GREATER, false );
		g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, vBufferSize, PRIMITIVE_LINES );
	}
}

void MDebugUtils::RenderSphereWireframe( Vector3 const &center, float radius, Rgba const &color, bool useXRay )
{
	MeshBuilder mbSphere;
	mbSphere.AddSphere( radius, 10, 6, center, color );
	Mesh* wireSphereMesh = mbSphere.ConstructMesh<Vertex_3DPCU>();

	g_theRenderer->EnableDepth( COMPARE_LESS, false );
	g_theRenderer->BindMaterialForShaderIndex( *g_defaultMaterial );

	// i.e. wire-frame => we want to see all the sides
	g_theRenderer->SetCullingMode( CULLMODE_NONE );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	
	g_theRenderer->DrawMesh( *wireSphereMesh );

	// if X-Ray enabled
	if( useXRay )
	{
		// Dim the color for X-Ray
		for( int v = 0; v < mbSphere.m_vertices.size(); v++ )
		{
			Rgba &vColor = mbSphere.m_vertices[v].m_color;

			vColor.r = (uchar)( (float)vColor.r * 0.5f );
			vColor.g = (uchar)( (float)vColor.g * 0.5f );
			vColor.b = (uchar)( (float)vColor.b * 0.5f );
		}

		Mesh* wireSphereXRayMesh = mbSphere.ConstructMesh<Vertex_3DPCU>();

		g_theRenderer->EnableDepth( COMPARE_GREATER, false );
		g_theRenderer->DrawMesh( *wireSphereXRayMesh );

		delete wireSphereXRayMesh;
	}

	delete wireSphereMesh;
}

void MDebugUtils::RenderLine( Vector3 const &startPos, Rgba const &startColor, Vector3 const &endPos, Rgba const &endColor, bool useXRay )
{
	constexpr int vBufferSize = 2;
	Vertex_3DPCU vBuffer[vBufferSize];
	vBuffer[0].m_color		= startColor;
	vBuffer[0].m_position	= startPos;
	vBuffer[1].m_color		= endColor;
	vBuffer[1].m_position	= endPos;

	// First Render Pass [ Normal ]
	g_theRenderer->BindMaterialForShaderIndex( *g_defaultMaterial );
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, vBufferSize, PRIMITIVE_LINES );

	// For, Second Render Pass [ X-Ray ]
	if( useXRay )
	{
		for( int i = 0; i < vBufferSize; i++ )
		{
			Rgba &vertColor = vBuffer[i].m_color;

			vertColor.r = (uchar)( (float)vertColor.r * 0.5f );
			vertColor.g = (uchar)( (float)vertColor.g * 0.5f );
			vertColor.b = (uchar)( (float)vertColor.b * 0.5f );
		}

		g_theRenderer->EnableDepth( COMPARE_GREATER, false );	// To Draw X-Ray without affecting the depth
		g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, vBufferSize, PRIMITIVE_LINES );
	}
}

void MDebugUtils::RenderBasis( Vector3 const &position, float length, bool useXRay )
{
	constexpr int vBufferSize = 6;
	Vertex_3DPCU vBuffer[vBufferSize];

	vBuffer[0].m_color		= RGBA_RED_COLOR;
	vBuffer[0].m_position	= position;
	vBuffer[1].m_color		= RGBA_RED_COLOR;
	vBuffer[1].m_position	= position + Vector3( length, 0.f, 0.f );

	vBuffer[2].m_color		= RGBA_GREEN_COLOR;
	vBuffer[2].m_position	= position;
	vBuffer[3].m_color		= RGBA_GREEN_COLOR;
	vBuffer[3].m_position	= position + Vector3( 0.f, length, 0.f );

	vBuffer[4].m_color		= RGBA_BLUE_COLOR;
	vBuffer[4].m_position	= position;
	vBuffer[5].m_color		= RGBA_BLUE_COLOR;
	vBuffer[5].m_position	= position + Vector3( 0.f, 0.f, length );

	// First Render Pass [ Normal ]
	g_theRenderer->BindMaterialForShaderIndex( *g_defaultMaterial );
	g_theRenderer->EnableDepth( COMPARE_LESS, true );
	g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, vBufferSize, PRIMITIVE_LINES );

	// For, Second Render Pass [ X-Ray ]
	if( useXRay )
	{
		for( int i = 0; i < vBufferSize; i++ )
		{
			Rgba &vertColor = vBuffer[i].m_color;

			vertColor.r = (uchar)( (float)vertColor.r * 0.5f );
			vertColor.g = (uchar)( (float)vertColor.g * 0.5f );
			vertColor.b = (uchar)( (float)vertColor.b * 0.5f );
		}

		g_theRenderer->EnableDepth( COMPARE_GREATER, false );	// To Draw X-Ray without affecting the depth
		g_theRenderer->DrawMeshImmediate<Vertex_3DPCU>( vBuffer, vBufferSize, PRIMITIVE_LINES );
	}
}

void MDebugUtils::RenderRaycast( RaycastResult_MC const &raycastResult )
{
	// Render line which did not hit
	RenderLine( raycastResult.m_startPosition, RGBA_GREEN_COLOR, raycastResult.m_impactPosition, RGBA_GREEN_COLOR, true );

	// The impact point!
	RenderBasis( raycastResult.m_impactPosition, 0.5f, true );
	
	// Render line which did hit
	RenderLine( raycastResult.m_impactPosition, RGBA_RED_COLOR, raycastResult.m_endPosition, RGBA_RED_COLOR, true );
}
