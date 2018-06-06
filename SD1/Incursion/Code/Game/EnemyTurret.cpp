#include "EnemyTurret.hpp"
#include "Game/GameCommon.hpp"


EnemyTurret::EnemyTurret( const Vector2& position, float orientationDegrees )
{
	SetPosition(position);
	m_orientationAngleInDegree = 0.f;
	m_velocity = Vector2( 0.f, 0.f );

	m_collosionDisc2.radius = 0.4f;
	m_visualDisc2.radius = 0.5f;

	turretOrientationInDegrees = orientationDegrees;
	turretRotationSpeedDegreesPerSeconds = 30.f;

	baseTexture = g_theRenderer->CreateOrGetTexture(pathToBasePNG);
	turretTexture = g_theRenderer->CreateOrGetTexture(pathToTurretPNG);
	textureBounds = AABB2( Vector2(0.f, 0.f) , m_visualDisc2.radius , m_visualDisc2.radius );

	m_health = 5.f;

	m_rotationSpeedMultiplier = MathUtil::GetRandomFloatAsPlusOrMinusOne();

	turretFireSound = g_theAudioSystem->CreateOrGetSound(pathToSound);
}

EnemyTurret::~EnemyTurret()
{

}

void EnemyTurret::Render() {
// DRAW BASE
	// Set graphPaper
	g_theRenderer->GLPushMatrix();
	g_theRenderer->GLTranslate( this->GetPosition().x , this->GetPosition().y , 0.f);
	g_theRenderer->GLRotate(m_orientationAngleInDegree, 0.f, 0.f, 1.f);
	g_theRenderer->GLScale(1.f, 1.f, 1.f);

	// Draw base
	g_theRenderer->DrawTexturedAABB( textureBounds, *baseTexture, Vector2(0,1), Vector2(1,0), Rgba(255, 255, 255, 255) );

	// Reset graphPaper
	g_theRenderer->GLPopMatrix();


// DRAW DEBUG.RAY
	Vector2 turretOrientationDirection = Vector2( MathUtil::CosDegree(this->turretOrientationInDegrees) , MathUtil::SinDegree(this->turretOrientationInDegrees));
	RaycastResult debugRay = g_theGame->currentMap->Raycast( this->GetPosition() , turretOrientationDirection , 10);
	// Draw it
	float fractionCovered = debugRay.m_atFractionTravelled;
	Rgba EndPointRGBA = Rgba( 255, 0, 0, (unsigned char) (255 * (1- fractionCovered)) );
	// If Raycast hit
	if( debugRay.m_didImpact == true )
	{
		g_theRenderer->DrawLine( this->GetPosition() , debugRay.m_impactPosition, RGBA_RED_COLOR, EndPointRGBA, 1.5f);
	}
	else 
	{
		// Else if it didn't hit
		g_theRenderer->DrawLine( this->GetPosition() , this->GetPosition() + (turretOrientationDirection * 10.f) , RGBA_RED_COLOR, Rgba(255, 0, 0, 0) , 1.5f);
	}

// DRAW TURRET
	// Set graphPaper
	g_theRenderer->GLPushMatrix();
	g_theRenderer->GLTranslate( this->GetPosition().x , this->GetPosition().y , 0.f);
	g_theRenderer->GLRotate(turretOrientationInDegrees, 0.f, 0.f, 1.f);
	g_theRenderer->GLScale(1.f, 1.f, 1.f);

	// Draw Tank
	g_theRenderer->DrawTexturedAABB( textureBounds, *turretTexture, Vector2(0,1), Vector2(1,0), Rgba(255, 255, 255, 255) );

	// Reset graphPaper
	g_theRenderer->GLPopMatrix();
}

void EnemyTurret::Update( float deltaSeconds ) {
	Entity* playerTank = g_theGame->currentMap->theTank;
	float distanceToPlayerTank = Vector2::GetDistance( playerTank->GetPosition() , this->GetPosition() );
	bool hasSight = g_theGame->currentMap->HasLineOfSight(this->GetPosition() , playerTank->GetPosition());

	if( hasSight && distanceToPlayerTank <= 10.f && g_theGame->currentMap->theTank->isInvisible == false ) 
	{
		// Found playerTank, try to shoot it..
		Vector2 directionToLookAt = playerTank->GetPosition() - this->GetPosition();
		directionToLookAt.NormalizeAndGetLength();
		float orientationDegreeToLookAt = directionToLookAt.GetOrientationDegrees();

		turretOrientationInDegrees = MathUtil::TurnToward( turretOrientationInDegrees , orientationDegreeToLookAt , turretRotationSpeedDegreesPerSeconds * deltaSeconds );

		Vector2 turretsFacingDirection = Vector2( MathUtil::CosDegree(turretOrientationInDegrees) , MathUtil::SinDegree(turretOrientationInDegrees) );
		if( Vector2::DotProduct( turretsFacingDirection, directionToLookAt) > Vector2::DotProductForAngleDifference(5) )
		{
			// Fire
			EnemyBullet* newBullet = createABullet(deltaSeconds);
			if( newBullet != nullptr) {
				g_theAudioSystem->PlaySound(turretFireSound, false, 0.04f, 0.0f, 1.0f, false);
				g_theGame->currentMap->AddEntityToList(newBullet , ENTITY_TYPE_ENEMY_BULLET);
			}
		}
	}
	else
	{
		// Go into, wander behavior
		turretOrientationInDegrees += turretRotationSpeedDegreesPerSeconds * deltaSeconds * m_rotationSpeedMultiplier;
	}

}


EnemyBullet* EnemyTurret::createABullet( float deltaSeconds ) {

	if( timeElaspedSinceLastFire > 1/fireBulletsPerSeconds ) {
		Vector2 bulletSpawnPosition = Vector2( (m_collosionDisc2.radius - 0.05f) * MathUtil::CosDegree(turretOrientationInDegrees) , (m_collosionDisc2.radius - 0.05f) * MathUtil::SinDegree(turretOrientationInDegrees) );
		bulletSpawnPosition += GetPosition();
		EnemyBullet* newBullet = new EnemyBullet( bulletSpawnPosition , turretOrientationInDegrees, bulletSpeed );

		timeElaspedSinceLastFire = 0.f;

		return newBullet;
	}
	else {
		timeElaspedSinceLastFire += deltaSeconds;

		return nullptr;
	}
}