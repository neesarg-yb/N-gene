#pragma once
#include <string>
#include "Engine/Math/MathUtil.hpp"

class Vector4;

class Rgba {

public:
	unsigned char r;						// Red byte, 0-255
	unsigned char g;						// Green byte, 0-255
	unsigned char b;						// Blue byte, 0-255
	unsigned char a;						// Alpha (opacity) byte, 0-255; 0 is transparent, 255 is opaque
	
	Rgba::Rgba();							// Default-constructs to opaque white (255, 255, 255, 255)
	explicit Rgba::Rgba( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );

	bool operator == (const Rgba& compareWith ) const;

	void	SetAsBytes( unsigned char redByte, unsigned char greenByte, unsigned char blueByte, unsigned char alphaByte=255 );		//	Set in range of [ 000 , 255 ]
	void	SetAsFloats( float normalizedRed, float normalizedGreen, float normalizedBlue, float normalizedAlpha=1.0f );			//	Set in range of [ 0.f , 1.f ]
	void	GetAsFloats( float& normalizedRed, float& normalizedGreen, float& normalizedBlue, float& normalizedAlpha ) const;		//	Get in range of [ 0.f , 1.f ]
	Vector4	GetAsNormalizedRgba() const;
	void	ScaleRGB( float rgbScale );		// Scale (and clamps) RGB components, but not A
	void	ScaleAlpha( float alphaScale );	// Scale (and clamps) Alpha, RGB is untouched

	void	SetFromText( const char* text );
};


const Rgba Interpolate( const Rgba& start, const Rgba& end, float fractionTowardEnd );


const Rgba RGBA_BLACK_COLOR		(   0,   0,   0, 255 );
const Rgba RGBA_WHITE_COLOR		( 255, 255, 255, 255 );
const Rgba RGBA_GRAY_COLOR		( 180, 180, 180, 255 );
const Rgba RGBA_RED_COLOR		( 255,   0,   0, 255 );
const Rgba RGBA_GREEN_COLOR		(   0, 255,   0, 255 );
const Rgba RGBA_BLUE_COLOR		(   0,   0, 255, 255 );
const Rgba RGBA_MAGENTA_COLOR	( 255,   0, 255, 255 );
const Rgba RGBA_PURPLE_COLOR	( 128,   0, 128, 255 );
const Rgba RGBA_YELLOW_COLOR	( 255, 255,   0, 255 );
const Rgba RGBA_KHAKI_COLOR		( 240, 230, 140, 255 );
const Rgba RGBA_CYAN_COLOR		(   0, 255, 255, 255 );
const Rgba RGBA_ORANGE_COLOR	( 255,  69,   0, 255 );
