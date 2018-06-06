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

bool MathUtil::CheckRandomChance( float chanceForSuccess ) {
	chanceForSuccess *= 100;
	int testInt = GetRandomIntInRange( 1, 100 );

	if( testInt <=  chanceForSuccess) {
		return true;
	}

	return false;
}

int MathUtil::ClampInt( int inValue, int min, int max ) {
	if(inValue < min) {
		inValue = min;
	}
	else if(inValue > max) {
		inValue = max;
	}

	return inValue;
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

float MathUtil::ClampFloatNegativeOneToOne( float inValue ) {
	if(inValue < -1.f) {
		inValue = -1.f;
	}
	else if(inValue > 1.f) {
		inValue = 1.f;
	}

	return inValue;
}

int MathUtil::RoundToNearestInt( float inValue ) {
	int sign = inValue >= 0 ? +1 : -1;
	float absInValue = abs(inValue);
	float fraction = absInValue - (int) absInValue;

	// Positive Number, do normal round
	if( sign == +1 ) {
		return (int)(inValue+0.5);
	}
	
	// Negative Number, so special round
	if(fraction > 0.5) {
		return (int)inValue - 1;			// -1.6 -> -2
	}
	else /* if(fraction <= 0.5) */ 
	{
		return (int)inValue;				// -1.5 -> -1
	}

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

float MathUtil::GetFractionInRange( float inValue, float rangeStart, float rangeEnd ) {
	float relativePosition = inValue - rangeStart;
	float range = rangeEnd - rangeStart;							// range can't be ZERO

	// Short-circuit if unsensible range
	if(range == 0) return 0.f;

	float fraction = relativePosition / range;

	return fraction;
}

float MathUtil::Interpolate( float start, float end, float fractionTowardEnd ) {
	float range = end - start;										// range can't be ZERO
	
	// Short-circuit if unsensible range
	if(range == 0) return start;

	float relativePosition = fractionTowardEnd * range;
	float numberAtGivenFraction = relativePosition + start;

	return numberAtGivenFraction;
}

float MathUtil::GetAngularDisplacement( float startDegrees, float endDegrees ) {
	float angularDisp = endDegrees - startDegrees;

	while ( angularDisp > 180.f ) {
		angularDisp -= 360;
	}

	while ( angularDisp < -180.f ) {
		angularDisp += 360;
	}

	return angularDisp;
}

float MathUtil::TurnToward( float currentDegrees, float goalDegrees, float maxTurnDegrees ) {
	float difference = goalDegrees - currentDegrees;

	while (difference > 180.f) {
		difference -= 360;
	}
	while (difference < -180.f) {
		difference += 360;
	}

	if( difference != 0 ) {
		if( abs(difference) >= abs(maxTurnDegrees) ) {
			float diffSign = (difference < 0.f) ? -1.f : 1.f;						// Since we compared abs(values), we need sign to determine:
			return currentDegrees + (maxTurnDegrees*diffSign);						// whether add/subtract the maxTurnDegrees
		} 
		else /* abs(diffrence) < maxTurnDegrees */ {
			return currentDegrees + difference;
		}
	}
	else /* difference == 0 */ {
		return currentDegrees;
	}
}

bool MathUtil::AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck ) {
	if( (bitFlags8 & flagsToCheck) == flagsToCheck ) {
		return true;
	}

	return false;
}

bool MathUtil::AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck ) {
	if( (bitFlags32 & flagsToCheck) == flagsToCheck ) {
		return true;
	}

	return false;
}

void MathUtil::SetBits( unsigned char& bitFlags8, unsigned char flagsToSet ) {
	bitFlags8 = bitFlags8 | flagsToSet;
}

void MathUtil::SetBits( unsigned int& bitFlags32, unsigned int flagsToSet ) {
	bitFlags32 = bitFlags32 | flagsToSet;
}

void MathUtil::ClearBits( unsigned char& bitFlags8, unsigned char flagToClear ) {
	unsigned char invertFlag = ~flagToClear;

	bitFlags8 = bitFlags8 & invertFlag;
}

void MathUtil::ClearBits( unsigned int& bitFlags32, unsigned int flagToClear ) {
	unsigned int invertFlag = ~flagToClear;

	bitFlags32 = bitFlags32 & invertFlag;
}