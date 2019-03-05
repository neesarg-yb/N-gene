#include "BSPGraph.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"

BSPGraph::BSPGraph( char const *id )
	: m_id( id )
{

}

BSPGraph::~BSPGraph()
{

}

BSPGraph* BSPGraph::GetLeft()
{
	return m_left;
}

BSPGraph* BSPGraph::GetRight()
{
	return m_right;
}

void BSPGraph::SetLeft( BSPGraph* newLeft )
{
	m_left = newLeft;
}

void BSPGraph::SetRight( BSPGraph* newRight )
{
	m_right = newRight;
}

Vector2 BSPGraph::DebugRender( Vector2 const &centerPos ) const
{
	bool recomupteSuccess = RecomputeRectSize();
	if(  recomupteSuccess == false  )
	{
		// It means that the BSP tree is incomplete!
		DebugRender2DText( 0.f, centerPos, m_rectSize.y, RGBA_RED_COLOR, RGBA_RED_COLOR, "FAILED!" );
		return centerPos;
	}
	
	if( IsLeafNode() )
	{
		// Draw this node's root
		DebugRenderRoot( centerPos );

		return centerPos;
	}
	else
	{
		// Box containing this whole node
		AABB2	thisRectAABB2	= AABB2( centerPos, m_rectSize.x * 0.5f, m_rectSize.y * 0.5f );

		// Box containing just left child
		Vector2	leftChildMins	= thisRectAABB2.mins;
		AABB2	leftChildAABB2	= AABB2( leftChildMins, leftChildMins + m_left->m_rectSize );

		// Box containing just right child
		Vector2	rightChildMins	= Vector2( leftChildAABB2.maxs.x, leftChildAABB2.mins.y );
		AABB2	rightChildAABB2	= AABB2( rightChildMins, rightChildMins + m_right->m_rectSize );

		// Size of this node's root
		Vector2	rootNodeSize	= BSP_LEAF_RECT_SIZE;
		rootNodeSize.y = rootNodeSize.x;
		
		// Center of this node's root 
		Vector2	rootCenterPos;
		rootCenterPos.x = rightChildMins.x;
		rootCenterPos.y = (thisRectAABB2.maxs.y + rightChildAABB2.maxs.y) * 0.5f;

		// Center of children's boxes
		Vector2 leftChildCenterPos  = leftChildAABB2.GetCenter();
		Vector2 rightChildCenterPos = rightChildAABB2.GetCenter();

		// Draw left & right children
		float	circleRadius		= min( BSP_LEAF_RECT_SIZE.x, BSP_LEAF_RECT_SIZE.y) * 0.5f;
		Vector2 leftChildRootPos	= m_left->DebugRender( leftChildCenterPos ) + Vector2( 0.f, circleRadius );
		Vector2 rightChildRootPos	= m_right->DebugRender( rightChildCenterPos ) + Vector2( 0.f, circleRadius );
		
		// Draw connecting line from root to its children
		DebugRender2DLine( 0.f, rootCenterPos, RGBA_GRAY_COLOR, leftChildRootPos,  RGBA_GRAY_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );
		DebugRender2DLine( 0.f, rootCenterPos, RGBA_GRAY_COLOR, rightChildRootPos, RGBA_GRAY_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );

		// Draw this node's root, now!
		DebugRenderRoot( rootCenterPos );

		return rootCenterPos;
	}
}

void BSPGraph::DebugRenderRoot( Vector2 const &centerPos ) const
{
	bool isLeafNode		= IsLeafNode();
	float circleRadius	= min( BSP_LEAF_RECT_SIZE.x, BSP_LEAF_RECT_SIZE.y) * 0.5f;
	AABB2 rectAABB2		= AABB2( centerPos, BSP_LEAF_RECT_SIZE.x * 0.4f, BSP_LEAF_RECT_SIZE.y * 0.5f );

	// Draw background
	if( isLeafNode )
		DebugRender2DQuad( 0.f, rectAABB2, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );							// Rectangle for a leaf root
	else
		DebugRender2DRound( 0.f, centerPos, circleRadius, 12U, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR );		// Circle for a non-leaf root
		
	// Print the ID
	Vector2 textPosition = centerPos - (Vector2( circleRadius, circleRadius ) * 0.5f);
	if( isLeafNode )
		textPosition = rectAABB2.mins + (rectAABB2.GetDimensions() * 0.15f);

	DebugRender2DText ( 0.f, textPosition, circleRadius, RGBA_RED_COLOR, RGBA_RED_COLOR, m_id );
}

bool BSPGraph::RecomputeRectSize() const
{
	// Only Operate on a valid BSP node
	if( IsValid() == false )
		return false;

	// If this is leaf, we don't need to compute
	if( IsLeafNode() )
		return true;

	bool leftRecompSuccess  = m_left->RecomputeRectSize();
	bool rightRecompSuccess = m_right->RecomputeRectSize();

	// Only proceed if recomputation on both children was a success
	if( !leftRecompSuccess || !rightRecompSuccess )
		return false;

	// A Square!
	Vector2 idNodeSize = BSP_LEAF_RECT_SIZE;
	idNodeSize.y = idNodeSize.x;

	Vector2 leftChildRectSize  = m_left->m_rectSize;
	Vector2 rightChildRectSize = m_right->m_rectSize;

	m_rectSize.x = max( idNodeSize.x, leftChildRectSize.x + rightChildRectSize.x );
	m_rectSize.y = max( leftChildRectSize.y, rightChildRectSize.y ) + idNodeSize.y;

	return true;
}
