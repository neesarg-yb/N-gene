#pragma once
#include <cmath>
#include "MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"

using namespace std;

bool AreEqualFloats( float a, float b, uint ulp )
{
	float absDiff	= std::fabsf( a - b );
	float absSum	= std::fabsf( a + b );

	// the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return (	absDiff <= (std::numeric_limits<float>::epsilon() * absSum * ulp)
			 // unless the result is subnormal
			 || absDiff <   std::numeric_limits<float>::min()	);
}

bool SolveQuadraticEquation( Vector2& out, float a, float b, float c )
{
	//	Quadratic Equation:
	//
	//	root = ( -b +- (d)^(0.5) ) / ( 2a )
	//	d	 = ( b^2 ) - ( 4ac )

	float bSquare	= b * b;
	float ac		= a * c;
	float d			= bSquare - (4.f * ac);

	// Solution is imaginary or infinite
	if( d < 0 || a == 0 )
		return false;

	float squareRootD	= sqrt( d );
	float root1			= ( -b + squareRootD ) / ( 2.f * a );
	float root2			= ( -b - squareRootD ) / ( 2.f * a );

	// Smaller root first & larger second
	if( root1 <= root2 )
	{
		out.x = root1; 
		out.y = root2;
	}
	else
	{
		out.x = root2; 
		out.y = root1;
	}

	return true;
}

float AreaOfTriangle2D( Vector2 p1, Vector2 p2, Vector2 p3 )
{
	return abs( ((p1.x * (p2.y - p3.y)) + (p2.x * (p3.y - p1.y)) + (p3.x * (p1.y - p2.y))) * 0.5f );
}

bool IsPointIsInsideTriangle2D( Vector2 point, Vector2 triangleCornerA, Vector2 triangleCornerB, Vector2 triangleCornerC )
{
	// Area of the triangle
	float A  = AreaOfTriangle2D( triangleCornerA, triangleCornerB, triangleCornerC );
	
	// Area of three sub-triangles made with the point & two corners
	float A1 = AreaOfTriangle2D( point, triangleCornerA, triangleCornerB );
	float A2 = AreaOfTriangle2D( point, triangleCornerB, triangleCornerC );
	float A3 = AreaOfTriangle2D( point, triangleCornerA, triangleCornerC );

	// If sum of sub-area(s) == total area i.e. the point is inside
	return ( A == (A1 + A2 + A3) );
}

void GetPointsOnCircle2D( Vector2 center, float radius, int numPoints, Vector2 *outArray )
{
	float stepInDegrees = 360.f / numPoints;

	for( int i = 0; i < numPoints; i++ )
	{
		outArray[i].x = radius * CosDegree( stepInDegrees * i );
		outArray[i].y = radius * SinDegree( stepInDegrees * i );
	}
}

float DegreeToRadian(float degree) {
	return ( (degree * M_PI) / 180.f );
}

float RadianToDegree(float radian) {
	return ( (radian * 180) / M_PI );
}

float CosDegree(float degree) {
	return ( cosf(DegreeToRadian(degree)) );
}

float SinDegree(float degree) {
	return ( sinf(DegreeToRadian(degree)) );
}

float atan2fDegree(float y, float x) {
	return RadianToDegree(atan2f(y, x));
}

float GetRandomFloatZeroToOne() {
	return ( (float) rand() / (float) RAND_MAX );
}

float GetRandomFloatInRange(float minInclusive, float maxInclusive) {
	float range = (maxInclusive - minInclusive);

	return minInclusive +  (GetRandomFloatZeroToOne() * range) ;
}

float GetRandomFloatAsPlusOrMinusOne() {
	return (rand() % 2 ? 1.f : -1.f);
}

int GetRandomNonNegativeIntLessThan(int maxNotInclusive) {
	return ( (int) rand() % maxNotInclusive );
}

int GetRandomIntInRange(int minInclusive, int maxInclusive) {
	int range = (maxInclusive - minInclusive) + 1;
	return ( minInclusive + GetRandomNonNegativeIntLessThan(range) );
}

bool CheckRandomChance( float chanceForSuccess ) {
	chanceForSuccess *= 100;
	int testInt = GetRandomIntInRange( 1, 100 );

	if( testInt <=  chanceForSuccess) {
		return true;
	}

	return false;
}

int ClampInt( int inValue, int min, int max ) {
	if(inValue < min) {
		inValue = min;
	}
	else if(inValue > max) {
		inValue = max;
	}

	return inValue;
}

float ClampFloat01(float number) {
	number = ClampFloat(number, 0.f, 1.f);

	return number;
}

float ClampFloat(float inValue, float minInclusive, float maxInclusive) {
	if(inValue < minInclusive) {
		inValue = minInclusive;
	} 
	else if(inValue > maxInclusive) {
		inValue = maxInclusive;
	}

	return inValue;
}

float ClampFloatNegativeOneToOne( float inValue ) {
	if(inValue < -1.f) {
		inValue = -1.f;
	}
	else if(inValue > 1.f) {
		inValue = 1.f;
	}

	return inValue;
}

int RoundToNearestInt( float inValue ) {
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

float GetSign( float number )
{
	if( number >= 0.f )
		return +1.f;
	else
		return -1.f;
}

void NewSeedForRandom() {
	srand( (unsigned int) time(NULL) );
}

float RangeMapFloat(float inValue, float inStart, float inEnd, float outStart, float outEnd) {
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

float GetFractionInRange( float inValue, float rangeStart, float rangeEnd ) {
	float relativePosition = inValue - rangeStart;
	float range = rangeEnd - rangeStart;							// range can't be ZERO

	// Short-circuit if unsensible range
	if(range == 0) return 0.f;

	float fraction = relativePosition / range;

	return fraction;
}

float Interpolate( float start, float end, float fractionTowardEnd ) {
	float range = end - start;										// range can't be ZERO
	
	// Short-circuit if unsensible range
	if(range == 0) return start;

	float relativePosition = fractionTowardEnd * range;
	float numberAtGivenFraction = relativePosition + start;

	return numberAtGivenFraction;
}

float GetAngularDisplacement( float startDegrees, float endDegrees ) {
	float angularDisp = endDegrees - startDegrees;

	while ( angularDisp > 180.f ) {
		angularDisp -= 360;
	}

	while ( angularDisp < -180.f ) {
		angularDisp += 360;
	}

	return angularDisp;
}

float TurnToward( float currentDegrees, float goalDegrees, float maxTurnDegrees ) {
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

bool AreBitsSet( unsigned char bitFlags8, unsigned char flagsToCheck ) {
	if( (bitFlags8 & flagsToCheck) == flagsToCheck ) {
		return true;
	}

	return false;
}

bool AreBitsSet( unsigned int bitFlags32, unsigned int flagsToCheck ) {
	if( (bitFlags32 & flagsToCheck) == flagsToCheck ) {
		return true;
	}

	return false;
}

void SetBits( unsigned char& bitFlags8, unsigned char flagsToSet ) {
	bitFlags8 = bitFlags8 | flagsToSet;
}

void SetBits( unsigned int& bitFlags32, unsigned int flagsToSet ) {
	bitFlags32 = bitFlags32 | flagsToSet;
}

void ClearBits( unsigned char& bitFlags8, unsigned char flagToClear ) {
	unsigned char invertFlag = ~flagToClear;

	bitFlags8 = bitFlags8 & invertFlag;
}

void ClearBits( unsigned int& bitFlags32, unsigned int flagToClear ) {
	unsigned int invertFlag = ~flagToClear;

	bitFlags32 = bitFlags32 & invertFlag;
}

float SmoothStart2( float t )
{
	t = ClampFloat01(t);

	return t * t;
}

float SmoothStart3( float t )
{
	t = ClampFloat01(t);

	return t * t * t;
}

float SmoothStart4( float t )
{
	t = ClampFloat01(t);

	return t * t * t * t;
}

float SmoothStop2 ( float t )
{
	t = ClampFloat01(t);

	float flip = 1.f - t;
	float square = flip * flip;
	float result = 1.f - square;	// flip again

	return result;
}

float SmoothStop3 ( float t )
{
	t = ClampFloat01(t);

	float flip = 1.f - t;
	float cube = flip * flip * flip;
	float result = 1.f - cube;	// flip again

	return result;
}

float SmoothStop4 ( float t )
{
	t = ClampFloat01(t);

	float flip = 1.f - t;
	float pow4 = flip * flip * flip * flip;
	float result = 1.f - pow4;	// flip again

	return result;
}

float SmoothStep3 ( float t )
{
	t = ClampFloat01(t);

	float smoothStart = SmoothStart3(t);
	float smoothStop  = SmoothStop3(t);

	float smoothStep = ( ( 1.f - t ) * smoothStart ) + ( t * smoothStop );		// using t as a weight

	return smoothStep;
}

int Interpolate( int start, int end, float fractionTowardEnd )
{
	float range = (float)end - (float)start;										// range can't be ZERO

																	// Short-circuit if unsensible range
	if(range == 0) return start;

	float relativePosition = fractionTowardEnd * range;
	float floatAnswer = ( relativePosition ) + start;
	float roundIt = floatAnswer < 0 ? -0.5f : 0.5f;
	int numberAtGivenFraction = (int) (floatAnswer + roundIt);		
	// Why (relativePosition + 0.5f) ?
	//		Computer consider (int) 6.51 = 6, but we need (int) 6.51 = 7.

	return numberAtGivenFraction;
}

unsigned char Interpolate( unsigned char start, unsigned char end, float fractionTowardEnd )
{
	unsigned char resuleNumber = (unsigned char) Interpolate( (int)start , (int)end , fractionTowardEnd );
	resuleNumber = (unsigned char) ClampInt( (int)resuleNumber , 0 , 255 );

	return resuleNumber;
}

void SetFromText( int& setIt , const char* text )
{
	setIt = atoi(text);
}

void SetFromText( float& setIt , const char* text )
{
	setIt = (float) atof(text);
}

void SetFromText( bool& setIt , const char* text )
{
	setIt = false;			// In case of malfunction, default value will be FALSE
	
	if( strcmp( text , "true" ) == 0 || strcmp( text , "1" ) == 0 )
		setIt = true;
}

void SetFromText( std::vector<std::string>& setIt, const char* delimiter, const char* text )
{
	std::string inputText = text;

	size_t startPos = 0;
	for( size_t nextDelPos = inputText.find( delimiter, startPos); nextDelPos != std::string::npos; nextDelPos = inputText.find( delimiter, nextDelPos+1 ) )
	{
		// delimiter found, push_back the string
		std::string strToPush( inputText, startPos, nextDelPos-startPos );
		setIt.push_back( strToPush );

		startPos = nextDelPos+1;
	}

	// push_back last part
	std::string strToPush( inputText, startPos );
	setIt.push_back( strToPush );
	
}

int GetIndexFromColumnRowNumberForMatrixOfWidth( int columnNum , int rowNum , int width )
{
	int index = ( width * rowNum ) + columnNum;

	return index;
}

std::vector<std::string> SplitIntoStringsByDelimiter( std::string passedString, char delimeter )
{
	std::string word = "";
	passedString += delimeter;

	int stringTotalLength = (int) passedString.length();

	// Traverse the string from left-to-right
	std::vector<std::string> subStringList;
	for( int i=0; i<stringTotalLength; i++ )
	{
		if( passedString[i] != delimeter )
			word += passedString[i];
		else
		{
			if( (int) word.size() != 0 )
				subStringList.push_back( word );

			word = "";
		}
	}

	return subStringList;
}

void ReplaceAllInString( std::string &stringToModify, std::string const &replaceFrom, std::string const &replaceTo )
{
	if( replaceFrom.empty() )
		return;

	size_t startPos = 0;
	while( (startPos = stringToModify.find( replaceFrom, startPos )) != std::string::npos )
	{
		stringToModify.replace( startPos, replaceFrom.length(), replaceTo );
		startPos += replaceTo.length();
	}
}

int ModuloNonNegative( int operatingOn, int moduloBy )
{
	// ( b + (a % b) ) % b
	int nnMod = ( moduloBy + ( operatingOn % moduloBy ) ) % moduloBy;

	return nnMod;
}

bool CycleLess( uint16_t a, uint16_t b )
{
	// Converting (Negative Number) -> (Unsigned Number)
	// --------------------------------------------------
	// [ Way 1 ]: Taking 2's compliment
	//
	// Negative is calculated as 2's compliment for unsigned numbers
	//		=>  6 = 110'b
	//		=> -6 = 010'b
	//		  ---------- (addition results into ZERO)
	//			0 = 000'b
	//
	// 2's compliment is NOT( number-as-positive ), and then add 0001'b to it
	//		=> (-6) = NOT(110'b) + 001'b
	//				= 001'b + 001'b
	//				= 010'b = 2
	//
	// [ Way 2 ]: MAX_NUM + 1 + (negative number)
	//
	//		=> -6 = (111'b) + (001'b) + (negative number)
	//			  = 7 + 1 - 6
	//			  = 8 - 6
	//			  = 2 = (010)
	uint16_t diff = b - a;

	return (diff != 0) && (diff < 0x8000);
}
