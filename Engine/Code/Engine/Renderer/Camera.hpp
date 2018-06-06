#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"

class Camera
{
public:
	const float m_aspectRatio = g_aspectRatio;

public:
	 Camera() {}
	~Camera() {}

	// For output_framebuffer
	void SetColorTarget( Texture *color_target );
	void SetDepthStencilTarget( Texture *depth_target );
	void Finalize();

	unsigned int GetFrameBufferHandle() const;

	// model setters
	void LookAt( Vector3 position, Vector3 target, Vector3 up = Vector3::UP ); 

	// projection settings
	void SetProjectionOrtho( float size, float screen_near, float screen_far ); 
	void IncrementCameraSizeBy( float sizeIncrement );									// If using Orthographic Camera
	void SetPerspectiveCameraProjectionMatrix( float fovDegrees, float aspectRatio, float nearZ, float farZ );

	// Camera's movement and rotation
	void SetCameraPositionTo		( Vector3 const &newPosition );						// Reset the position
	void SetCameraEulerRotationTo	( Vector3 const &newEulerRotation );				// Reset the rotation (in degrees)
	void MoveCameraPositionBy		( Vector3 const &localTranslation );				// Modification to the existing position
	void RotateCameraBy				( Vector3 const &localRotation );					// Modification to the existing rotation (in degrees)

	// Information Fetchers
	Vector3 GetForwardVector() const;
	Vector3 GetEulerRotation() const;

public:
	Matrix44 m_camera_matrix;	// where is the camera? (Model Matrix)
	Matrix44 m_view_matrix;		// inverse of cameraMatrix (used for shader) (World to Camera)
	Matrix44 m_proj_matrix;		// projection, identity by default..

	FrameBuffer m_output_framebuffer;

private:
	// Used only if camera is orthographic
	float	 m_size			=  2.f;
	float	 m_screen_near	= -1.f;
	float	 m_screen_far	=  1.f;
};