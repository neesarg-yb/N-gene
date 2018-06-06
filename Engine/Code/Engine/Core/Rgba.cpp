#include "Engine/Core/Rgba.hpp"

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

void Rgba::SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte/* =255 */ ) {
	r = redByte;
	g = greenByte;
	b = blueByte;
	a = alphaByte;
}

void Rgba::SetAsFloats( float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha/* =1.0f */ ) {
	r = (unsigned char) MathUtil::RangeMapFloat(normalizedRed,   0.f, 1.f, 0.f, 255.f);
	g = (unsigned char) MathUtil::RangeMapFloat(normalizedGreen, 0.f, 1.f, 0.f, 255.f);
	b = (unsigned char) MathUtil::RangeMapFloat(normalizedBlue,  0.f, 1.f, 0.f, 255.f);
	a = (unsigned char) MathUtil::RangeMapFloat(normalizedAlpha, 0.f, 1.f, 0.f, 255.f);
}

void Rgba::GetAsFloats( float& normalizedRed, float& normalizedGreen, float& normalizedBlue, float& normalizedAlpha ) const {
	normalizedRed   = MathUtil::RangeMapFloat(r, 0.f, 255.f, 0.f, 1.f);
	normalizedGreen = MathUtil::RangeMapFloat(g, 0.f, 255.f, 0.f, 1.f);
	normalizedBlue  = MathUtil::RangeMapFloat(b, 0.f, 255.f, 0.f, 1.f);
	normalizedAlpha = MathUtil::RangeMapFloat(a, 0.f, 255.f, 0.f, 1.f);
}

void Rgba::ScaleRGB( float rgbScale ) {
	// Scale
	float scaledR = r * rgbScale;
	float scaledG = g * rgbScale;
	float scaledB = b * rgbScale;

	// Clamp
	r = (unsigned char) MathUtil::ClampFloat(scaledR, 0.f, 255.f);
	g = (unsigned char) MathUtil::ClampFloat(scaledG, 0.f, 255.f);
	b = (unsigned char) MathUtil::ClampFloat(scaledB, 0.f, 255.f);
}

void Rgba::ScaleAlpha( float alphaScale ) {
	// Scale
	float scaledAlpha = a * alphaScale;

	// Clamp
	a = (unsigned char) MathUtil::ClampFloat(scaledAlpha, 0.f, 255.f);
}