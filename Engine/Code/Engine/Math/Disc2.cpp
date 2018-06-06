#include "Disc2.hpp"


Disc2::Disc2()
{

}

Disc2::~Disc2()
{

}

Disc2::Disc2(const Disc2& copyFrom) {
	*this = copyFrom;
}

Disc2::Disc2(float initialX, float initialY, float initialRadius) {
	this->center = Vector2(initialX, initialY);
	this->radius = initialRadius;
}

Disc2::Disc2(const Vector2& initialCenter, float initialRadius) {
	this->center = initialCenter;
	this->radius = initialRadius;
}

void Disc2::operator += (const Vector2& traslation) {
	this->center += traslation;
}

void Disc2::operator -= (const Vector2& traslation) {
	this->center -= traslation;
}

Disc2 Disc2::operator + (const Vector2& traslation) const {
	Disc2 tempDisc2 = Disc2( (this->center + traslation), this->radius );

	return tempDisc2;
}

Disc2 Disc2::operator - (const Vector2& traslation) const {
	Disc2 tempDisc2 = Disc2( (this->center - traslation), this->radius );

	return tempDisc2;
}

void Disc2::StretchToIncludePoint(float x, float y) {
	float distanceFromCenter = Vector2::GetDistance(center, Vector2(x,y));
	if( distanceFromCenter > radius ) {
		radius += (distanceFromCenter - radius);
	}
}

void Disc2::StretchToIncludePoint(const Vector2& point) {
	float distanceFromCenter = Vector2::GetDistance(center, point);
	if( distanceFromCenter > radius ) {
		radius += (distanceFromCenter - radius);
	}
}

void Disc2::AddPaddingToRadius(float paddingRadius) {
	this->radius += paddingRadius;
}

void Disc2::Translate(float translationX, float translationY) {
	this->center.x += translationX;
	this->center.y += translationY;
}

void Disc2::Translate(const Vector2& translation) {
	this->center += translation;
}

bool Disc2::IsPointInside(float x, float y) const {
	float distanceFromCenter = Vector2::GetDistance(this->center, Vector2(x,y));
	if(distanceFromCenter <= radius) {
		return true;
	}

	return false;
}

bool Disc2::IsPointInside(const Vector2& point) const {
	float distanceFromCenter = Vector2::GetDistance(this->center, point);
	if(distanceFromCenter <= radius) {
		return true;
	}

	return false;
}

bool Disc2::DoDiscsOverlap(const Vector2& aCenter, float aRadius, const Vector2& bCenter, float bRadius) {
	float distanceBetweenTwoDiscs = Vector2::GetDistance(aCenter, bCenter);
	float sumOfBothRadius = aRadius + bRadius;
	if(distanceBetweenTwoDiscs <= sumOfBothRadius) {
		return true;
	}

	return false;
}

bool Disc2::DoDiscsOverlap(const Disc2& a, const Disc2& b) {
	return DoDiscsOverlap(a.center, a.radius, b.center, b.radius);
}

bool Disc2::DoDiscAndPointOverlap(const Disc2& disc, const Vector2& point) {
	return Vector2::GetDistance(disc.center, point) <= disc.radius;
}

const Disc2 Interpolate( const Disc2& start, const Disc2& end, float fractionTowardEnd )
{
	Disc2 resultDisc = Disc2();

	resultDisc.center = Interpolate( start.center , end.center , fractionTowardEnd );
	resultDisc.radius = Interpolate( start.radius , end.radius , fractionTowardEnd );

	return resultDisc;
}