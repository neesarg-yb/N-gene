#pragma once

#include <cstdlib>
#include <math.h>
#include <time.h>

const float M_PI = 3.1415926535f;  /* Pi */

class MathUtil {
public:
	static float DegreeToRadian(float degree);
	static float RadianToDegree(float radian);
	static float CosDegree(float degree);
	static float SinDegree(float degree);
	static float atan2fDegree(float y, float x);
	
	static int	 GetRandomIntInRange(int minInclusive, int maxInclusive);
	static int	 GetRandomNonNegativeIntLessThan(int maxNotInclusive);
	static float GetRandomFloatInRange(float minInclusive, float maxInclusive);
	static float GetRandomFloatZeroToOne();
	static float GetRandomFloatAsPlusOrMinusOne();
	static bool	 CheckRandomChance( float chanceForSuccess );								// If 0.27 passed, returns true 27% of the time

	static int	 ClampInt( int inValue, int min, int max );
	static float ClampFloat01(float number);												// Clamps the number in range  [ 0.0f , 1.0f ]
	static float ClampFloat(float inValue, float minInclusive, float maxInclusive);			// Clamps the inValue in range [ minInclusive, maxInclusive ]
	static float ClampFloatNegativeOneToOne( float inValue );
	static int	 RoundToNearestInt( float inValue );										// 0.5 rounds up to 1; -0.5 rounds up to 0
	
	// For a value in [inStart,inEnd], finds the corresponding value in [outStart,outEnd].
	static float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd);

	// Finds the % (as a fraction) of inValue in [rangeStart,rangeEnd].
	// For example, 3 is 25% (0.25) of the way through the range [2,6].
	static float GetFractionInRange( float inValue, float rangeStart, float rangeEnd );

	// Finds the value at a certain % (fraction) in [rangeStart,rangeEnd].
	// For example, at 75% (0.75) of the way through [2,6] is 5.
	static float Interpolate( float start, float end, float fractionTowardEnd );

	// Finds the “angular displacement” (or signed angular distance) from startDegrees to endDegrees.
	static float GetAngularDisplacement( float startDegrees, float endDegrees );

	// I face currentDegrees and want to turn “toward” goalDegrees, by up to maxTurnDegrees.
	static float TurnToward( float currentDegrees, float goalDegrees, float maxTurnDegrees );

	static bool  AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck );
	static bool  AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck );
	static void  SetBits( unsigned char& bitFlags8, unsigned char flagsToSet );
	static void  SetBits( unsigned int& bitFlags32, unsigned int flagsToSet );
	static void  ClearBits( unsigned char& bitFlags8, unsigned char flagToClear );
	static void  ClearBits( unsigned int& bitFlags32, unsigned int flagToClear );


	static void NewSeedForRandom();
	
private:
	MathUtil() {

	}
};