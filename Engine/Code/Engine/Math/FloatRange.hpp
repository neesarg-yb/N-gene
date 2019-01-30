#pragma once

#include <string>
#include "Engine/Math/MathUtil.hpp"

class FloatRange
{
public:
	float min = (std::numeric_limits<float>::max)();
	float max = (std::numeric_limits<float>::min)();

	FloatRange();									// Initializes an invalid range where [min, max] = [positive max, negative min]
	explicit FloatRange( float min , float max );
	explicit FloatRange( float initialMinMax );
	~FloatRange();

	float	GetRandomInRange() const;
	void	ExpandToInclude( float number );
	void	SetFromText( const char* text );

private:

};

bool DoRangesOverlap( const FloatRange& a , const FloatRange& b );

const FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd );