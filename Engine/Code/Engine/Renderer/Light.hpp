#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/Renderable.hpp"

class Mesh;
class Renderer;

class Light
{
public:
	 Light();
	 Light( Vector3 const &position, Vector3 const &eulerRotation = Vector3::ZERO  );
	~Light();

public:
	// Color, Intensity
	Vector4		m_lightColorAndIntensity	= Vector4( 0.f, 0.f, 0.f, 0.f );

	// Position, Direction
	Transform	m_transform;
	Vector3		m_direction					= Vector3( 0.f, 0.f, 1.f );			// m_transform.GetRotation()->GetAsDirection()
	float		m_directionFactor			= 0.f;								// 1: Direction Light & 0: Point Light
	
	// Attenuation
	Vector3		m_attenuation				= Vector3( 0, 0, 1 );
	float		m_dotInnerAngle				= 0.f;
	float		m_dotOuterAngle				= 360.f;
	
	// ShadowMap
	bool		m_isUsingShadowMap			= false;
	Matrix44	m_viewProjectionMat			= Matrix44();

public:
	Renderable *m_renderable				= nullptr;
	
public:
	LightData	GetLightDataForUBO() const;
	float		GetAttenuationForRenderableAt( Vector3 const &renderablePos ) const;

	Vector3		GetPosition		() const;
	Vector3		GetEulerRotation() const;
	void		SetPosition		( Vector3 const &newPosition );
	void		SetEulerRotation( Vector3 const &newRotation );
	void		SetPosition		( float x,		 float y,		float z );
	void		SetEulerRotation( float aroundX, float aroundY, float aroundZ );

public:
	void SetUpForPointLight			( float intensity, Vector3 const &attenuationConstants = Vector3( 0.f, 0.f, 1.f ), Rgba const &theColor = RGBA_WHITE_COLOR );
	void SetUpForSpotLight			( float intensity, float innerAngle, float outerAngle, Vector3 const &attenuationConstants = Vector3( 0.f, 0.f, 1.f ), Rgba const &theColor = RGBA_WHITE_COLOR );
	void SetUpForDirectionalLight	( float intensity, Vector3 const &attenuationConstants = Vector3( 0.f, 0.f, 1.f ), Rgba const &theColor = RGBA_WHITE_COLOR );

	void UsesShadowMap( bool usesShadowMap );
	void SetViewProjectionForShadowMap( Matrix44 const &viewProjMatrix );

public:
	virtual void Update( float deltaSeconds );
	virtual void Render( Renderer &currentRenderer ) const;
			void EnablePerimeterRotationAround( Vector3 const &rotateAround, float withRadius );
			void DisablePerimeterRotation();

private:
	bool		m_perimeterMovementEnabled = false;
	float		m_perimeterRadius;
	Vector3		m_rotateAroundLocation;

	void		PerformPerimeterRotation( float deltaSeconds );
};