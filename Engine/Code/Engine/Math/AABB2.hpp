#pragma once

#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"

class AABB2
{
public:
	Vector2 mins;
	Vector2 maxs;
	
	// Constructors & Deconstruction
			~AABB2();
			 AABB2();
	explicit AABB2( float minX, float minY, float maxX, float maxY );
	explicit AABB2( const Vector2& mins, const Vector2& maxs );
	explicit AABB2( const Vector2& center, float radiusX, float radiusY );

	// Mutators
	void StretchToIncludePoint( float x, float y );
	void StretchToIncludePoint( const Vector2& point );
	void AddPaddingToSides( float xPaddingRadius, float yPaddingRadius );
	void Translate( const Vector2& translation );
	void Translate( float translationX, float translationY );

	// Accessors / queries
	bool	IsPointInside( float x, float y ) const;
	bool	IsPointInside( const Vector2& point ) const;
	Vector2 GetDimensions() const;
	Vector2 GetCenter() const;

	// Operator
	void  operator +=( const Vector2& translation );
	void  operator -=( const Vector2& antiTranslation );
	AABB2 operator + ( const Vector2& translation ) const;
	AABB2 operator - ( const Vector2& antiTranslation ) const;

	// Static functions
	static bool DoAABBsOverlap( const AABB2& a, const AABB2& b );

private:

};