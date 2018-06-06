#pragma once

#include <cstdlib>
#include <math.h>
#include <time.h>

# define M_PI           3.1415926535f  /* pi */

class MathUtil {
public:
	static float DegreeToRadian(float degree);
	static float RadianToDegree(float radian);
	static float CosDegree(float degree);
	static float SinDegree(float degree);
	static float atan2fDegree(float y, float x);
	
	static int GetRandomIntInRange(int minInclusive, int maxInclusive);
	static int GetRandomNonNegativeIntLessThan(int maxNotInclusive);
	static float GetRandomFloatInRange(float minInclusive, float maxInclusive);
	static float GetRandomFloatZeroToOne();
	static float GetRandomFloatAsPlusOrMinusOne();

	static float ClampFloat01(float number);											// Clamps the number in range  [ 0.0f , 1.0f ]
	static float ClampFloat(float inValue, float minInclusive, float maxInclusive);		// Clamps the inValue in range [ minInclusive, maxInclusive ]
	static float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd);	// Maps inValue to outRange

	static void NewSeedForRandom();
	
private:
	MathUtil() {
	
	}
};