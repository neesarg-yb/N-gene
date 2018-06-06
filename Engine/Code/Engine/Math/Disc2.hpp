#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtil.hpp"

class Disc2
{
public:
	Vector2 center;
	float	radius;
	
			~Disc2();
			 Disc2();
			 Disc2( const Disc2& copyFrom );
	explicit Disc2( float initialX, float initialY, float initialRadius );
	explicit Disc2( const Vector2& initialCenter, float initialRadius );


	void  operator += (const Vector2& translation);				// move
	void  operator -= (const Vector2& antiTranslation);
	Disc2 operator +  (const Vector2& translation) const;		// create a moved copy
	Disc2 operator -  (const Vector2& antiTranslation) const;

	void StretchToIncludePoint(float x, float y);				// expand radius if (x,y) is outside
	void StretchToIncludePoint(const Vector2& point);			// expand radius if point is outside
	void AddPaddingToRadius(float paddingRadius);
	void Translate(float translationX, float translationY);		// move the center 
	void Translate(const Vector2& translation);					// move the center
	bool IsPointInside(float x, float y) const;					// is (x,y) within disk interior
	bool IsPointInside(const Vector2& point) const;				// is "point" within disk interior

	bool static DoDiscsOverlap( const Vector2& aCenter, float aRadius,
								const Vector2& bCenter, float bRadius);
	bool static DoDiscsOverlap( const Disc2& a, const Disc2& b);
	bool static DoDiscAndPointOverlap( const Disc2& disc, const Vector2& point);

private:

};

const Disc2 Interpolate( const Disc2& start, const Disc2& end, float fractionTowardEnd );