#include "Matrix44.hpp"

Matrix44::Matrix44( const float* sixteenValuesBasisMajor )
{
	Ix = sixteenValuesBasisMajor[0];  Jx = sixteenValuesBasisMajor[4];  Kx = sixteenValuesBasisMajor[8];  Tx = sixteenValuesBasisMajor[12]; 
	Iy = sixteenValuesBasisMajor[1];  Jy = sixteenValuesBasisMajor[5];  Ky = sixteenValuesBasisMajor[9];  Ty = sixteenValuesBasisMajor[13]; 
	Iz = sixteenValuesBasisMajor[2];  Jz = sixteenValuesBasisMajor[6];  Kz = sixteenValuesBasisMajor[10]; Tz = sixteenValuesBasisMajor[14]; 
	Iw = sixteenValuesBasisMajor[3];  Jw = sixteenValuesBasisMajor[7];	Kw = sixteenValuesBasisMajor[11]; Tw = sixteenValuesBasisMajor[15];	
}

Matrix44::Matrix44( const Vector2& iBasis, const Vector2& jBasis, const Vector2& translation /* = Vector2(0.f,0.f) */ )
{
	Ix = iBasis.x;		Jx = jBasis.x;		Kx = 0;		Tx = translation.x; 
	Iy = iBasis.y;		Jy = jBasis.y;		Ky = 0;		Ty = translation.y; 
	Iz = 0;				Jz = 0;				Kz = 1;		Tz = 0;		
	Iw = 0;				Jw = 0;				Kw = 0;		Tw = 1;
}

Vector2 Matrix44::TransformPosition2D( const Vector2& position2D )
{
	Vector2 transformedPos2D = Vector2( Ix*position2D.x + Jx*position2D.y + Kx*0.f + Tx*1.f, 
										Iy*position2D.x + Jy*position2D.y + Ky*0.f + Ty*1.f );
	return transformedPos2D;
}

Vector2 Matrix44::TransformDisplacement2D( const Vector2& displacement2D )
{
	Vector2 vecToReturn = Vector2( Ix*displacement2D.x + Jx*displacement2D.y + Kx*0.f + Tx*0.f, 
								   Iy*displacement2D.x + Jy*displacement2D.y + Ky*0.f + Ty*0.f );
	return vecToReturn;
}

void Matrix44::SetIdentity()
{
	*this = Matrix44();
}

void Matrix44::SetValues( const float* sixteenValuesBasisMajor )
{
	*this = Matrix44( sixteenValuesBasisMajor );
}

void Matrix44::Append( const Matrix44& matrixToAppend )
{
	float old_Ix = Ix,	old_Iy = Iy,	old_Iz = Iz,	old_Iw = Iw,
		  old_Jx = Jx,	old_Jy = Jy,	old_Jz = Jz,	old_Jw = Jw,
		  old_Kx = Kx,	old_Ky = Ky,	old_Kz = Kz,	old_Kw = Kw,
		  old_Tx = Tx,	old_Ty = Ty,	old_Tz = Tz,	old_Tw = Tw;

	Ix = (old_Ix * matrixToAppend.Ix) + (old_Jx * matrixToAppend.Iy) + (old_Kx * matrixToAppend.Iz) + (old_Tx * matrixToAppend.Iw);
	Iy = (old_Iy * matrixToAppend.Ix) + (old_Jy * matrixToAppend.Iy) + (old_Ky * matrixToAppend.Iz) + (old_Ty * matrixToAppend.Iw);
	Iz = (old_Iz * matrixToAppend.Ix) + (old_Jz * matrixToAppend.Iy) + (old_Kz * matrixToAppend.Iz) + (old_Tz * matrixToAppend.Iw);
	Iw = (old_Iw * matrixToAppend.Ix) + (old_Jw * matrixToAppend.Iy) + (old_Kw * matrixToAppend.Iz) + (old_Tw * matrixToAppend.Iw);

	Jx = (old_Ix * matrixToAppend.Jx) + (old_Jx * matrixToAppend.Jy) + (old_Kx * matrixToAppend.Jz) + (old_Tx * matrixToAppend.Jw);
	Jy = (old_Iy * matrixToAppend.Jx) + (old_Jy * matrixToAppend.Jy) + (old_Ky * matrixToAppend.Jz) + (old_Ty * matrixToAppend.Jw);
	Jz = (old_Iz * matrixToAppend.Jx) + (old_Jz * matrixToAppend.Jy) + (old_Kz * matrixToAppend.Jz) + (old_Tz * matrixToAppend.Jw);
	Jw = (old_Iw * matrixToAppend.Jx) + (old_Jw * matrixToAppend.Jy) + (old_Kw * matrixToAppend.Jz) + (old_Tw * matrixToAppend.Jw);

	Kx = (old_Ix * matrixToAppend.Kx) + (old_Jx * matrixToAppend.Ky) + (old_Kx * matrixToAppend.Kz) + (old_Tx * matrixToAppend.Kw);
	Ky = (old_Iy * matrixToAppend.Kx) + (old_Jy * matrixToAppend.Ky) + (old_Ky * matrixToAppend.Kz) + (old_Ty * matrixToAppend.Kw);
	Kz = (old_Iz * matrixToAppend.Kx) + (old_Jz * matrixToAppend.Ky) + (old_Kz * matrixToAppend.Kz) + (old_Tz * matrixToAppend.Kw);
	Kw = (old_Iw * matrixToAppend.Kx) + (old_Jw * matrixToAppend.Ky) + (old_Kw * matrixToAppend.Kz) + (old_Tw * matrixToAppend.Kw);

	Tx = (old_Ix * matrixToAppend.Tx) + (old_Jx * matrixToAppend.Ty) + (old_Kx * matrixToAppend.Tz) + (old_Tx * matrixToAppend.Tw);
	Ty = (old_Iy * matrixToAppend.Tx) + (old_Jy * matrixToAppend.Ty) + (old_Ky * matrixToAppend.Tz) + (old_Ty * matrixToAppend.Tw);
	Tz = (old_Iz * matrixToAppend.Tx) + (old_Jz * matrixToAppend.Ty) + (old_Kz * matrixToAppend.Tz) + (old_Tz * matrixToAppend.Tw);
	Tw = (old_Iw * matrixToAppend.Tx) + (old_Jw * matrixToAppend.Ty) + (old_Kw * matrixToAppend.Tz) + (old_Tw * matrixToAppend.Tw);

}

void Matrix44::Transpose()
{
	Matrix44 originalMatrix = *this;

	Ix = originalMatrix.Ix;		Jx = originalMatrix.Iy;		Kx = originalMatrix.Iz;		Tx = originalMatrix.Iw;
	Iy = originalMatrix.Jx;		Jy = originalMatrix.Jy;		Ky = originalMatrix.Jz;		Ty = originalMatrix.Jw;
	Iz = originalMatrix.Kx;		Jz = originalMatrix.Ky;		Kz = originalMatrix.Kz;		Tz = originalMatrix.Kw;
	Iw = originalMatrix.Tx;		Jw = originalMatrix.Ty;		Kw = originalMatrix.Tz;		Tw = originalMatrix.Tw;
}

Vector3 Matrix44::Multiply( const Vector3& vecToMultiply, const float w )
{

	Vector3 toReturn;
	toReturn.x	= ( Ix * vecToMultiply.x ) + ( Jx * vecToMultiply.y ) + ( Kx * vecToMultiply.z ) + ( Tx * w );
	toReturn.y	= ( Iy * vecToMultiply.x ) + ( Jy * vecToMultiply.y ) + ( Ky * vecToMultiply.z ) + ( Ty * w );
	toReturn.z	= ( Iz * vecToMultiply.x ) + ( Jz * vecToMultiply.y ) + ( Kz * vecToMultiply.z ) + ( Tz * w );

	return toReturn;
}

void Matrix44::RotateDegrees2D( float rotationDegreesAboutZ )
{
	float rotationMatrix[16] =	{	 CosDegree( rotationDegreesAboutZ ), SinDegree( rotationDegreesAboutZ ), 0.f,		 0.f,
									-SinDegree( rotationDegreesAboutZ ), CosDegree( rotationDegreesAboutZ ), 0.f,		 0.f,
									 0.f,								 0.f,								 1.f,		 0.f,
									 0.f,								 0.f,								 0.f,		 1.f	};

	Matrix44 rotationMatrix44 = Matrix44( rotationMatrix );

	Append( rotationMatrix44 );
}

void Matrix44::Translate2D( const Vector2& translation )
{
	Matrix44 translationMatrix44;
	translationMatrix44.Tx = translation.x;
	translationMatrix44.Ty = translation.y;

	Append( translationMatrix44 );
}

void Matrix44::ScaleUniform2D( float scaleXY )
{
	Matrix44 scaleMatrix44;
	scaleMatrix44.Ix = scaleXY;
	scaleMatrix44.Jy = scaleXY;

	Append( scaleMatrix44 );
}

void Matrix44::Translate3D( Vector3 const &translation )
{
	Matrix44 translateMatrix;
	translateMatrix.Tx = translation.x;
	translateMatrix.Ty = translation.y;
	translateMatrix.Tz = translation.z;
	
	Append( translateMatrix );
}

void Matrix44::RotateDegrees3D( Vector3 const &rotateAroundAxisZXY )
{
	Matrix44 rotationAroundZMatrix;			// Clockwise
	rotationAroundZMatrix.Ix =  CosDegree( rotateAroundAxisZXY.z );
	rotationAroundZMatrix.Jx = -SinDegree( rotateAroundAxisZXY.z );
	rotationAroundZMatrix.Iy =  SinDegree( rotateAroundAxisZXY.z );
	rotationAroundZMatrix.Jy =  CosDegree( rotateAroundAxisZXY.z );

	Matrix44 rotationAroundXMatrix;			// Clockwise
	rotationAroundXMatrix.Jy =  CosDegree( rotateAroundAxisZXY.x );
	rotationAroundXMatrix.Ky = -SinDegree( rotateAroundAxisZXY.x );
	rotationAroundXMatrix.Jz =  SinDegree( rotateAroundAxisZXY.x );
	rotationAroundXMatrix.Kz =  CosDegree( rotateAroundAxisZXY.x );

	Matrix44 rotationAroundYMatrix;			// Counter-Clockwise
	rotationAroundYMatrix.Ix =  CosDegree( rotateAroundAxisZXY.y );
	rotationAroundYMatrix.Kx =  SinDegree( rotateAroundAxisZXY.y );
	rotationAroundYMatrix.Iz = -SinDegree( rotateAroundAxisZXY.y );
	rotationAroundYMatrix.Kz =  CosDegree( rotateAroundAxisZXY.y );

	Append( rotationAroundZMatrix );
	Append( rotationAroundXMatrix );
	Append( rotationAroundYMatrix );
}

void Matrix44::Scale3D( Vector3 const &scale )
{
	Matrix44 scaleMatrix;
	scaleMatrix.Ix = scale.x;
	scaleMatrix.Jy = scale.y;
	scaleMatrix.Kz = scale.z;

	Append( scaleMatrix );
}

void Matrix44::ScaleUniform3D( float uniformScale )
{
	Matrix44 scaleMatrix;
	scaleMatrix.Ix = uniformScale;
	scaleMatrix.Jy = uniformScale;
	scaleMatrix.Kz = uniformScale;

	Append( scaleMatrix );
}

void Matrix44::Scale2D( float scaleX, float scaleY )
{
	float scaleMatrix[16] = {	scaleX,		0.f,		0.f,	0.f,
								0.f,		scaleY,		0.f,	0.f,
								0.f,		0.f,		1.f,	0.f,
								0.f,		0.f,		0.f,	1.f	 };

	Matrix44 scaleMatrix44 = Matrix44( scaleMatrix );
	Append( scaleMatrix44 );
}

Matrix44 Matrix44::MakeRotationDegrees2D( float rotationDegreesAboutZ )
{
	Matrix44 toReturn;
	toReturn.RotateDegrees2D( rotationDegreesAboutZ );

	return toReturn;
}

Matrix44 Matrix44::MakeTranslation2D( const Vector2& translation )
{
	Matrix44 toReturn;
	toReturn.Translate2D( translation );

	return toReturn;
}

Matrix44 Matrix44::MakeScaleUniform2D( float scaleXY )
{
	Matrix44 toReturn;
	toReturn.ScaleUniform2D( scaleXY );

	return toReturn;
}

Matrix44 Matrix44::MakeScale2D( float scaleX, float scaleY )
{
	Matrix44 toReturn;
	toReturn.Scale2D( scaleX, scaleY );

	return toReturn;
}

Matrix44 Matrix44::MakeOrtho2D( const Vector2& bottomLeft, const Vector2& topRight )
{
	Matrix44 toReturn;

	/////////////////////////////////////////////////////////////////////////////////////
	//       Calculations are done according to the info provided by the link,         //
	//---------------------------------------------------------------------------------//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd373965(v=vs.85).aspx //
	//                                                                                 //
	/////////////////////////////////////////////////////////////////////////////////////

	// Translate it by
	toReturn.Tx = -(topRight.x + bottomLeft.x)/(topRight.x - bottomLeft.x);
	toReturn.Ty = -(topRight.y + bottomLeft.y)/(topRight.y - bottomLeft.y);
	toReturn.Tz = -(1.f + 0.f)/(1.f - 0.f);

	// Scale it down by
	float xScale	=  2.f / ( topRight.x - bottomLeft.x );
	float yScale	=  2.f / ( topRight.y - bottomLeft.y );
	float zScale	=  2.f / ( 1.f - 0.f);							// On MSDN, it is -zScale. Which is wrong

	toReturn.Ix = xScale;
	toReturn.Jy = yScale;
	toReturn.Kz = zScale;

	return toReturn;
}

Matrix44 Matrix44::MakeOrtho3D( float screen_width, float screen_height, float screen_near, float screen_far )
{

	Matrix44 toReturn;

	/////////////////////////////////////////////////////////////////////////////////////
	//       Calculations are done according to the info provided by the link,         //
	//---------------------------------------------------------------------------------//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dd373965(v=vs.85).aspx //
	//                                                                                 //
	/////////////////////////////////////////////////////////////////////////////////////

	const float		half_width	=  screen_width * 0.5f;
	const float		half_height	=  screen_height * 0.5f;
	const float		left		= -half_width;
	const float&	right		=  half_width;
	const float&	top			=  half_height;
	const float		bottom		= -half_height;

	// Translate it by
	toReturn.Tx = -(right + left)/(right - left);
	toReturn.Ty = -(top + bottom) / (top - bottom);
	toReturn.Tz = -(screen_far + screen_near) / (screen_far - screen_near);

	// Scale it down by
	float xScale	=  2.f / ( right - left );
	float yScale	=  2.f / ( top - bottom );
	float zScale	=  2.f / ( screen_far - screen_near);			// On MSDN, it is -zScale. Which is wrong

	toReturn.Ix = xScale;
	toReturn.Jy = yScale;
	toReturn.Kz = zScale;

	return toReturn;
}

Matrix44 Matrix44::MakePerspective3D( float fovDegrees, float aspectRatio, float nearZ, float farZ )
{
	float fovRadians = DegreeToRadian( fovDegrees * 0.5f );
	float d = 1.f / atan2f( fovRadians, 1.f );
	float q = 1.f / ( farZ - nearZ );

	float array[16] = {
		// I Column
		 d / aspectRatio,	
		 0.f,	
		 0.f,
		 0.f,

		// J Column
		 0.f,
		 d,
		 0.f,
		 0.f,

		// K Column
		 0.f,
		 0.f,
		 (nearZ + farZ) * q,
		 1.f,

		// T Column
		 0.f,
		 0.f,
		-2.f * nearZ * farZ * q,
		 0.f
	};

	return Matrix44( array );
}

Matrix44 Matrix44::MakeLookAtView( const Vector3& target_position, const Vector3& camera_position, const Vector3& camera_up_vector /* = Vector3( 0.f, 1.f, 0.f ) */ )
{
	Matrix44 toReturn;

	/////////////////////////////////////////////////////////////////////////////////////
	//       Calculations are done according to the info provided by the link,         //
	//---------------------------------------------------------------------------------//
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb281710(v=vs.85).aspx //
	//                                                                                 //
	/////////////////////////////////////////////////////////////////////////////////////

	Vector3 zaxis = (target_position - camera_position).GetNormalized();
	Vector3 xaxis = Vector3::CrossProduct(camera_up_vector, zaxis).GetNormalized();
	Vector3 yaxis = Vector3::CrossProduct(zaxis, xaxis);
	Vector3 taxis = Vector3( Vector3::DotProduct(xaxis, camera_position),
							 Vector3::DotProduct(yaxis, camera_position),
							 Vector3::DotProduct(zaxis, camera_position) );
	
	toReturn.Ix	= xaxis.x;
	toReturn.Iy = yaxis.x;
	toReturn.Iz = zaxis.x;
	toReturn.Iw = 0.f;

	toReturn.Jx	= xaxis.y;
	toReturn.Jy = yaxis.y;
	toReturn.Jz = zaxis.y;
	toReturn.Jw = 0.f;

	toReturn.Kx	= xaxis.z;
	toReturn.Ky = yaxis.z;
	toReturn.Kz = zaxis.z;
	toReturn.Kw = 0.f;

	toReturn.Tx	= Vector3::DotProduct(xaxis, camera_position) * -1.f;
	toReturn.Ty = Vector3::DotProduct(yaxis, camera_position) * -1.f;
	toReturn.Tz = Vector3::DotProduct(zaxis, camera_position) * -1.f;
	toReturn.Tw = 1.f;

	/*
	RESULT MATRIX,
		__																							  __
		|	xaxis.x						  yaxis.x						zaxis.x						0  |
		|	xaxis.y						  yaxis.y						zaxis.y						0  |
		|	xaxis.z						  yaxis.z						zaxis.z						0  |
		|	-dot(xaxis, cameraPosition)  -dot(yaxis, cameraPosition)   -dot(zaxis, cameraPosition)  1  |
		--																							  --
	*/

	return toReturn;
}

Vector3 Matrix44::GetIColumn() const
{
	return Vector3( Ix, Iy, Iz );
}

Vector3 Matrix44::GetJColumn() const
{
	return Vector3( Jx, Jy, Jz );
}

Vector3 Matrix44::GetKColumn() const
{
	return Vector3( Kx, Ky, Kz );
}

Vector3 Matrix44::GetTColumn() const
{
	return Vector3( Tx, Ty, Tz );
}

Vector3 Matrix44::GetEulerRotation() const
{
	/*
		ASSUMING THAT ROTATION MATRIX IS,

		--													  --     --			   --
		|	 cz*cy - sz*sx*sy	-sz*cx		cz*sy + sz*sx*cy   |     |	Ix	Jx	Kx	|
		|	 sz*cy + cz*sx*sy	 cz*cx		sz*sy - cz*sx*cy   |  =  |	Iy	Jy	Ky	|
		|	-sz*cx				 sx			cx*cy			   |     |	Iz	Jz	Kz	|
		--													  --     --			   --
	*/

	float xRad;
	float yRad;
	float zRad;

	float sx = ClampFloat( Jz, -1.f, 1.f );
	xRad	 = asinf( sx );

	float cx = cosf( xRad );
	if( cx != 0.f )
	{
		yRad = atan2f( -Iz, Kz );
		zRad = atan2f( -Jx, Jy );
	}
	else
	{
		zRad = 0.f;
		yRad = atan2f( Kx, Ix );
	}

	return Vector3( RadianToDegree(xRad), 
					RadianToDegree(yRad), 
					RadianToDegree(zRad) );
}

Matrix44 Matrix44::GetOrthonormalInverse() const
{
	Matrix44 toReturn	= *this;
	Vector3  traslation	= toReturn.GetTColumn();

	toReturn.Tx = 0.f;
	toReturn.Ty = 0.f;
	toReturn.Tz = 0.f;
	toReturn.Tw = 1.f;

	toReturn.Transpose();
	Vector3 inverseTranslation = toReturn.Multiply( traslation * -1.f, 0.f );

	toReturn.Tx = inverseTranslation.x;
	toReturn.Ty = inverseTranslation.y;
	toReturn.Tz = inverseTranslation.z;
	toReturn.Tw = 1.f;

	return toReturn;
}