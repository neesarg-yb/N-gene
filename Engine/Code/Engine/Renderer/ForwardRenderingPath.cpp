#pragma once
#include <tuple>
#include "ForwardRenderingPath.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Renderable.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"

typedef std::tuple< float, unsigned int > ContributionLightIndexTuple;
typedef std::vector< DrawCall > DrawCallList;
typedef std::tuple< float, DrawCall > ZDistanceDrawcallTuple;

ForwardRenderingPath::ForwardRenderingPath( Renderer &activeRenderer )
	: m_renderer( activeRenderer )
{
	m_shadowCamera			= new Camera();
	m_shadowColorTarget		= m_renderer.CreateRenderTarget( 2048, 2048, TEXTURE_FORMAT_RGBA8 );
	m_shadowDepthTarget		= m_renderer.CreateRenderTarget( 2048, 2048, TEXTURE_FORMAT_D24S8 );
	m_shadowCamera->SetColorTarget( m_shadowColorTarget );
	m_shadowCamera->SetDepthStencilTarget( m_shadowDepthTarget );
}

ForwardRenderingPath::~ForwardRenderingPath()
{
	delete m_shadowDepthTarget;
	delete m_shadowColorTarget;
	delete m_shadowCamera;
}

void ForwardRenderingPath::RenderSceneForCamera( Camera &camera, Scene &scene ) const
{
	// For each Lights, Render for ShadowMap
	TODO( "I'm assuming that there is only one directional light & it uses ShadowMap!" );
	RenderSceneForShadowMap( scene );

	// Bind the camera
	m_renderer.BindCamera( &camera );
	
	// Do the camera cleanup operations
	m_renderer.ClearColor( RGBA_BLACK_COLOR );
	m_renderer.ClearDepth( 1.0f ); 
	m_renderer.EnableDepth( COMPARE_LESS, true );

	camera.PreRender( m_renderer );

	// Generate draw calls
	std::vector< DrawCall > drawCalls;
	for ( uint renderableIdx = 0; renderableIdx < scene.m_renderables.size(); renderableIdx++ )
	{
		Renderable *thisRenderable = scene.m_renderables[ renderableIdx ];

		// For each mesh in theRenderable, construct a drawcall
		for( unsigned int mIdx = 0; mIdx < thisRenderable->m_meshes.size(); mIdx++ )
		{
			Mesh const	*meshToAdd		= thisRenderable->GetMesh( mIdx );
			Material	*materialToAdd	= thisRenderable->GetMaterial( mIdx );
			Transform	&transformToAdd = thisRenderable->m_modelTransform;
			
			DrawCall dc( *meshToAdd, *materialToAdd, transformToAdd );
			dc.m_layer				= thisRenderable->GetRenderLayer( mIdx );
			dc.m_queueTypeIsApha	= thisRenderable->IsAlphaQueueType( mIdx );
			// For now, every materials uses light
			SetMostContributingLights( dc.m_lightCount, dc.m_lightIndices, thisRenderable->GetPosition(), scene.m_lights );
			
			drawCalls.push_back( dc );
		}
	}

	// Sort draw calls
	SortDrawCallsForCamera( drawCalls, camera );

	// Render 'em
	for( uint dcIdx = 0; dcIdx < drawCalls.size(); dcIdx++ )
	{
		DrawCall& dc = drawCalls[ dcIdx ];

		// Draw for each Shaders present in ShaderGroup
		uint shaderGroupSize = (uint) dc.m_material->m_shaderGroup.size();
		for( uint shaderIndex = 0; shaderIndex < shaderGroupSize; shaderIndex++ )
		{
			m_renderer.BindMaterialForShaderIndex( *dc.m_material, shaderIndex );
			EnableLightsForDrawCall( dc, scene.m_lights );

			m_renderer.DrawMesh( *dc.m_mesh, dc.m_model );
		}

	}

	camera.PostRender( m_renderer );

	TODO( "Apply Effects, if there are any.." );
}

void ForwardRenderingPath::RenderSceneForShadowMap( Scene &scene ) const
{
	for each (Light* light in scene.m_lights)
	{
		// Setup the camera at that light
		Matrix44 lightsWorldMatrix	= light->m_transform.GetWorldTransformMatrix();
		m_shadowCamera->m_cameraTransform.SetFromMatrix( lightsWorldMatrix );
		Matrix44 projectionMatrix	= Matrix44::MakeOrtho3D( 256, 256, -100, 100 );
		m_shadowCamera->SetProjectionMatrix( projectionMatrix );

		m_renderer.BindCamera( m_shadowCamera );

		m_renderer.ClearColor( RGBA_BLACK_COLOR );
		m_renderer.ClearDepth( 1.0f ); 
		m_renderer.EnableDepth( COMPARE_LESS, true );

		for each (Renderable* thisRenderable in scene.m_renderables)
		{
			for( unsigned int mIdx = 0; mIdx < thisRenderable->m_meshes.size(); mIdx++ )
			{
				// Setup a drawcall
				Mesh const	*mesh		= thisRenderable->GetMesh( mIdx );
				Material	*material	= thisRenderable->GetMaterial( mIdx );
				Transform	&transform  = thisRenderable->m_modelTransform;

				// Draw for each Shaders present in ShaderGroup
				m_renderer.BindMaterialForShaderIndex( *material );
				m_renderer.DrawMesh( *mesh, transform.GetWorldTransformMatrix() );
			}
		}
	}
}

void ForwardRenderingPath::SetMostContributingLights( unsigned int &lightCount, unsigned int ( &effectiveLightIndices )[MAX_LIGHTS], Vector3 const &renderablePosition, std::vector< Light* > &lightsInScene ) const
{
	lightCount = 0;

	// Get all the lightSources to sort
	std::vector< ContributionLightIndexTuple > lightSourcesToSort;
	for( unsigned int idx = 0; idx < lightsInScene.size(); idx++ )
	{
		Light* thisLight		= lightsInScene[idx];
		float lightContribution = thisLight->GetAttenuationForRenderableAt( renderablePosition );

		ContributionLightIndexTuple thisTuple = std::make_tuple( lightContribution, idx );
		lightSourcesToSort.push_back( thisTuple );
	}

	// Bubble Sort
	int n = (int) lightSourcesToSort.size();
	for( int i = 0; i < n - 1; i++ )
	{
		for( int j = 0; j < n-i-1; j++ )
		{
			float aLightsContribution = std::get<0>( lightSourcesToSort[j] );
			float bLightsContribution = std::get<0>( lightSourcesToSort[j+1] );
			if( aLightsContribution < bLightsContribution )
			{
				// Swap these two tuples
				ContributionLightIndexTuple temp = lightSourcesToSort[j];
				lightSourcesToSort[j]		= lightSourcesToSort[j+1];
				lightSourcesToSort[j+1]		= temp;
			}
		}
	}

	// Out sorted lights and the count
	for( int i = 0; i < lightSourcesToSort.size() && i < MAX_LIGHTS; i++ )
	{
		effectiveLightIndices[i] = std::get<1>( lightSourcesToSort[i] );
		lightCount++;
	}
}

void ForwardRenderingPath::SortDrawCallsForCamera( std::vector<DrawCall> &drawCallsToSort, Camera &camera ) const
{
	// Separate all the layers in different vectors
	std::map< unsigned int , DrawCallList > layersAndDrawCalls;
	while( drawCallsToSort.size() > 0 )
	{
		DrawCall lastDrawCall = drawCallsToSort.back();
		drawCallsToSort.pop_back();

		layersAndDrawCalls[ lastDrawCall.m_layer ].push_back( lastDrawCall );
	}

	// For each layer
	for( std::map< unsigned int, DrawCallList >::iterator itLayer = layersAndDrawCalls.begin(); itLayer != layersAndDrawCalls.end(); itLayer++ )
	{
		DrawCallList& itDrawCallList = itLayer->second;
		// For each draw call in that layer
		for( unsigned int i = 0; i < itDrawCallList.size(); i++ )
		{
			// Sort for Opaque
			DrawCall &drawCallOfThisLayer = itDrawCallList[i];
			if( drawCallOfThisLayer.m_queueTypeIsApha == false )
			{
				drawCallsToSort.push_back( drawCallOfThisLayer );
				itDrawCallList.erase( itDrawCallList.begin() + i );
				i--;
			}
		}

		// For remaining alpha draw calls in that layer, sort 'em
		SortAlphaDrawCallsForCameraZ( itDrawCallList, camera );
		
		for( unsigned int i = 0; i < itDrawCallList.size(); i++ )
		{
			drawCallsToSort.push_back( itDrawCallList[i] );
		}
	}
}

void ForwardRenderingPath::SortAlphaDrawCallsForCameraZ( std::vector< DrawCall > &drawCallsToSort, Camera &camera ) const
{
	// Create vector to sort it
	std::vector< ZDistanceDrawcallTuple > drawCallTuplesToSort;
	while( drawCallsToSort.size() > 0 )
	{
		DrawCall& toPush	= drawCallsToSort.back();
		Vector3 direction	= toPush.m_model.GetTColumn() - camera.GetCameraModelMatrix().GetTColumn();
		Vector3 cameraZ		= camera.GetCameraModelMatrix().GetKColumn();
		float	projection	= Vector3::DotProduct( direction, cameraZ );

		ZDistanceDrawcallTuple tupleToPush = std::make_tuple( projection, toPush );
		drawCallTuplesToSort.push_back( tupleToPush );

		drawCallsToSort.pop_back();
	}

	// Bubble Sort
	int n = (int) drawCallTuplesToSort.size();
	for( int i = 0; i < n - 1; i++ )
	{
		for( int j = 0; j < n-i-1; j++ )
		{
			float aDrawCallDistance = std::get<0>( drawCallTuplesToSort[j] );
			float bDrawCallDistance = std::get<0>( drawCallTuplesToSort[j+1] );
			if( aDrawCallDistance < bDrawCallDistance )
			{
				// Swap these two tuples
				ZDistanceDrawcallTuple temp = drawCallTuplesToSort[j];
				drawCallTuplesToSort[j]		= drawCallTuplesToSort[j+1];
				drawCallTuplesToSort[j+1]	= temp;
			}
		}
	}

	// Add all sorted DrawCalls to the out vector
	for( int i = 0; i < drawCallTuplesToSort.size(); i++ )
	{
		drawCallsToSort.push_back( std::get<1>( drawCallTuplesToSort[i] ) );
	}
}

void ForwardRenderingPath::EnableLightsForDrawCall( DrawCall &dc, std::vector< Light* > &allLights ) const
{
	unsigned int numLights = dc.m_lightCount;

	// Reset/Disable all active lights of Renderer
	m_renderer.DisableAllLights();

	// Enable lights that are needed for this DrawCall
	for( unsigned int bindPoint = 0; bindPoint < numLights; bindPoint++ )
	{
		unsigned int lighIdx = dc.m_lightIndices[ bindPoint ];
		m_renderer.EnableLight( bindPoint, *allLights[ lighIdx ] );
	}
}