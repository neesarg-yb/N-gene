#pragma once

#include "Engine/Math/MathUtil.hpp"
#include "Engine/Math/Vector2.hpp"

class AABB2
{
public:
	static AABB2 ONE_BY_ONE;
	static AABB2 NDC_SIZE;

public:
	Vector2 mins;
	Vector2 maxs;
	
	// Constructors & Deconstruction
			~AABB2();
			 AABB2();
			 AABB2( const AABB2& copy );
	explicit AABB2( float minX, float minY, float maxX, float maxY );
	explicit AABB2( const Vector2& mins, const Vector2& maxs );
	explicit AABB2( const Vector2& center, float radiusX, float radiusY );

	void SetFromText( const char* inputText );

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
	Vector2 GetClosestPointInsideAABB( Vector2 fromThePoint ) const;

	// Operator
	void  operator +=( const Vector2& translation );
	void  operator -=( const Vector2& antiTranslation );
	AABB2 operator + ( const Vector2& translation ) const;
	AABB2 operator - ( const Vector2& antiTranslation ) const;
	AABB2 operator * ( const float multiplyAllBy ) const;
	AABB2 operator / ( const float divideAllBy ) const;

	// Static functions
	static bool DoAABBsOverlap( const AABB2& a, const AABB2& b );
private:

};

const AABB2 Interpolate( const AABB2& start, const AABB2& end, float fractionTowardEnd );