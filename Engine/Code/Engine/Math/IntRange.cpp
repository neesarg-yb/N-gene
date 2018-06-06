#pragma once
#include "IntRange.hpp"
#include "Engine/Math/MathUtil.hpp"

IntRange::IntRange( int min , int max )
{
	this->min = min;
	this->max = max;
}

IntRange::IntRange( int initialMinMax )
{
	this->min = initialMinMax;
	this->max = initialMinMax;
}

IntRange::~IntRange()
{

}

int IntRange::GetRandomInRange() const
{
	return GetRandomIntInRange( min , max );
}

void IntRange::SetFromText( const char* text )
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

bool DoRangesOverlap( const IntRange& a , const IntRange& b )
{
	bool condition1 = b.min >= a.min && b.min <= a.max;
	bool condition2 = b.max >= a.min && b.max <= a.max;

	if( condition1 || condition2 )
		return true;

	return false;
}

const IntRange Interpolate( const IntRange& start, const IntRange& end, float fractionTowardEnd )
{
	int resultMin = Interpolate( start.min , end.min , fractionTowardEnd );
	int resultMax = Interpolate( start.max , end.max , fractionTowardEnd );

	IntRange resultRange = IntRange( resultMin , resultMax );

	return resultRange;
}