#pragma once

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
	Complex operator + ( Complex const &b ) const;	// Positive slide
	Complex operator - ( Complex const &b ) const;	// Negative slide
	Complex operator * ( Complex const &b ) const;	// Adds the angles, multiplies the magnitudes
	Complex operator / ( Complex const &b ) const;	// Subtracts the angles, divides the magnitude

	void	operator += ( Complex const &b );
	void	operator -= ( Complex const &b );
	void	operator *= ( Complex const &b );
	void	operator /= ( Complex const &b );

public:
			float GetRotation() const;
			float GetMagnitude() const;
	inline	float GetMagnitudeSquared() const;
};
