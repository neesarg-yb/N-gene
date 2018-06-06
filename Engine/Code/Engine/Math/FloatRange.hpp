#pragma once

#include <string>
#include "Engine/Math/MathUtil.hpp"

class FloatRange
{
public:
	float min;
	float max;

	explicit FloatRange( float min , float max );
	explicit FloatRange( float initialMinMax );
	~FloatRange();

	float GetRandomInRange() const;

	void SetFromText( const char* text );

private:

};

bool DoRangesOverlap( const FloatRange& a , const FloatRange& b );

const FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd );