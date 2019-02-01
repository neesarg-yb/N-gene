#include "Complex.hpp"
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
	Complex const &a = *this;

	return (a.r == b.r) && (a.i == b.i);
}

bool Complex::operator!=( Complex const &b ) const
{
	Complex const &a = *this;

	return (a.r != b.r) && (a.i != b.i);
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

void Complex::TurnToward( Complex const &target, float maxRotationDegrees )
{
	Complex &current( *this );
	Complex diff = target/current;

	float rotationDiff	 = fabs( diff.GetRotation() );
	float rotationNeeded = (rotationDiff < maxRotationDegrees) ? rotationDiff : maxRotationDegrees; 

	if( diff.i > 0.f )
		current *= Complex( +rotationNeeded );
	else
		current *= Complex( -rotationNeeded );
}
