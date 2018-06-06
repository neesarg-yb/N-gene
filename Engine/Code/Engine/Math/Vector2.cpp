#include "Engine/Math/Vector2.hpp"


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

float Vector2::GetOrientationDegrees() const {
	return MathUtil::atan2fDegree(y, x);
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

	Vector2 firstVector = Vector2( MathUtil::CosDegree(firstAngle) , MathUtil::SinDegree(firstAngle) );
	Vector2 secondVector = Vector2( MathUtil::CosDegree(secondAngle) , MathUtil::SinDegree(secondAngle) );

	return DotProduct( firstVector , secondVector );
}