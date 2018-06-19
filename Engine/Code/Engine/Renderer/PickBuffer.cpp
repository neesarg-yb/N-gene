#pragma once
#include "PickBuffer.hpp"
#include "Engine/Renderer/Scene.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Window.hpp"

PickBuffer::PickBuffer( Renderer &activeRenderer )
	: m_activeRenderer( activeRenderer )
{
	uint width		= Window::GetInstance()->GetWidth();
	uint height		= Window::GetInstance()->GetHeight();

	m_pickTarget	= Renderer::CreateRenderTarget( width, height, TEXTURE_FORMAT_RGBA8 );
	m_depthTarget	= Renderer::CreateRenderTarget( width, height, TEXTURE_FORMAT_D24S8 );

	m_pickMaterial	= Material::CreateNewFromFile( "Data\\Materials\\pickbuffer.material" );

	m_camera		= new Camera();
}

PickBuffer::~PickBuffer()
{
	delete m_camera;
	delete m_pickMaterial;
	delete m_depthTarget;
	delete m_pickTarget;
}

void PickBuffer::GeneratePickBuffer( Camera &activeCamera, Scene &currentScene )
{
	// View and Projection
	m_camera->CopyTransformViewAndProjection( activeCamera );

	// Set Color & Depth Targets
	uint bufferWidth	= activeCamera.m_outputFramebuffer.GetWidth( 0U );
	uint bufferHeight	= activeCamera.m_outputFramebuffer.GetHeight( 0U ); 
	ResizePickTargets( bufferWidth, bufferHeight );
	m_camera->SetColorTarget( m_pickTarget, 0U );
	m_camera->SetDepthStencilTarget( m_depthTarget );

	// Get the Renderer Ready
	m_activeRenderer.BindCamera( m_camera );
	m_activeRenderer.ClearColor( RGBA_BLACK_COLOR );
	m_activeRenderer.ClearDepth( 1.f );

	// Do the Render Pass
	m_activeRenderer.BindMaterialForShaderIndex( *m_pickMaterial, 0U );
	for each ( Renderable* renderable in currentScene.m_renderables )
	{
		uint pickID	= renderable->GetPickID();
		
		// Skip if invalid
		TODO( "Renderables which uses blending should be ignored, as well." );
		if( pickID == INVALID_PICK_ID ) 
			continue;

		// Draw
		Rgba pickColor	= *(Rgba*) (&pickID);
		m_activeRenderer.SetUniform( "PICK_ID", pickColor );
		for each (Mesh* meshR in renderable->m_meshes)
		{
			m_activeRenderer.DrawMesh( *meshR, renderable->m_modelTransform.GetWorldTransformMatrix() );
		}
	}

}

uint PickBuffer::GetHandle( Vector2 mousePosition )
{
	return 0U;
}

void PickBuffer::ResizePickTargets( uint width, uint height )
{
	IntVector2 newSize = IntVector2( (int)width, (int)height );

	if( m_bufferDimension == newSize )
		return;
	else
	{
		delete m_depthTarget;
		delete m_pickTarget;

		m_pickTarget	= Renderer::CreateRenderTarget( width, height, TEXTURE_FORMAT_RGBA8 );
		m_depthTarget	= Renderer::CreateRenderTarget( width, height, TEXTURE_FORMAT_D24S8 );
	}
}
