#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector4.hpp"

Rgba::Rgba() {
	r = 255;
	g = 255;
	b = 255;
	a = 255;
}

Rgba::Rgba( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte/* =255 */ ) {
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

bool Rgba::operator == ( const Rgba& compareWith ) const
{
	bool areSame = true;

	if( this->r != compareWith.r )		areSame = false;
	if( this->g != compareWith.g )		areSame = false;
	if( this->b != compareWith.b )		areSame = false;
	if( this->a != compareWith.a )		areSame = false;

	return areSame;
}

void Rgba::SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte/* =255 */ ) {
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsFloats( float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha/* =1.0f */ ) {
	r = (unsigned char) RangeMapFloat(normalizedRed,   0.f, 1.f, 0.f, 255.f);
	g = (unsigned char) RangeMapFloat(normalizedGreen, 0.f, 1.f, 0.f, 255.f);
	b = (unsigned char) RangeMapFloat(normalizedBlue,  0.f, 1.f, 0.f, 255.f);
	a = (unsigned char) RangeMapFloat(normalizedAlpha, 0.f, 1.f, 0.f, 255.f);
}

void Rgba::GetAsFloats( float& normalizedRed, float& normalizedGreen, float& normalizedBlue, float& normalizedAlpha ) const {
	normalizedRed   = RangeMapFloat(r, 0.f, 255.f, 0.f, 1.f);
	normalizedGreen = RangeMapFloat(g, 0.f, 255.f, 0.f, 1.f);
	normalizedBlue  = RangeMapFloat(b, 0.f, 255.f, 0.f, 1.f);
	normalizedAlpha = RangeMapFloat(a, 0.f, 255.f, 0.f, 1.f);
}

Vector4 Rgba::GetAsNormalizedRgba() const
{
	Vector4 normalizedRgba;

	normalizedRgba.x = RangeMapFloat(r, 0.f, 255.f, 0.f, 1.f);
	normalizedRgba.y = RangeMapFloat(g, 0.f, 255.f, 0.f, 1.f);
	normalizedRgba.z = RangeMapFloat(b, 0.f, 255.f, 0.f, 1.f);
	normalizedRgba.w = RangeMapFloat(a, 0.f, 255.f, 0.f, 1.f);

	return normalizedRgba;
}

void Rgba::ScaleRGB( float rgbScale ) {
	// Scale
	float scaledR = r * rgbScale;
	float scaledG = g * rgbScale;
	float scaledB = b * rgbScale;

	// Clamp
	r = (unsigned char) ClampFloat(scaledR, 0.f, 255.f);
	g = (unsigned char) ClampFloat(scaledG, 0.f, 255.f);
	b = (unsigned char) ClampFloat(scaledB, 0.f, 255.f);
}

void Rgba::ScaleAlpha( float alphaScale ) {
	// Scale
	float scaledAlpha = a * alphaScale;

	// Clamp
	a = (unsigned char) ClampFloat(scaledAlpha, 0.f, 255.f);
}

void Rgba::SetFromText( const char* text )
{
	std::string inputStr = text;
	float fInput;

	int nextStartIndex = 0;
	int commaAt = (int) inputStr.find(',');
	int length = commaAt - nextStartIndex;
	std::string first(inputStr , nextStartIndex , length);
	::SetFromText( fInput , first.c_str() );
	this->r = (unsigned char) fInput;

	nextStartIndex = commaAt + 1;
	commaAt = (int) inputStr.find(',' , nextStartIndex);
	length = commaAt - nextStartIndex;
	std::string second(inputStr , nextStartIndex , length);
	::SetFromText( fInput , second.c_str() );
	this->g = (unsigned char) fInput;


	nextStartIndex = commaAt + 1;
	commaAt = (int) inputStr.find(',' , nextStartIndex);
	length = commaAt - nextStartIndex;
	std::string third(inputStr , nextStartIndex , length);
	::SetFromText( fInput , third.c_str() );
	this->b = (unsigned char) fInput;


	if( commaAt == -1 )		// if alpha value is not passed, set it to 255
	{
		this->a = 255;
	}
	else					// if alpha is passed, use that value
	{
		nextStartIndex = commaAt + 1;
		commaAt = (int) inputStr.find(',' , nextStartIndex);
		length = commaAt - 1 - nextStartIndex;
		std::string fourth(inputStr , nextStartIndex , length);
		::SetFromText( fInput , fourth.c_str() );
		this->a = (unsigned char) fInput;
	}
}


const Rgba Interpolate( const Rgba& start, const Rgba& end, float fractionTowardEnd )
{
	unsigned char r = Interpolate( start.r , end.r , fractionTowardEnd );
	unsigned char g = Interpolate( start.g , end.g , fractionTowardEnd );
	unsigned char b = Interpolate( start.b , end.b , fractionTowardEnd );
	unsigned char a = Interpolate( start.a , end.a , fractionTowardEnd );
	Rgba resultColor = Rgba( r , g , b , a );

	return resultColor;
}