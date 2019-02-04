#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Complex
{
public:
	 Complex( float rotation );						// Constructs a complex number with unit magnitude
	 Complex( float real, float imaginary );	
	~Complex();

public:
	float r = 1.f;									// real part
	float i = 0.f;									// imaginary part

public:
	Complex operator +  ( Complex const &b ) const;	// Positive slide
	Complex operator -  ( Complex const &b ) const;	// Negative slide
	Complex operator *  ( Complex const &b ) const;	// Adds the angles, multiplies the magnitudes
	Complex operator /  ( Complex const &b ) const;	// Subtracts the angles, divides the magnitude
	bool	operator == ( Complex const &b ) const;
	bool	operator != ( Complex const &b ) const;

	void	operator += ( Complex const &b );
	void	operator -= ( Complex const &b );
	void	operator *= ( Complex const &b );
	void	operator /= ( Complex const &b );

public:
			float GetRotation() const;				// Returns in degrees
			float GetMagnitude() const;
	inline	float GetMagnitudeSquared() const;

public:
	void TurnToward( Complex const &target, float maxRotationDegrees );	// maxRotationDegrees should be a positive float
};
