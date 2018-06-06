#include "MathUtil.hpp"

using namespace std;

float MathUtil::DegreeToRadian(float degree) {
	return ( (degree * M_PI) / 180.f );
}

float MathUtil::RadianToDegree(float radian) {
	return ( (radian * 180) / M_PI );
}

float MathUtil::CosDegree(float degree) {
	return ( cosf(DegreeToRadian(degree)) );
}

float MathUtil::SinDegree(float degree) {
	return ( sinf(DegreeToRadian(degree)) );
}

float MathUtil::atan2fDegree(float y, float x) {
	return RadianToDegree(atan2f(y, x));
}

float MathUtil::GetRandomFloatZeroToOne() {
	return ( (float) rand() / (float) RAND_MAX );
}

float MathUtil::GetRandomFloatInRange(float minInclusive, float maxInclusive) {
	float range = (maxInclusive - minInclusive);

	return minInclusive +  (MathUtil::GetRandomFloatZeroToOne() * range) ;
}

float MathUtil::GetRandomFloatAsPlusOrMinusOne() {
	return (rand() % 2 ? 1.f : -1.f);
}

int MathUtil::GetRandomNonNegativeIntLessThan(int maxNotInclusive) {
	return ( (int) rand() % maxNotInclusive );
}

int MathUtil::GetRandomIntInRange(int minInclusive, int maxInclusive) {
	int range = (maxInclusive - minInclusive) + 1;
	return ( minInclusive + GetRandomNonNegativeIntLessThan(range) );
}

float MathUtil::ClampFloat01(float number) {
	number = ClampFloat(number, 0.f, 1.f);

	return number;
}

float MathUtil::ClampFloat(float inValue, float minInclusive, float maxInclusive) {
	if(inValue < minInclusive) {
		inValue = minInclusive;
	} 
	else if(inValue > maxInclusive) {
		inValue = maxInclusive;
	}

	return inValue;
}

void MathUtil::NewSeedForRandom() {
	srand( (unsigned int) time(NULL) );
}

float MathUtil::RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd) {
	// If inRange is zero, call of this function is not-appropriate, handle this situation..
	if(inStart == inEnd) {
		return (outStart + outEnd) * 0.5f;
	}

	// Function call is appropriate, start calculation
	float inRange = inEnd - inStart;
	float outRange = outEnd - outStart;
	float inRelativeToStart = inValue - inStart;
	float fractionOfRange = inRelativeToStart / inRange;			// inRange can't be ZERO
	float outRelativeToStart = fractionOfRange * outRange;

	return outRelativeToStart + outStart;
}