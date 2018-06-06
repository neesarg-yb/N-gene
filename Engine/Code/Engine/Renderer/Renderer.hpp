#pragma once

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"


class Renderer
{
public:
	Vector2 framesBottomLeft;
	Vector2 framesTopRight;
	Rgba defaultInkColor;
	float defaultDrawingThickness;
	
	Renderer();
	Renderer( const Vector2& bottomLeft, const Vector2& topRight, const Rgba& inkColor, float drawingThickness );
	~Renderer();

	void BeginFrame();
	void EndFrame();

	void ClearScreen( const Rgba& clearColor );
	void SetOrtho( const Vector2& bottomLeft, const Vector2& topRight );
	void DrawLine( const Vector2& start, const Vector2& end, const Rgba& startColor, const Rgba& endColor, float lineThickness );
	void DrawFromVertexArray( const Vector2 vertex[], int arraySize, const Vector2& center, float orientationDegree, float scale );
	void DrawPolygon( const Vector2& center, float radius, float sides, float orientationAngle );
	void DrawDottedPolygon( const Vector2& center, float radius, float sides, float orientationAngle, const Rgba& color );
	
	
private:
};