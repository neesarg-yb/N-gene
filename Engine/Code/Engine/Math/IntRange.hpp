#pragma once
#include <string>
#include "Engine/Math/MathUtil.hpp"

class IntRange
{
public:
	int min;
	int max;

	explicit IntRange( int min , int max );
	explicit IntRange( int initialMinMax );
	~IntRange();

	int GetRandomInRange() const;

	void SetFromText( const char* text );

private:

};

bool DoRangesOverlap( const IntRange& a , const IntRange& b );

const IntRange Interpolate( const IntRange& start, const IntRange& end, float fractionTowardEnd );