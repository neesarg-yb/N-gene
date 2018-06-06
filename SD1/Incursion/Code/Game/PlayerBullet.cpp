#include "PlayerBullet.hpp"
#include "Game/GameCommon.hpp"


PlayerBullet::PlayerBullet( Vector2 position, float shootAtAngle, float speed )
{
	SetPosition( position );
	SetVelocityDirectionInDegree( speed , shootAtAngle );
	m_orientationAngleInDegree = shootAtAngle;
	m_rotationSpeedInDegreePerSeconds = 0.f;
	m_rotationSpeedMultiplier = 1.f;

	m_visualDisc2.radius = 0.05f;
	m_collosionDisc2.radius = 0.01f;
	
	textureBounds = AABB2( Vector2(0.f, 0.f) , m_visualDisc2.radius , m_visualDisc2.radius );
	bulletTexture = g_theRenderer->CreateOrGetTexture( pathToBulletPNG );
	m_health = 3;
}

PlayerBullet::~PlayerBullet()
{

}

void PlayerBullet::Render() {
	// Setup the Graph Paper
	g_theRenderer->GLPushMatrix();
	g_theRenderer->GLTranslate( this->GetPosition().x , this->GetPosition().y , 0.f );
	g_theRenderer->GLRotate( m_orientationAngleInDegree, 0.f, 0.f, 1.f );
	g_theRenderer->GLScale( 1.f, 1.f, 1.f );

	// Draw Bullet
	g_theRenderer->DrawTexturedAABB( textureBounds, *bulletTexture, Vector2(0,1), Vector2(1,0), RGBA_BLUE_COLOR );

	// Reset the Graph Paper
	g_theRenderer->GLPopMatrix();
}

void PlayerBullet::Update( float deltaSeconds ) {
	Vector2 newPos = GetPosition() + ( m_velocity * deltaSeconds );
	SetPosition( newPos );
}