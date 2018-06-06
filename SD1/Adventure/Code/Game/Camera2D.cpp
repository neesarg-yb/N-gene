#include "Camera2D.hpp"


Camera2D::Camera2D( Entity& followEntity, Map& inMap, float tilesInViewVertically, float aspectRatio )
	: m_followMe( followEntity )
{
	m_numOfTilesVertically	= tilesInViewVertically;
	m_aspectRatio			= aspectRatio;

	PrepCameraForNewMap( inMap );
}

void Camera2D::Update()
{
	Vector2 cameraPosition	= m_followMe.m_position;
	cameraPosition			= m_cameraPositionBounds.GetClosestPointInsideAABB( cameraPosition );

	float camWidth			= m_numOfTilesVertically * m_aspectRatio;
	float camHeight			= m_numOfTilesVertically;
	m_bottomLeftOrtho		= cameraPosition - Vector2(camWidth/2.f, camHeight/2.f);
	m_topRightOrtho			= cameraPosition + Vector2(camWidth/2.f, camHeight/2.f);
}

void Camera2D::Update( Vector2 bottomLeftOrtho, Vector2 topRightOrtho )
{
	// Set ortho
	m_bottomLeftOrtho	= bottomLeftOrtho;
	m_topRightOrtho		= topRightOrtho;
}

void Camera2D::Render()
{
	// Set ortho
	g_theRenderer->SetOrtho( m_bottomLeftOrtho, m_topRightOrtho );
	// Clear all screen (backbuffer) pixels to default_screen_color
	g_theRenderer->ClearScreen(RGBA_BLACK_COLOR);
}

void Camera2D::OverlaySolidAABBOnScreenWithAlpha( const Rgba& boxColor, float alpha )
{
	alpha = ClampFloat(alpha, 0 , 255);

	AABB2 bounds = AABB2( m_bottomLeftOrtho, m_topRightOrtho );
	Rgba colorWithAlpha = Rgba( boxColor.r , boxColor.g , boxColor.b , (unsigned char)alpha );
	g_theRenderer->DrawAABB(bounds , colorWithAlpha);
}

void Camera2D::PrepCameraForNewMap( Map& inMap )
{
	// Set CameraPositionBounds
	int lastTileIndex		= ( inMap.m_dimension.x * inMap.m_dimension.y ) - 1;
	Vector2 mapBoundMins	= Vector2( (float) inMap.m_tiles[0].m_tileCoords.x ,					(float) inMap.m_tiles[0].m_tileCoords.y );
	Vector2 mapBoundMaxs	= Vector2( (float) inMap.m_tiles[ lastTileIndex ].m_tileCoords.x + 1.f,	(float) inMap.m_tiles[ lastTileIndex ].m_tileCoords.y + 1.f );
	float cameraWidth		= m_numOfTilesVertically * m_aspectRatio;
	float cameraHeight		= m_numOfTilesVertically;

	float cbMinsX			= mapBoundMins.x + (cameraWidth/2.f);
	float cbMinsY			= mapBoundMins.y + (cameraHeight/2.f);
	float cbMaxsX			= mapBoundMaxs.x - (cameraWidth/2.f);
	float cbMaxsY			= mapBoundMaxs.y - (cameraHeight/2.f);
	if( cameraHeight > inMap.m_dimension.y ) // If camera hight is larger than map, fix its position to center of map's height
	{
		cbMinsY = inMap.m_dimension.y /2.f;
		cbMaxsY = cbMinsY;
	}
	if( cameraWidth > inMap.m_dimension.x )	// If camera width is larger than map, fix its position to center of map's width
	{
		cbMinsX = inMap.m_dimension.x /2.f;
		cbMaxsX = cbMinsX;
	}
	
	Vector2 cameraBoundMins = Vector2( cbMinsX, cbMinsY );
	Vector2 cameraBoundMaxs = Vector2( cbMaxsX, cbMaxsY );

	m_cameraPositionBounds  = AABB2( cameraBoundMins, cameraBoundMaxs );
}

AABB2 Camera2D::GetCameraBounds() const
{
	return AABB2( m_bottomLeftOrtho, m_topRightOrtho );
}