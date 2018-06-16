#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/MathUtil.hpp"

class Matrix44
{
public:
	float Ix = 1.0f; // "basis-major", i.e. I-terms come first (x,y,z,w)
	float Iy = 0.0f;
	float Iz = 0.0f;
	float Iw = 0.0f;

	float Jx = 0.0f;
	float Jy = 1.0f;
	float Jz = 0.0f;
	float Jw = 0.0f;

	float Kx = 0.0f;
	float Ky = 0.0f;
	float Kz = 1.0f;
	float Kw = 0.0f;

	float Tx = 0.0f;
	float Ty = 0.0f;
	float Tz = 0.0f;
	float Tw = 1.0f;

	Matrix44() {} // Identity matrix
	explicit Matrix44( const float* sixteenValuesBasisMajor ); // float[16] array in order Ix, Iy...
	explicit Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation = Vector2(0.f,0.f) );

	// Accessors
	Vector2 TransformPosition2D( const Vector2& position2D );				// Written assuming z=0, w=1
	Vector2 TransformDisplacement2D( const Vector2& displacement2D );		// Written assuming z=0, w=0

	// Mutators
	void SetIdentity();
	void SetValues	( const float* sixteenValuesBasisMajor );				// float[16] array in order Ix, Iy...
	void Append		( const Matrix44& matrixToAppend );						// a.k.a. Concatenate (right-multiply)
	void Transpose	();
	
	Vector3 Multiply	( const Vector3& vecToMultiply, const float w ) const;	// Mat44 * Vec4( x, y, z, w )
	Vector4	Multiply	( const Vector4& vecToMultiply ) const;

	// Modifiers
	void Translate2D	( const Vector2& translation );
	void RotateDegrees2D( float rotationDegreesAboutZ );
	void Scale2D		( float scaleX, float scaleY );
	void ScaleUniform2D	( float scaleXY );

	// Column Operations
	void SetIColumn( Vector3 iColumn );
	void SetJColumn( Vector3 jColumn );
	void SetKColumn( Vector3 kColumn );
	void NormalizeIJKColumns();

	void Translate3D	( Vector3 const &translation );
	void RotateDegrees3D( Vector3 const &rotateAroundAxisYXZ );				// Rotation Order, Around Axis:  Z(Roll, clockwise) --> X(Pitch, clockwise) --> Y(Yaw, counter-clockwise)
	void Scale3D		( Vector3 const &scale );
	void ScaleUniform3D	( float uniformScale );

	// Producers
	static Matrix44 MakeRotationDegrees2D	( float rotationDegreesAboutZ );
	static Matrix44 MakeTranslation2D		( const Vector2& translation );
	static Matrix44 MakeScaleUniform2D		( float scaleXY );
	static Matrix44 MakeScale2D				( float scaleX, float scaleY );
	static Matrix44 MakeOrtho2D				( const Vector2& bottomLeft, const Vector2& topRight );
	static Matrix44 MakeOrtho3D				( float screen_width, float screen_height, float screen_near, float screen_far );		// Center will be (0, 0, 0)
	static Matrix44 MakePerspective3D		( float fovDegrees, float aspectRatio, float nearZ, float farZ );		// Makes the Projection Matrix for Perspective Camera
	static Matrix44 MakeLookAtView			( const Vector3& target_position, const Vector3& camera_position, const Vector3& camera_up_vector = Vector3( 0.f, 1.f, 0.f ) );	// Returns View Matrix, according to provided Look At arguments
	
	// Column Accessors
	Vector3	GetIColumn() const;
	Vector3 GetJColumn() const;
	Vector3 GetKColumn() const;
	Vector3 GetTColumn() const;

	void	GetAsFloats( float (&outArray)[16] ) const;

	// Information Fetchers
	Vector3		GetEulerRotation() const;
	bool		GetInverse( Matrix44 &outInvMatrix ) const;
	Matrix44	GetOrthonormalInverse() const;
private:

};