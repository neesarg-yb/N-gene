#include "Polygon2.hpp"


Polygon2::Polygon2()
{

}

Polygon2::Polygon2(float centerX, float centerY, float radius, float sides) {
	this->center.x = centerX;
	this->center.y = centerY;
	this->radius = radius;
	this->sides = sides;
}

Polygon2::Polygon2(const Vector2& center, float radius, float sides) {
	this->center = center;
	this->radius = radius;
	this->sides = sides;
}

Polygon2::~Polygon2()
{

}
