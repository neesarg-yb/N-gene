#pragma once
#include "Camera.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/TextureCube.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

Camera::Camera()
{
	m_cameraUBO = UniformBuffer::For<UBOCameraMatrices>( GetUBOCameraMatrices() );


	uint width	= Window::GetInstance()->GetWidth();
	uint height = Window::GetInstance()->GetHeight();

	m_bloomTexture = Renderer::CreateRenderTarget( width, height );
}

Camera::~Camera()
{
	delete m_bloomTexture;
	delete m_cameraUBO;

	if( m_skyboxEnabled )
	{
		delete m_skyboxTexture;
		delete m_skyboxMesh;
	}
}

void Camera::SetColorTarget( Texture *color_target, uint slot /* = 0 */ )
{
	m_outputFramebuffer.SetColorTarget( color_target, slot );
}

void Camera::SetDepthStencilTarget( Texture *depth_target )
{
	m_outputFramebuffer.SetDepthStencilTarget( depth_target );
}

void Camera::Finalize()
{
	m_outputFramebuffer.Finalize();
}

void Camera::UpdateUBO()
{
	if( m_updateView )
		UpdateViewMatrix();

	*m_cameraUBO->As<UBOCameraMatrices>() = GetUBOCameraMatrices();
	 m_cameraUBO->UpdateGPU();
}

unsigned int Camera::GetFrameBufferHandle() const
{
	return m_outputFramebuffer.GetHandle();
}

void Camera::PreRender( Renderer &theRenderer )
{
	PROFILE_SCOPE_FUNCTION();

	TODO( "Decide when you'll bind Bloom and other textures, again!" );
	// Unbind all other color targets except main one..
	for( uint i = 1; i < MAX_COLOR_TARGETS; i++ )
		m_outputFramebuffer.SetColorTarget( nullptr, i );

	m_outputFramebuffer.Finalize();

	// Render SkyBox if enabled
	if( m_skyboxEnabled )
		RenderSkyBox( theRenderer );

	// Bind the Bloom Texture
	SetColorTarget( m_bloomTexture, 1 );
}

void Camera::PostRender( Renderer &theRenderer )
{
	PROFILE_SCOPE_FUNCTION();

	Shader *bloomShader = theRenderer.CreateOrGetShader( "bloom_fs" );

	// Apply the Bloom Effect
	ApplyEffect( bloomShader, theRenderer, 3 );

	this->Finalize();
	Shader *combineShader = theRenderer.CreateOrGetShader( "combine_fs" );
	
	theRenderer.UseShader( combineShader );
	theRenderer.DrawTexturedAABB( AABB2::NDC_SIZE, *m_bloomTexture, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
}

void Camera::LookAt( Vector3 position, Vector3 target, Vector3 up /* = Vector3::UP */ )
{
	Vector3 K		= (target - position).GetNormalized();
	Vector3 I		= Vector3::CrossProduct(up, K).GetNormalized();
	Vector3 J		= Vector3::CrossProduct(K, I);
	Vector3 T		= position;
	float array[16] = { I.x, I.y, I.z, 0.f, J.x, J.y, J.z, 0.f, K.x, K.y, K.z, 0.f, T.x, T.y, T.z, 1.f };
	
	Matrix44 cameraMatrix = Matrix44( array );
	Vector3 eulerRotation = cameraMatrix.GetEulerRotation();
	m_cameraTransform.SetPosition( position );
	m_cameraTransform.SetRotation( eulerRotation );

	m_viewMatrix = cameraMatrix.GetOrthonormalInverse();
}

Matrix44 Camera::UpdateViewMatrix()
{
	m_viewMatrix = m_cameraTransform.GetWorldTransformMatrix().GetOrthonormalInverse();

	return m_viewMatrix;
}

Matrix44 Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

Matrix44 Camera::GetProjectionMatrix() const
{
	return m_projMatrix;
}

void Camera::SetProjectionMatrixUnsafe( Matrix44 const &projMatrix )
{
	m_projMatrix = projMatrix;
}

void Camera::SetViewMatrixUnsafe( Matrix44 const &viewMat )
{
	m_updateView = false;
	m_viewMatrix = viewMat;
}

void Camera::SetAspectRatioForPerspective( float aspectRatio )
{
	m_aspectRatio = aspectRatio;
	m_projMatrix = Matrix44::MakePerspective3D( m_fov, m_aspectRatio, m_screen_near, m_screen_far );
}

void Camera::SetFOVForPerspective( float fov )
{
	m_fov = fov;
	m_projMatrix = Matrix44::MakePerspective3D( m_fov, m_aspectRatio, m_screen_near, m_screen_far );
}

void Camera::SetNearAndFarForPerspective( float cameraNear, float cameraFar )
{
	m_screen_far = cameraFar;
	m_screen_near = cameraNear;
	m_projMatrix = Matrix44::MakePerspective3D( m_fov, m_aspectRatio, m_screen_near, m_screen_far );
}

void Camera::SetProjectionOrtho( float size, float screen_near, float screen_far )
{
	m_size		  = size;
	m_screen_near = screen_near;
	m_screen_far  = screen_far;

	m_projMatrix = Matrix44::MakeOrtho3D( size * m_aspectRatio, size, screen_near, screen_far );
}

void Camera::IncrementCameraSizeBy( float sizeIncrement )
{
	if( m_size + sizeIncrement <= 0.f )
		return;
	
	m_size += sizeIncrement;
	SetProjectionOrtho( m_size, m_screen_near, m_screen_far );
}

void Camera::SetPerspectiveCameraProjectionMatrix( float fovDegrees, float aspectRatio, float nearZ, float farZ )
{
	m_fov			= fovDegrees;
	m_aspectRatio	= aspectRatio;
	m_screen_near	= nearZ;
	m_screen_far	= farZ;

	m_projMatrix = Matrix44::MakePerspective3D( fovDegrees, aspectRatio, nearZ, farZ );
}

void Camera::CopyTransformViewAndProjection( Camera const &referenceCamera )
{
	// Copy Transform, View & Projection Matrix
	m_cameraTransform	= referenceCamera.m_cameraTransform;
	m_projMatrix		= referenceCamera.m_projMatrix;
	m_viewMatrix		= referenceCamera.m_viewMatrix;

	// Copy all the private properties which are dependent of these matrices
	m_aspectRatio		= referenceCamera.m_aspectRatio;
	m_fov				= referenceCamera.m_fov;
	m_size				= referenceCamera.m_size;
	m_screen_near		= referenceCamera.m_screen_near;
	m_screen_far		= referenceCamera.m_screen_far;
}

void Camera::SetCameraPositionTo( Vector3 const &newPosition )
{
	m_cameraTransform.SetPosition( newPosition );
}

void Camera::SetCameraQuaternionRotationTo( Quaternion const &newQuaternionRotation )
{
	m_cameraTransform.SetQuaternion( newQuaternionRotation );
}

void Camera::SetCameraEulerRotationTo( Vector3 const &newEulerRotation )
{
	m_cameraTransform.SetRotation( newEulerRotation );
}

void Camera::MoveCameraPositionBy( Vector3 const &localTranslation )
{
	Vector3 worldTranslation	= m_cameraTransform.GetWorldTransformMatrix().Multiply( localTranslation, 0.f );
	Vector3 currentPosition		= m_cameraTransform.GetPosition();
	m_cameraTransform.SetPosition( currentPosition + worldTranslation );
}

void Camera::RotateCameraBy( Vector3 const &localRotation )
{
	Vector3 currentRotation = m_cameraTransform.GetRotation();
	m_cameraTransform.SetRotation( currentRotation + localRotation );
}

Vector3 Camera::GetForwardVector() const 
{
	TODO( "This should be camera's transform matrix!" );
	return m_viewMatrix.GetKColumn();
}

Vector3 Camera::GetEulerRotation() const
{
	return m_cameraTransform.GetRotation();
}

Matrix44 Camera::GetCameraModelMatrix() const
{
	return m_cameraTransform.GetTransformMatrix();
}

UBOCameraMatrices Camera::GetUBOCameraMatrices() const
{
	UBOCameraMatrices toReturn;

	toReturn.usesShadowMap		= m_shadowMapEnabled ? 1.f : 0.f;
	toReturn.viewMatrix			= m_viewMatrix;
	toReturn.projectionMatrix	= m_projMatrix;

	return toReturn;
}

void Camera::SetupForSkybox( std::string pathToSkyboxImage )
{
	// If Skybox was already setup, delete the older texture
	if( m_skyboxEnabled == true )
	{
		// Reset the variables
		delete m_skyboxTexture;
		delete m_skyboxMesh;
	}

	// Setup all the variables required for Skybox
	m_skyboxTexture = new TextureCube();
	bool success	= m_skyboxTexture->MakeFromImage( pathToSkyboxImage.c_str() );

	GUARANTEE_OR_DIE( success, "Skybox Creation has a GLError!!" );

	m_skyboxMesh	= MeshBuilder::CreateCube( Vector3::ONE_ALL );
	m_skyboxModel	= Matrix44();
	m_skyboxEnabled = true;
}

void Camera::RenderSkyBox( Renderer &theRenderer )
{
	// Bind Shader
	theRenderer.UseShader( theRenderer.CreateOrGetShader( "skybox" ) );

	// Bind Camera UBO
	theRenderer.BindCamera( this );
	
	// Bind Texture & Sampler
	theRenderer.BindTextureCube( 8, *m_skyboxTexture, theRenderer.GetDefaultSampler( SAMPLER_LINEAR ) );

	// Draw Cube
	theRenderer.DrawMesh( *m_skyboxMesh, m_skyboxModel );
}

Vector3 Camera::GetWorldPositionFromNDC( Vector3 ndcPos ) const
{
	// NDC to View
	Matrix44 invProjMatrix;	
	bool valid			= m_projMatrix.GetInverse( invProjMatrix );
	Vector4	 viewPos	= invProjMatrix.Multiply( Vector4( ndcPos, 1.f ) );
	GUARANTEE_RECOVERABLE( valid, "Warning: Couln't inverse the Matrix!!" );
	// 
	// 	std::string viewString = Stringf( "View: ( %f, %f, %f, %f )", viewPos.x, viewPos.y, viewPos.z, viewPos.w );
	// 	DebugRender2DText( 0.f, Vector2(-380.f, -380.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, viewString.c_str() );

	// View to World
	Matrix44 invViewMatrix;
	valid				= m_viewMatrix.GetInverse( invViewMatrix );
	Vector4 worldPos4	= invViewMatrix.Multiply( viewPos );
	GUARANTEE_RECOVERABLE( valid, "Warning: Couln't inverse the Matrix!!" );

	Vector3 worldPos	= Vector3( worldPos4.x, worldPos4.y, worldPos4.z ) / worldPos4.w;
	// 
	// 	std::string worldString = Stringf( "World: ( %f, %f, %f )", worldPos.x, worldPos.y, worldPos.z );
	// 	DebugRender2DText( 0.f, Vector2(-380.f, -360.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, worldString.c_str() );

	return worldPos;
}

Vector3 Camera::GetWorldPositionFromScreen( Vector2 screenPosition, float ndcZ /* = 0.f */ ) const
{
	// Screen to NDC
	Vector3 ndcXYZ;
	ndcXYZ.x	= RangeMapFloat( screenPosition.x, 0.f, (float)Window::GetInstance()->GetWidth(),  -1.f,  1.f );
	ndcXYZ.y	= RangeMapFloat( screenPosition.y, 0.f, (float)Window::GetInstance()->GetHeight(),  1.f, -1.f );
	ndcXYZ.z	= ClampFloat( ndcZ, -1.f, 1.f );
// 
// 	std::string ndcString = Stringf( "NDC: ( %f, %f, %f )", ndcXYZ.x, ndcXYZ.y, ndcXYZ.z );
// 	DebugRender2DText( 0.f, Vector2(-380.f, -400.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, ndcString.c_str() );

	Vector3 worldPos = GetWorldPositionFromNDC( ndcXYZ );

	return worldPos;
}

Vector2 Camera::GetScreenPositionFromWorld( Vector3 const &worldPoint, float w ) const
{
	// World to Camera
	Vector4 posInCamera = m_viewMatrix.Multiply( Vector4( worldPoint, w ) );

	// Camera to Clip
	Vector4 posInClip = m_projMatrix.Multiply( posInCamera );

	// Clip to NDC
	Vector3 posInNDC = Vector3( posInClip.x / posInClip.w,
								posInClip.y / posInClip.w,
								posInClip.z / posInClip.w );

	// Scale NDC according to Camera's width & height
	float	halfHeight			= (float) Window::GetInstance()->GetHeight() * 0.5f;
	float	halfWidth			= (float) Window::GetInstance()->GetWidth() * 0.5f;
	Vector2 screenPosition	= Vector2( posInNDC.x * halfWidth, posInNDC.y * halfHeight );
	
	return screenPosition;
}

void Camera::ApplyEffect( Shader *fullScreenEffect, Renderer &theRenderer, uint totalPasses /* = 1 */ )
{
	Texture *secondaryTexture = m_outputFramebuffer.m_color_targets[1];		// Camera's secondary bound Texture

	Camera effectCamera;
	effectCamera.SetColorTarget( secondaryTexture );
	effectCamera.Finalize();

	theRenderer.UseShader( fullScreenEffect );

	// Blur Passes
	for( uint pass = 0; pass < totalPasses; pass++ )
	{
		theRenderer.DrawTexturedAABB( AABB2::NDC_SIZE, *secondaryTexture, Vector2::ZERO, Vector2::ONE_ONE, RGBA_WHITE_COLOR );
	}
}
