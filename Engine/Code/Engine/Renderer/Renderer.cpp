#pragma once

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#include "Engine/Renderer/Renderer.hpp"


Renderer::Renderer()
{
	framesBottomLeft = Vector2(0.f, 0.f);
	framesTopRight = Vector2(1000.f, 1000.f);
	defaultInkColor = Rgba(255, 255, 255, 255);
	defaultDrawingThickness = 1.5f;

	glLineWidth( defaultDrawingThickness );
	glEnable( GL_BLEND );
	glEnable( GL_LINE_SMOOTH );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

Renderer::Renderer( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& inkColor, float drawingThickness ) {
	framesBottomLeft = bottomLeft;
	framesTopRight = topRight;
	this->defaultInkColor = inkColor;
	this->defaultDrawingThickness = drawingThickness;
}

Renderer::~Renderer()
{
	
}

void Renderer::BeginFrame() {

}

void Renderer::EndFrame() {

}

void Renderer::ClearScreen( const Rgba& clearColor ) {
	float r, g, b, a;
	clearColor.GetAsFloats(r, g, b, a);					// [0, 255] to [0.f, 1.f]

	// Clear all screen (backbuffer) pixels to given color
	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Renderer::SetOrtho( const Vector2& bottomLeft, const Vector2& topRight ) {
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (100,100) top-right
	glLoadIdentity();
	glOrtho( bottomLeft.x, topRight.x, bottomLeft.y, topRight.y, 0.f, 1.f );
}

void Renderer::DrawLine( const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float lineThickness ) {
	// Setting line's thickness
	glLineWidth(lineThickness);

	// Start drawing line
	glBegin( GL_LINES );
	
	// Setting start color
	glColor4ub(startColor.r, startColor.g, startColor.b, startColor.a);
	glVertex2f(start.x , start.y);

	// Setting end color
	glColor4ub(endColor.r, endColor.g, endColor.b, endColor.a);
	glVertex2f(end.x , end.y);
	glEnd();

	// Reset ink color & thickness
	glColor4ub(defaultInkColor.r, defaultInkColor.g, defaultInkColor.b, defaultInkColor.a);
	glLineWidth(defaultDrawingThickness);
}

void Renderer::DrawFromVertexArray( const Vector2 vertex[], int arraySize, const Vector2& center, float orientationDegree, float scale ) {
	glPushMatrix();
	glTranslatef(center.x, center.y, 0.f);
	glRotatef(orientationDegree, 0.f, 0.f, 1.f);
	glScalef(scale, scale, scale);

	// For loop for every line
	for(int i=0; i<arraySize-1; i++) {
		DrawLine(vertex[i], vertex[i+1], defaultInkColor, defaultInkColor, defaultDrawingThickness);
	} 
	DrawLine(vertex[arraySize-1], vertex[0], defaultInkColor, defaultInkColor, defaultDrawingThickness);

	glPopMatrix();
}

void Renderer::DrawPolygon( const Vector2& center, float radius, float sides, float orientationAngle ) {
	// For loop for every line
	for(float i=0; i<sides; i++) {
		float startAngle = ( i * (360.f/sides) );
		float endAngle = startAngle + (360.f/sides);

		Vector2 startPoint;
		Vector2 endPoint;

		startPoint.x = ( center.x + (radius * MathUtil::CosDegree(startAngle + orientationAngle)));
		startPoint.y = ( center.y + (radius * MathUtil::SinDegree(startAngle + orientationAngle)));
		endPoint.x   = ( center.x + (radius * MathUtil::CosDegree(endAngle + orientationAngle)));
		endPoint.y   = ( center.y + (radius * MathUtil::SinDegree(endAngle + orientationAngle)));

		DrawLine(startPoint, endPoint, defaultInkColor, defaultInkColor, defaultDrawingThickness);
	}
}

void Renderer::DrawDottedPolygon( const Vector2& center, float radius, float sides, float orientationAngle, const Rgba& color ) {
	// For loop for every line
	for(float i=0; i<sides; i+=2) {
		float startAngle = ( i * (360.f/sides) );
		float endAngle = startAngle + (360.f/sides);

		Vector2 startPoint;
		Vector2 endPoint;

		startPoint.x = ( center.x + (radius * MathUtil::CosDegree(startAngle + orientationAngle)));
		startPoint.y = ( center.y + (radius * MathUtil::SinDegree(startAngle + orientationAngle)));
		endPoint.x   = ( center.x + (radius * MathUtil::CosDegree(endAngle + orientationAngle)));
		endPoint.y   = ( center.y + (radius * MathUtil::SinDegree(endAngle + orientationAngle)));

		DrawLine(startPoint, endPoint, color, color, defaultDrawingThickness);
	}
}