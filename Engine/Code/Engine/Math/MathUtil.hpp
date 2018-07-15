#pragma once

#include <cstdlib>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <string>

class Vector2;

bool	SolveQuadraticEquation( Vector2& out, float a, float b, float c );			// Solves a*(t^2) + b*t + c = 0. If roots found, returns true. Note: Always returns two roots. 

float	DegreeToRadian(float degree);
float	RadianToDegree(float radian);
float	CosDegree(float degree);
float	SinDegree(float degree);
float	atan2fDegree(float y, float x);

int		GetRandomIntInRange(int minInclusive, int maxInclusive);
int		GetRandomNonNegativeIntLessThan(int maxNotInclusive);
float	GetRandomFloatInRange(float minInclusive, float maxInclusive);
float	GetRandomFloatZeroToOne();
float	GetRandomFloatAsPlusOrMinusOne();
bool	CheckRandomChance( float chanceForSuccess );								// If 0.27 passed, returns true 27% of the time

int		ClampInt( int inValue, int min, int max );
float	ClampFloat01(float number);													// Clamps the number in range  [ 0.0f , 1.0f ]
float	ClampFloat(float inValue, float minInclusive, float maxInclusive);			// Clamps the inValue in range [ minInclusive, maxInclusive ]
float	ClampFloatNegativeOneToOne( float inValue );
int		RoundToNearestInt( float inValue );											// 0.5 rounds up to 1; -0.5 rounds up to 0

// For a value in [inStart,inEnd], finds the corresponding value in [outStart,outEnd].
float	RangeMapFloat( float inValue, float inStart, float inEnd, float outStart, float outEnd );

// Finds the % (as a fraction) of inValue in [rangeStart,rangeEnd].
// For example, 3 is 25% (0.25) of the way through the range [2,6].
float	GetFractionInRange( float inValue, float rangeStart, float rangeEnd );

// Finds the value at a certain % (fraction) in [rangeStart,rangeEnd].
// For example, at 75% (0.75) of the way through [2,6] is 5.
float	Interpolate( float start, float end, float fractionTowardEnd );

// Finds the “angular displacement” (or signed angular distance) from startDegrees to endDegrees.
float	GetAngularDisplacement( float startDegrees, float endDegrees );

// I face currentDegrees and want to turn “toward” goalDegrees, by up to maxTurnDegrees.
float	TurnToward	( float currentDegrees, float goalDegrees, float maxTurnDegrees );

bool	AreBitsSet	( unsigned char bitFlags8, unsigned char flagsToCheck );
bool	AreBitsSet	( unsigned int bitFlags32, unsigned int flagsToCheck );
void	SetBits		( unsigned char& bitFlags8, unsigned char flagsToSet );
void	SetBits		( unsigned int& bitFlags32, unsigned int flagsToSet );
void	ClearBits	( unsigned char& bitFlags8, unsigned char flagToClear );
void	ClearBits	( unsigned int& bitFlags32, unsigned int flagToClear );

void	NewSeedForRandom();

float	SmoothStart2( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease in”)
float	SmoothStart3( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease in”)
float	SmoothStart4( float t ); // 4th-degree smooth start (a.k.a. “quartic ease in”)
float	SmoothStop2 ( float t ); // 2nd-degree smooth start (a.k.a. “quadratic ease out”)
float	SmoothStop3 ( float t ); // 3rd-degree smooth start (a.k.a. “cubic ease out”)
float	SmoothStop4 ( float t ); // 4th-degree smooth start (a.k.a. “quartic ease out”)
float	SmoothStep3 ( float t ); // 3rd-degree smooth start/stop (a.k.a. “smoothstep”)

int				Interpolate( int start, int end, float fractionTowardEnd );
unsigned char	Interpolate( unsigned char start, unsigned char end, float fractionTowardEnd );

void	SetFromText( int& setIt , const char* text );
void	SetFromText( float& setIt , const char* text );
void	SetFromText( bool& setIt , const char* text );
void	SetFromText( std::vector<std::string>& setIt, const char* delimiter, const char* text );

int		GetIndexFromColumnRowNumberForMatrixOfWidth( int columnNum , int rowNum , int width );		// It assumes that first cell is at ( zero , zero ) & width starts from 1

std::vector<std::string>	SplitIntoStringsByDelimiter( std::string passedString, char delimeter );
void						ReplaceAllInString( std::string &stringToModify, std::string const &replaceFrom, std::string const &replaceTo );

int	ModuloNonNegative( int operatingOn, int moduloBy );			// It is like normal modulo, but on negative values it wraps around the range => (-1) will give you (moduloBy - 1)