#pragma once
#include <string>
#include "Engine/Math/MathUtil.hpp"

class FloatRange
{
public:
			 FloatRange();									// Initializes an invalid range where [min, max] = [positive max, negative min]
	explicit FloatRange( float min , float max );
	explicit FloatRange( float initialMinMax );
			~FloatRange();

public:
	float min =  FLT_MAX; 
	float max = -FLT_MAX;

public:
	float	GetRandomInRange() const;
	void	ExpandToInclude( float number );
	void	SetFromText	( const char* text );

public:
	bool	Includes( float number ) const;
};

		bool		DoRangesOverlap	( const FloatRange& a , const FloatRange& b );
const	FloatRange	Interpolate		( const FloatRange& start, const FloatRange& end, float fractionTowardEnd );
