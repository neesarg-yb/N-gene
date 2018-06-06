#include "Engine/Math/AABB2.hpp"

AABB2::AABB2()
{

}

AABB2::~AABB2()
{

}

AABB2::AABB2(float minX, float minY, float maxX, float maxY) {
	this->mins.x = minX;
	this->mins.y = minY;

	this->maxs.x = maxX;
	this->maxs.y = maxY;
}

AABB2::AABB2( const Vector2& mins, const Vector2& maxs ) {
	this->mins = mins;
	this->maxs = maxs;
}

AABB2::AABB2( const Vector2& center, float radiusX, float radiusY  ) {
	this->mins.x = center.x - radiusX;
	this->mins.y = center.y - radiusY;

	this->maxs.x = center.x + radiusX;
	this->maxs.y = center.y + radiusY;
}

void AABB2::StretchToIncludePoint( float x, float y ) {
	if( x < mins.x ) {
		mins.x = x;
	} else if( x > maxs.x ) {
		maxs.x = x;
	}

	if( y < mins.y ) {
		mins.y = y;
	} else if( y > maxs.y ) {
		maxs.y = y;
	}
}

void AABB2::StretchToIncludePoint( const Vector2& point ) {
	StretchToIncludePoint( point.x, point.y );
}

void AABB2::AddPaddingToSides( float xPaddingRadius, float yPaddingRadius ) {
	mins.x -= xPaddingRadius;
	mins.y -= yPaddingRadius;

	maxs.x += xPaddingRadius;
	maxs.y += yPaddingRadius;
}

void AABB2::Translate( float translationX, float translationY ) {
	float radiusX = (maxs.x - mins.x) / 2;
	float radiusY = (maxs.y - mins.y) / 2;
	Vector2 newCenter = Vector2( (mins.x + radiusX) + translationX , (mins.y + radiusY) + translationY );

	*this = AABB2(newCenter, radiusX, radiusY);
}

void AABB2::Translate( const Vector2& translation ) {
	Translate( translation.x , translation.y );
}

bool AABB2::IsPointInside( float x, float y ) const {
	if( x >= mins.x && x <= maxs.x ) {
		if( y >= mins.y && y <= maxs.y ) {
			return true;
		}
	}

	return false;
}

bool AABB2::IsPointInside( const Vector2& point ) const {
	return IsPointInside( point.x , point.y );
}

Vector2 AABB2::GetDimensions() const {
	float dimensionX = (maxs.x - mins.x);
	float dimensionY = (maxs.y - mins.y);
	return Vector2( dimensionX, dimensionY );
}

Vector2 AABB2::GetCenter() const {
	float radiusX = (maxs.x - mins.x) / 2;
	float radiusY = (maxs.y - mins.y) / 2;
	Vector2 center = Vector2( mins.x + radiusX , mins.y + radiusY );

	return center;
}

void AABB2::operator+=( const Vector2& translation ) {
	this->Translate(translation);
}

void AABB2::operator-=( const Vector2& antiTranslation ) {
	this->Translate(Vector2( -antiTranslation.x, -antiTranslation.y ));
}

AABB2 AABB2::operator+( const Vector2& translation ) const {
	AABB2 temp = *this;
	temp.Translate(translation);

	return temp;
}

AABB2 AABB2::operator-( const Vector2& antiTranslation ) const {
	AABB2 temp = *this;
	temp.Translate(Vector2( -antiTranslation.x, -antiTranslation.y ));

	return temp;
}

bool AABB2::DoAABBsOverlap( const AABB2& a, const AABB2& b ) {
	Vector2 centerA = a.GetCenter();
	Vector2 centerB = b.GetCenter();
	Vector2 differenceAB = Vector2( abs(centerA.x - centerB.x) , abs(centerA.y - centerB.y) );

	Vector2 radiusA = a.GetDimensions() / 2.f;
	Vector2 radiusB = b.GetDimensions() / 2.f;

	// Do AABBs overlap on x axis
	if( differenceAB.x <= radiusA.x + radiusB.x ) {
		// Do AABBs overlap on y axis
		if( differenceAB.y <= radiusA.y + radiusB.y ) {
			return true;
		}
	}
	
	return false;
}