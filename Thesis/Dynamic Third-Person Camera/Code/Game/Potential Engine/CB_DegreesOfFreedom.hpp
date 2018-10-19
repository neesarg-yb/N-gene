#pragma once
#include "Game/Potential Engine/CameraBehaviour.hpp"

class CB_DegreesOfFreedom : public CameraBehaviour
{
public:
	 CB_DegreesOfFreedom( char const *name );
	~CB_DegreesOfFreedom();

protected:
	CameraState m_goalState;

public:
	void		PreUpdate () = 0;
	void		PostUpdate() = 0;
	CameraState	Update( float deltaSeconds, CameraState const &currentState ) = 0;

protected:
	// Set the Current CameraState
	void SetWorldPosition( float distanceFromAnchor, float rotationInDegrees, float altitudeInDegrees );
	void SetOrientationToLookAtAnchor();
	void SetOffsetToWorldPosition( float localHorizontalOffset, float localVerticalOffset );
	void SetFOV( float cameraFOV );

private:
	Vector3 GetPositionFromSpericalCoordinate( float radius, float rotation, float altitude );
};
