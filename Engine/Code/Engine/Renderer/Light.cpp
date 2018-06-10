#pragma once
#include "Light.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Renderer.hpp"

Light::Light()
{
	// Setup the Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );
	
	Mesh* lightMesh		= MeshBuilder::CreateCube( Vector3( 0.25f, 0.25f, 0.25f ) );
	Shader* defShader	= Shader::CreateNewFromFile( "Data//Shaders//default.shader" );
	Material* lightMat	= new Material( *defShader );

	m_renderable->SetBaseMesh( lightMesh );
	m_renderable->SetBaseMaterial( lightMat );
}

Light::Light( Vector3 const &position, Vector3 const &eulerRotation /* = Vector3::ZERO */ )
{
	m_transform.SetPosition( position );
	m_transform.SetRotation( eulerRotation );

	m_direction = m_transform.GetRotation().GetAsDirection();


	// Setup the Renderable
	m_renderable = new Renderable();
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	Mesh* lightMesh		= MeshBuilder::CreateCube( Vector3::ONE_ALL, Vector3::ZERO );
	Material* lightMat	= Material::CreateNewFromFile( "Data//Materials//default.material" );

	m_renderable->SetBaseMesh( lightMesh );
	m_renderable->SetBaseMaterial( lightMat );
}

Light::~Light()
{
	delete m_renderable;
}

void Light::Update( float deltaSeconds )
{
	if( m_perimeterMovementEnabled )
		PerformPerimeterRotation( deltaSeconds );
}

void Light::Render( Renderer &currentRenderer ) const
{
	currentRenderer.SetCurrentDiffuseTexture( nullptr );
	currentRenderer.SetCurrentNormalTexture( nullptr );
	currentRenderer.SetCurrentEmissiveTexture( nullptr );
	currentRenderer.DrawMesh( *m_renderable->GetMesh(), m_transform.GetWorldTransformMatrix() );
}

void Light::EnablePerimeterRotationAround( Vector3 const &rotateAround, float withRadius )
{
	m_perimeterMovementEnabled	= true;
	m_rotateAroundLocation		= rotateAround;
	m_perimeterRadius			= withRadius;
}

void Light::DisablePerimeterRotation()
{
	m_perimeterMovementEnabled	= false;
}

void Light::PerformPerimeterRotation( float deltaSeconds )
{
	static Vector3		polarPosition		= Vector3( m_perimeterRadius, 0.f, 0.f );
	static float const	rotationSpeed		= 20.f;	// Degrees per seconds
	static float const	altitudeChangeSpeed = 12.f;	// Degrees per seconds
	static float		timeElapsed			= 0.f;
	timeElapsed += deltaSeconds;

	polarPosition.y = (polarPosition.y > 360.f) ? 0.f : polarPosition.y + (rotationSpeed * deltaSeconds);
	polarPosition.z = SinDegree( timeElapsed * altitudeChangeSpeed );
	polarPosition.z = RangeMapFloat( polarPosition.z, -1.f, 1.f, 70.f, 110.f );

	Vector3 newLightPosition =  m_rotateAroundLocation + PolarToCartesian( polarPosition.x, polarPosition.y, polarPosition.z );
	m_transform.SetPosition( newLightPosition );
}

LightData Light::GetLightDataForUBO() const
{
	LightData lightData;
	
	lightData.colorAndIntensity	= m_lightColorAndIntensity;
	lightData.position			= m_transform.GetWorldPosition();
	lightData.direction			= m_direction;
	lightData.directionFactor	= m_directionFactor;
	lightData.attenuation		= m_attenuation;
	lightData.dotInnerAngle		= m_dotInnerAngle;
	lightData.dotOuterAngle		= m_dotOuterAngle;

	return lightData;
}

float Light::GetAttenuationForRenderableAt( Vector3 const &renderablePos ) const
{
	// Direction and Distance
	Vector3		directionToLight	= m_transform.GetWorldPosition() - renderablePos;
	float const distanceFromLight	= directionToLight.GetLength();
				directionToLight	= directionToLight.GetNormalized();

	Vector3		lightForwardDir		= m_direction.GetNormalized();
	float		lightPower			= m_lightColorAndIntensity.w;
	
	// Get actual Light Direction
	directionToLight				= Interpolate( lightForwardDir * -1.f, directionToLight, m_directionFactor );

	// Light Power
	float const dotAngle			= Vector3::DotProduct( lightForwardDir, directionToLight * -1.f );
				lightPower			= lightPower * dotAngle;
	TODO("Ask Forseth and correct this implemntation where inner & outer angles are taken in account");

	// Main Calculation
	float const a = m_attenuation.x;
	float const b = distanceFromLight * m_attenuation.y;
	float const c = distanceFromLight * distanceFromLight * m_attenuation.z;

	return lightPower / ( a + b + c );
}

Vector3 Light::GetPosition() const
{
	return m_transform.GetWorldPosition();
}

Vector3 Light::GetEulerRotation() const
{
	return m_transform.GetRotation();
}

void Light::SetPosition( Vector3 const &newPosition )
{
	m_transform.SetPosition( newPosition );
}

void Light::SetPosition( float x, float y, float z )
{
	m_transform.SetPosition( Vector3( x, y, z) );
}

void Light::SetEulerRotation( Vector3 const &newRotation )
{
	m_transform.SetRotation( newRotation );
	m_direction = m_transform.GetRotation().GetAsDirection();
}

void Light::SetEulerRotation( float aroundX, float aroundY, float aroundZ )
{
	m_transform.SetRotation( Vector3( aroundX, aroundY, aroundZ ) );
	m_direction = m_transform.GetRotation().GetAsDirection();
}

void Light::SetUpForPointLight( float intensity, Vector3 const &attenuationConstants /*= Vector3( 0.f, 0.f, 1.f )*/, Rgba const &theColor /*= RGBA_WHITE_COLOR */ )
{
	m_transform.RemoveChild( &m_renderable->m_modelTransform );

	// Reset the mesh of Renderable, according to Light Type & color
	delete m_renderable->m_meshes[0];
	m_renderable->m_meshes[0]	= MeshBuilder::CreateSphere( 1.f, 10, 10, Vector3::ZERO, theColor );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	m_attenuation				= attenuationConstants;
	m_lightColorAndIntensity	= Vector4( theColor.GetAsNormalizedRgba().IgnoreW(), intensity );

	m_direction					= Vector3( 0.f, 0.f, 1.f );
	m_directionFactor			= 0.f;

	m_dotInnerAngle				= 0.f;
	m_dotOuterAngle				= 360.f;
}

void Light::SetUpForSpotLight( float intensity, float innerAngle, float outerAngle, Vector3 const &attenuationConstants /*= Vector3( 0.f, 0.f, 1.f )*/, Rgba const &theColor /*= RGBA_WHITE_COLOR */ )
{
	m_transform.RemoveChild( &m_renderable->m_modelTransform );

	// Reset the mesh of Renderable, according to Light Type & color
	delete m_renderable->m_meshes[0];
	m_renderable->m_meshes[0]		= MeshBuilder::CreateCube( Vector3( 1.f, 1.f, 0.2f ), Vector3::ZERO, theColor );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	m_attenuation					= attenuationConstants;
	m_lightColorAndIntensity		= Vector4( theColor.GetAsNormalizedRgba().IgnoreW(), intensity );

	m_direction						= m_transform.GetRotation().GetAsDirection();
	m_directionFactor				= 1.f;

	m_dotInnerAngle					= CosDegree( innerAngle * 0.5f );
	m_dotOuterAngle					= CosDegree( outerAngle * 0.5f );
}

void Light::SetUpForDirectionalLight( float intensity, Vector3 const &attenuationConstants /* = Vector3( 0.f, 0.f, 1.f ) */, Rgba const &theColor /* = RGBA_WHITE_COLOR */ )
{
	m_transform.RemoveChild( &m_renderable->m_modelTransform );

	// Reset the mesh of Renderable, according to Light Type & color
	delete m_renderable->m_meshes[0];
	m_renderable->m_meshes[0]	= MeshBuilder::CreateCube( Vector3( 0.4f, 0.4f, 1.f ), Vector3::ZERO, theColor );
	m_renderable->m_modelTransform.SetParentAs( &m_transform );

	m_attenuation				= attenuationConstants;
	m_lightColorAndIntensity	= Vector4( theColor.GetAsNormalizedRgba().IgnoreW(), intensity );

	m_direction					= m_transform.GetRotation().GetAsDirection();
	m_directionFactor			= 1.f;

	m_dotInnerAngle				= 0.f;
	m_dotOuterAngle				= 360.f;
}