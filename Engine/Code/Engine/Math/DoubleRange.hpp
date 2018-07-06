#pragma once

class DoubleRange
{
public:
	double min	= 0.0;
	double max	= 0.0;

public:
			 DoubleRange();
	explicit DoubleRange( double rMin , double rMax );
	explicit DoubleRange( double initialMinMax );
			~DoubleRange();

public:
	bool	IsValueInRange( double valueToCheck );
	void	ChangeRangeToInclude( double includeValue );
};