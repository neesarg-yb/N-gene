#include "Complex.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Complex::Complex( float rotation )
	: r( CosDegree(rotation) )
	, i( SinDegree(rotation) )
{

}

Complex::Complex( float real, float imaginary )
	: r( real )
	, i( imaginary )
{

}

Complex::~Complex()
{

}

Complex Complex::operator+( Complex const &b ) const
{
	Complex sum( *this );

	sum.r += b.r;
	sum.i += b.i;

	return sum;
}

Complex Complex::operator-( Complex const &b ) const
{
	Complex sub( *this );

	sub.r -= b.r;
	sub.i -= b.i;

	return sub;
}

Complex Complex::operator*( Complex const &b ) const
{
	Complex const &a = *this;

	float multReal = ((a.r * b.r) - (a.i * b.i));
	float multImg  = ((a.r * b.i) + (a.i * b.r));

	return Complex( multReal, multImg );
}

Complex Complex::operator/( Complex const &b ) const
{
	Complex const &a = *this;

	float numeratorR  = ((a.r * b.r) + (a.i * b.i));
	float numeratorI  = ((a.i * b.r) - (a.r * b.i));
	float denominator = ((b.r * b.r) + (b.i * b.i));

	GUARANTEE_RECOVERABLE( denominator != 0, "Error in class Complex: Divide by ZERO!" );

	return Complex( numeratorR/denominator, numeratorI/denominator );
}

void Complex::operator+=( Complex const &b )
{
	Complex &a = *this;
	a = a + b;
}

void Complex::operator-=( Complex const &b )
{
	Complex &a = *this;
	a = a - b;
}

void Complex::operator*=( Complex const &b )
{
	Complex &a = *this;
	a = a * b;
}

void Complex::operator/=( Complex const &b )
{
	Complex &a = *this;
	a = a / b;
}

bool Complex::operator==( Complex const &b ) const
{
	Vector2 vec1( this->r, this->i );
	Vector2 vec2( b.r, b.i );
	float dotProduct = Vector2::DotProduct( vec1, vec2 );
	
	return AreEqualFloats( dotProduct, 1.f, 1U );
}

bool Complex::operator!=( Complex const &b ) const
{
	return !(b == *this);
}

float Complex::GetRotation() const
{
	return atan2fDegree( i, r );
}

float Complex::GetMagnitude() const
{
	return sqrtf( GetMagnitudeSquared() );
}

float Complex::GetMagnitudeSquared() const
{
	return ((r * r) + (i * i));
}

void Complex::TurnToward( Complex const &target, float maxRotationDegreesPositive )
{
	Complex &current( *this );
	Complex diff	= target/current;
	Complex maxRot	= Complex( maxRotationDegreesPositive );

	bool rotateByMax = maxRot.r > diff.r;			// i.e. max rotation is smaller than the difference
	bool cwRotation	 = diff.i < 0.f;				// clock-wise rotation?

	Complex rotator = rotateByMax ? maxRot : diff;

	// If we need to rotate in clock-wise direction
	if( rotateByMax && cwRotation )
		rotator.i *= -1.f;							// rotator = Complex( -maxRotationDegrees )

	current *= rotator;
}

Complex Complex::TurnTowardByFraction( Complex const &target, float fractionRotation ) const
{
	float degreesDifference	= fabsf( (*this / target).GetRotation() );
	float degreesRotation	= degreesDifference * fractionRotation;

	Complex toReturn( *this );
	toReturn.TurnToward( target, degreesRotation );

	return toReturn;
}
