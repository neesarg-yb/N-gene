#pragma once
#include <string>
#include "Engine/Math/Vector2.hpp"

#define BSP_LEAF_RECT_SIZE Vector2( 3.f, 1.5f )

class BSPGraph
{
public:
	 BSPGraph( char const *id );
	~BSPGraph();

public:
	std::string	 m_id		= "N/A";

private:
	BSPGraph	*m_left		= nullptr;
	BSPGraph	*m_right	= nullptr;
	
	mutable Vector2 m_rectSize = BSP_LEAF_RECT_SIZE;				// When Rendering this node, this serves as its AABB2 size

public:
	BSPGraph*	GetLeft();
	BSPGraph*	GetRight();

	void		SetLeft( BSPGraph* newLeft );
	void		SetRight( BSPGraph* newRight );

	bool		IsLeafNode() const;									// Returns true if its both children are nullptr
	bool		IsValid() const;									// Returns true if only one child is present

public:
	Vector2		DebugRender( Vector2 const &centerPos ) const;		// Returns the center position of id_label node

private:
	void		DebugRenderRoot( Vector2 const &centerPos ) const;
	bool		RecomputeRectSize() const;
};

inline bool BSPGraph::IsLeafNode() const
{
	return (m_left == nullptr) && (m_right == nullptr);
}

inline bool BSPGraph::IsValid() const
{
	if( IsLeafNode() )
		return true;												// Both leaves are nullptr => Is Valid
	else
		return (m_left != nullptr) && (m_right != nullptr);			// Both leaves are not nullptr => Is Valid
}
