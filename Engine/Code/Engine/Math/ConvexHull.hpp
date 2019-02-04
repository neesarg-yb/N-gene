#pragma once
#include <vector>
#include "Engine/Math/Plane3.hpp"
#include "Engine/Core/EngineCommon.hpp"

class ConvexPolyhedron;

class ConvexHull
{
	friend ConvexPolyhedron;

public:
	 ConvexHull();
	~ConvexHull();

private:
	std::vector< Plane3 > m_planes;

public:
	void AddPlane		( Plane3 const &newPlane );
	bool IsPointOutside	( Vector3 const &testP ) const;

public:
	inline uint GetPlaneCount() const { return (uint)m_planes.size(); }
};
