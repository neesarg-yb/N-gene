#pragma once
#include <string>
#include "Engine/Math/MathUtil.hpp"

class Rgba {

public:
	unsigned char r;						// Red byte, 0-255
	unsigned char g;						// Green byte, 0-255
	unsigned char b;						// Blue byte, 0-255
	unsigned char a;						// Alpha (opacity) byte, 0-255; 0 is transparent, 255 is opaque
	
	Rgba::Rgba();							// Default-constructs to opaque white (255, 255, 255, 255)
	explicit Rgba::Rgba( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );

	bool operator == (const Rgba& compareWith ) const;

	void SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );		//	Set in range of [ 000 , 255 ]
	void SetAsFloats( float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha=1.0f );			//	Set in range of [ 0.f , 1.f ]
	void GetAsFloats( float& normalizedRed, float& normalizedGreen, float& normalizedBlue, float& normalizedAlpha ) const;		//	Get in range of [ 0.f , 1.f ]
	void ScaleRGB( float rgbScale );		// Scale (and clamps) RGB components, but not A
	void ScaleAlpha( float alphaScale );	// Scale (and clamps) Alpha, RGB is untouched

	void SetFromText( const char* text );
};


const Rgba Interpolate( const Rgba& start, const Rgba& end, float fractionTowardEnd );