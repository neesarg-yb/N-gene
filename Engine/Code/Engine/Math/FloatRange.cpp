#pragma once

#include "FloatRange.hpp"
#include "Engine/Math/MathUtil.hpp"

FloatRange::FloatRange()
{

}

FloatRange::FloatRange( float min , float max )
{
	this->min = min;
	this->max = max;
}

FloatRange::FloatRange( float initialMinMax )
{
	this->min = initialMinMax;
	this->max = initialMinMax;
}

FloatRange::~FloatRange()
{

}

float FloatRange::GetRandomInRange() const
{
	return GetRandomFloatInRange( this->min , this->max );
}

void FloatRange::ExpandToInclude( float number )
{
	min = (number < min) ? number : min;
	max = (number > max) ? number : max;
}

void FloatRange::SetFromText( const char* text )
{
	std::string inputStr = text;
	int tildeAt = (int) inputStr.find('~');

	if( tildeAt == -1 )  // If no tilde found
	{
		::SetFromText( this->min , inputStr.c_str() );
		::SetFromText( this->max , inputStr.c_str() );
	}
	else  // the tilde is there
	{
		std::string first(inputStr , 0 , tildeAt);
		::SetFromText( this->min , first.c_str() );
		std::string second(inputStr , tildeAt+1);
		::SetFromText( this->max , second.c_str() );
	}
}

bool DoRangesOverlap( const FloatRange& a , const FloatRange& b )
{
	bool condition1 = b.min >= a.min && b.min <= a.max;
	bool condition2 = b.max >= a.min && b.max <= a.max;

	if( condition1 || condition2 )
		return true;

	return false;
}

const FloatRange Interpolate( const FloatRange& start, const FloatRange& end, float fractionTowardEnd )
{
	float resultMin = Interpolate( start.min , end.min , fractionTowardEnd );
	float resultMax = Interpolate( start.max , end.max , fractionTowardEnd );

	FloatRange resultRange = FloatRange( resultMin , resultMax );

	return resultRange;
}