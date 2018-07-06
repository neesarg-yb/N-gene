#pragma once
#include "DoubleRange.hpp"

DoubleRange::DoubleRange()
{

}

DoubleRange::DoubleRange( double rMin , double rMax )
	: min( rMin )
	, max( rMax )
{

}

DoubleRange::DoubleRange( double initialMinMax )
	: min( initialMinMax )
	, max( initialMinMax )
{

}

DoubleRange::~DoubleRange()
{

}

bool DoubleRange::IsValueInRange( double valueToCheck )
{
	if( valueToCheck >= min && valueToCheck <= max )
		return true;
	else
		return false;
}

void DoubleRange::ChangeRangeToInclude( double includeValue )
{
	if( IsValueInRange( includeValue ) )
		return;

	min = (includeValue < min) ? includeValue : min;
	max = (includeValue > max) ? includeValue : max;
}
