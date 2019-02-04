#pragma once
#include <string>
#include "Engine/Math/MathUtil.hpp"

class IntRange
{
public:
			 IntRange();						// Initializes an invalid range where [min, max] = [positive max, negative min]
	explicit IntRange( int min , int max );
	explicit IntRange( int initialMinMax );
			~IntRange();

public:
	int min =  INT_MAX;
	int max = -INT_MAX;

public:
	int		GetRandomInRange() const;
	void	SetFromText( const char* text );
};

		bool		DoRangesOverlap	( const IntRange& a , const IntRange& b );
const	IntRange	Interpolate		( const IntRange& start, const IntRange& end, float fractionTowardEnd );
