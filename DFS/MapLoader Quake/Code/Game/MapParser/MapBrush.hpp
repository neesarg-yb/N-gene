#pragma once
#include <string>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Game/MapParser/MapFileBuffer.hpp"

class MapEntity;

struct MapPlane
{
public:
	// Plane
	Vector3			planeDescriptionPoints[3]	= { Vector3::ZERO, Vector3::ZERO, Vector3::ZERO };

	// Texture File Name
	std::string		textureName					= "default";

	// Texture Coordinate Data
	Vector2			texturePositionOffset		= Vector2::ZERO;
	float			textureRotation				= 0.f;
	Vector2			textureScale				= Vector2::ONE_ONE;

public:
	MapPlane();
	MapPlane( Vector3 *planeDescriptionThreePoints, std::string const &texName, Vector2 const &texPositionOffset, float texRotation, Vector2 const &texScale );
};

class MapBrush
{
	friend MapEntity;

public:
	std::vector< MapPlane > m_planes;

public:
	void AddPlane( MapPlane const &plane );
	void AddPlane( Vector3 *planeDescriptionThreePoints, std::string const &texName, Vector2 const &texPositionOffset, float texRotation, Vector2 const &texScale );

private:
	static MapBrush* ParseFromBuffer( MapFileBuffer &buffer ); // Returns nullptr on failure
};
