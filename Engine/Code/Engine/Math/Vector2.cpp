#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const Vector2& copy )
{
	x = copy.x;
	y = copy.y;
}


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( float initialX, float initialY )
{
	x = initialX;
	y = initialY;
}

Vector2::Vector2( const IntVector2& copyFrom )
{
	x = (float) copyFrom.x;
	y = (float) copyFrom.y;
}

Vector2 Vector2::ZERO		= Vector2(  0.f,  0.f );
Vector2 Vector2::ONE_ONE	= Vector2(  1.f,  1.f );
Vector2 Vector2::TOP_DIR	= Vector2(  0.f,  1.f );
Vector2 Vector2::LEFT_DIR	= Vector2( -1.f,  0.f );
Vector2 Vector2::RIGHT_DIR	= Vector2(  1.f,  0.f );
Vector2 Vector2::BOTTOM_DIR = Vector2(  0.f, -1.f );

//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator + ( const Vector2& vecToAdd ) const
{
	return Vector2( (x + vecToAdd.x), (y + vecToAdd.y) );
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator-( const Vector2& vecToSubtract ) const
{
	return Vector2( (x - vecToSubtract.x), (y - vecToSubtract.y) );
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator*( float uniformScale ) const
{
	return Vector2( (x * uniformScale), (y *  uniformScale) );
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::operator/( float inverseScale ) const
{
	return Vector2( (x / inverseScale), (y / inverseScale) );
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator+=( const Vector2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator-=( const Vector2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vector2::operator=( const Vector2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vector2 operator*( float uniformScale, const Vector2& vecToScale )
{
	return Vector2( (vecToScale.x * uniformScale), (vecToScale.y * uniformScale) );
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator==( const Vector2& compare ) const
{
	bool areEqual = false;
	if(x == compare.x && y == compare.y) {
		areEqual = true;
	}

	return areEqual;
}


//-----------------------------------------------------------------------------------------------
bool Vector2::operator!=( const Vector2& compare ) const
{
	bool areDiffrent = true;
	if(x == compare.x && y == compare.y) {
		areDiffrent = false;
	}

	return areDiffrent;
}

float Vector2::GetLengthSquared() const {
	return ( (x*x) + (y*y) );
}

float Vector2::GetLength() const {
	return sqrtf(GetLengthSquared());
}

Vector2 Vector2::GetNormalized() const {
	Vector2 tempVec = *this;
	float lengthOfVector = tempVec.GetLength();
	if(lengthOfVector != 0) {
		float multiplier = 1.f / tempVec.GetLength();
		tempVec *= multiplier;
	} else {
		// Vector's length is zero, return a unit vector at theta 0 degree
		tempVec = Vector2(1, 0);
	}

	return ( tempVec );
}

float Vector2::NormalizeAndGetLength() {
	float lengthOfThis = this->GetLength();
	*this = this->GetNormalized();

	return lengthOfThis;
}

void Vector2::RotateByDegreesClockwise( float degrees )
{
	RotateByDegreesAntiClockwise( -degrees );
}

void Vector2::RotateByDegreesAntiClockwise( float degrees )
{
	float vLength	= GetLength();
	float vAngle	= GetOrientationDegrees();

	x = vLength * CosDegree( vAngle + degrees );
	y = vLength * SinDegree( vAngle + degrees );
}

float Vector2::GetOrientationDegrees() const {
	return atan2fDegree(y, x);
}

Vector3 Vector2::GetAsVector3() const 
{
	return Vector3( x, y, 0.f );
}

Vector2 Vector2::MakeDirectionAtDegrees(float degree) {
	float radian = (degree * M_PI) / 180.f;

	return Vector2(cosf(radian), sinf(radian));
}


float Vector2::GetDistanceSquared(const Vector2& pos1, const Vector2& pos2) {
	return ( (pos1.x - pos2.x)*(pos1.x - pos2.x)) + ((pos1.y - pos2.y)*(pos1.y - pos2.y) );
}

float Vector2::GetDistance(const Vector2& pos1, const Vector2& pos2) {
	return sqrtf(Vector2::GetDistanceSquared(pos1, pos2));
}

float Vector2::DotProduct( const Vector2& a, const Vector2& b ) {
	float scalerProduct = ( a.x * b.x ) + ( a.y * b.y );

	return scalerProduct;
}

float Vector2::DotProductForAngleDifference( float angleInDegrees ) {
	float firstAngle = 0.f;
	float secondAngle = angleInDegrees;

	Vector2 firstVector = Vector2( CosDegree(firstAngle) , SinDegree(firstAngle) );
	Vector2 secondVector = Vector2( CosDegree(secondAngle) , SinDegree(secondAngle) );

	return DotProduct( firstVector , secondVector );
}


void Vector2::SetFromText( const char* text )
{
	std::string inputStr = text;
	int firstCommaAt = (int) inputStr.find(',');

	std::string first(inputStr , 0 , firstCommaAt);
	::SetFromText( this->x , first.c_str() );
	std::string second(inputStr , firstCommaAt+1);
	::SetFromText( this->y , second.c_str() );
}



const Vector2 GetProjectedVector( const Vector2& vectorToProject, const Vector2& projectOnto )
{
	Vector2 normalizedProjectOnto = projectOnto.GetNormalized();
	float lengthOfProjection = Vector2::DotProduct( vectorToProject , normalizedProjectOnto );
	Vector2 resultVec = normalizedProjectOnto * lengthOfProjection;

	return resultVec;
}

const Vector2 GetTransformedIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ )
{
	float projILength = Vector2::DotProduct( originalVector , newBasisI );
	float projJLenght = Vector2::DotProduct( originalVector , newBasisJ );;

	Vector2 newIComponent = projILength * Vector2( 1 , 0 );		// New I axis' direction is (1,0) now
	Vector2 newJComponent = projJLenght * Vector2( 0 , 1 );		// New J axis' direction is (0,1) now

	Vector2 resultVec = newIComponent + newJComponent;

	return resultVec;
}

const Vector2 GetTransformedOutOfBasis( const Vector2& vectorInBasis, const Vector2& oldBasisI, const Vector2& oldBasisJ )
{
	float newXComponent = vectorInBasis.x * oldBasisI.x + vectorInBasis.y * oldBasisJ.x;		// vectorInBasis.i * oldBasisI.x + vectorInBasis.j * oldBasisJ.x
	float newYComponent = vectorInBasis.x * oldBasisI.y + vectorInBasis.y * oldBasisJ.y;		// vectorInBasis.i * oldBasisI.y + vectorInBasis.j * oldBasisJ.y

	Vector2 resultVec = Vector2(newXComponent, newYComponent);

	return resultVec;
}

void DecomposeVectorIntoBasis( const Vector2& originalVector, const Vector2& newBasisI, const Vector2& newBasisJ, Vector2& out_vectorAlongI, Vector2& out_vectorAlongJ )
{
	out_vectorAlongI = GetProjectedVector( originalVector , newBasisI );
	out_vectorAlongJ = GetProjectedVector( originalVector , newBasisJ );
}


const Vector2 Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd )
{
	Vector2 resultVec;

	resultVec.x = Interpolate( start.x , end.x , fractionTowardEnd );
	resultVec.y = Interpolate( start.y , end.y , fractionTowardEnd );

	return resultVec;
}


Vector2 Reflect( const Vector2& incomingVector , const Vector2& normalVector )
{
	Vector2 reflectionVComp = GetProjectedVector(incomingVector , normalVector);
	Vector2 reflectionVec = incomingVector - ( 2.f * reflectionVComp );
	
	return reflectionVec;
}

Vector2 PolarToCartesian( float r, float degrees )
{
	float x = r * CosDegree( degrees );
	float y = r * SinDegree( degrees );

	return Vector2( x, y );
}