#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"

class Mesh;
class Shader;
class TextureCube;

class Camera
{
public:
	const float m_aspectRatio = g_aspectRatio;

public:
			 Camera();
	virtual ~Camera();

	// For output_framebuffer
	void SetColorTarget( Texture *color_target, uint slot = 0 );
	void SetDepthStencilTarget( Texture *depth_target );
	void Finalize();
	void UpdateUBO();

	unsigned int GetFrameBufferHandle() const;

	// For on camera effects
	void PreRender	( Renderer &theRenderer );		// For Camera Effects: Bind Textures, etc..
	void PostRender	( Renderer &theRenderer );		// For Camera Effects: Apply Effects..

	// model setters
	void LookAt( Vector3 position, Vector3 target, Vector3 up = Vector3::UP ); 

	// View Matrix
	Matrix44 UpdateViewMatrix();
	Matrix44 GetViewMatrix() const;
	Matrix44 GetProjectionMatrix() const;

	// projection settings
	void SetProjectionMatrix( Matrix44 const &projMatrix );
	void SetProjectionOrtho( float size, float screen_near, float screen_far ); 
	void IncrementCameraSizeBy( float sizeIncrement );									// If using Orthographic Camera
	void SetPerspectiveCameraProjectionMatrix( float fovDegrees, float aspectRatio, float nearZ, float farZ );
	void CopyTransformViewAndProjection( Camera const &referenceCamera );						// Caution!: Copies View and Projection Matrices from referenceCamera

	// Camera's movement and rotation
	void SetCameraPositionTo		( Vector3 const &newPosition );						// Reset the position
	void SetCameraEulerRotationTo	( Vector3 const &newEulerRotation );				// Reset the rotation (in degrees)
	void MoveCameraPositionBy		( Vector3 const &localTranslation );				// Modification to the existing position
	void RotateCameraBy				( Vector3 const &localRotation );					// Modification to the existing rotation (in degrees)

	// Information Fetchers
	Vector3				GetForwardVector() const;
	Vector3				GetEulerRotation() const;
	Matrix44			GetCameraModelMatrix() const;
	UBOCameraMatrices	GetUBOCameraMatrices() const;

	// Skybox
	void SetupForSkybox	( std::string pathToSkyboxImage );								// Enables the Skybox using the image at path
	void RenderSkyBox	( Renderer &theRenderer );

	// Screen to World
	Vector3	GetWorldPositionFromScreen( Vector2 screenPosition, float ndcZ = 0.f );
	// World to Screen
	Vector2 GetScreenPositionFromWorld( Vector3 const &worldPoint, float w );			// Assumes that center of screen is ( 0, 0 )

public:
	Transform		 m_cameraTransform;
private:
	Matrix44		 m_viewMatrix;		// inverse of cameraMatrix (used for shader) (World to Camera)
	Matrix44		 m_projMatrix;		// projection, identity by default.. (Camera to Clip)

public:
	UniformBuffer	*m_cameraUBO		= nullptr;
	FrameBuffer		 m_outputFramebuffer;

private:
	// Used only if camera is orthographic
	float			m_size				=  2.f;
	float			m_screen_near		= -1.f;
	float			m_screen_far		=  1.f;
					
	Texture*		m_bloomTexture		= nullptr;

	// Skybox
	bool			m_skyboxEnabled		= false;
	TextureCube*	m_skyboxTexture		= nullptr;
	Mesh*			m_skyboxMesh		= nullptr;
	Matrix44		m_skyboxModel		= Matrix44();

	void ApplyEffect( Shader *fullScreenEffect, Renderer &theRenderer, uint totalPasses = 1 );
};