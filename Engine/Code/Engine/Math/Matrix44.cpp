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
	float zScale	= -2.f / ( 1.f - 0.f);

	toReturn.Ix = xScale;
	toReturn.Jy = yScale;
	toReturn.Kz = zScale;

	return toReturn;
}