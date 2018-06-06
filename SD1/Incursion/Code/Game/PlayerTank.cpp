#include "Game/PlayerTank.hpp"
#include "Game/GameCommon.hpp"


PlayerTank::PlayerTank()
{
	// Set all member variables of Entity class
	Vector2 tankPosition = Vector2(10.f, 10.f);
	this->SetPosition( tankPosition );			// Will set center if collision & visual discs, as well
	SetVelocityDirectionInDegree(0.f , 90.f);
	m_orientationAngleInDegree	= 90.f;
	m_visualDisc2.radius		= 0.4f;
	m_collosionDisc2.radius		= 0.32f;
	m_rotationSpeedInDegreePerSeconds = 180.f;

	textureBounds = AABB2( Vector2(0.f, 0.f) , m_visualDisc2.radius , m_visualDisc2.radius );
	tankTexture		= g_theRenderer->CreateOrGetTexture(pathToBasePNG);
	turretTexture	= g_theRenderer->CreateOrGetTexture(pathToTurretPNG);
	turretOrientationInDegrees = 45.f;
	turretRotationSpeedDegreesPerSeconds = 360.f;
	
	m_health = 10.f;

	tankFireSound = g_theAudioSystem->CreateOrGetSound(pathToSound);

	defaultFireRate = fireBulletsPerSeconds;
}

PlayerTank::PlayerTank( Vector2 position, float orientationInDegrees ) {
	// Set all member variables of Entity class
	this->SetPosition( position );			// Will set center if collision & visual discs, as well
	SetVelocityDirectionInDegree(0.f , orientationInDegrees);
	m_orientationAngleInDegree	= orientationInDegrees;
	m_visualDisc2.radius		= 0.4f;
	m_collosionDisc2.radius		= 0.32f;
	m_rotationSpeedInDegreePerSeconds = 180.f;

	textureBounds = AABB2( Vector2(0.f, 0.f) , m_visualDisc2.radius , m_visualDisc2.radius );
	tankTexture		= g_theRenderer->CreateOrGetTexture(pathToBasePNG);
	turretTexture	= g_theRenderer->CreateOrGetTexture(pathToTurretPNG);
	turretOrientationInDegrees = 45.f;
	turretRotationSpeedDegreesPerSeconds = 360.f;

	m_health = 10.f;
	tankFireSound = g_theAudioSystem->CreateOrGetSound(pathToSound);


	defaultFireRate = fireBulletsPerSeconds;
}

PlayerTank::~PlayerTank()
{

}

void PlayerTank::Render() {
	if( isInvisible )
		return;

// DRAW TANK
	// Set graphPaper
	g_theRenderer->GLPushMatrix();
	g_theRenderer->GLTranslate( this->GetPosition().x , this->GetPosition().y , 0.f);
	g_theRenderer->GLRotate(m_orientationAngleInDegree, 0.f, 0.f, 1.f);
	g_theRenderer->GLScale(1.f, 1.f, 1.f);

	// Draw Tank
	g_theRenderer->DrawTexturedAABB( textureBounds, *tankTexture, Vector2(0,1), Vector2(1,0), Rgba(255, 255, 255, 255) );

	// Reset graphPaper
	g_theRenderer->GLPopMatrix();

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

// DRAW OVERHEAT METER
	// Set graphPaper
	g_theRenderer->GLPushMatrix();
	g_theRenderer->GLTranslate( this->GetPosition().x , this->GetPosition().y + ( this->m_visualDisc2.radius * ( 1 + 0.1f ) ), 0.f);
	g_theRenderer->GLRotate(0.f, 0.f, 0.f, 1.f);
	g_theRenderer->GLScale(1.f, 1.f, 1.f);

	// Draw meter
	Vector2 baseBarDimension = Vector2(this->m_visualDisc2.radius, 0.06f * this->m_visualDisc2.radius); 
	AABB2 baseBounds = AABB2( Vector2(0.f,0.f) , baseBarDimension.x , baseBarDimension.y );
	g_theRenderer->DrawAABB( baseBounds , RGBA_WHITE_COLOR );

	Vector2 heatBarDimension = Vector2(this->m_visualDisc2.radius * gunOverheatFraction * -1.f , 0.05f * this->m_visualDisc2.radius);
	Vector2 heatBarAnchorPoint = Vector2( (-baseBarDimension.x) - (heatBarDimension.x), (baseBarDimension.y/2.f) - (heatBarDimension.y/2.f) );
	AABB2 heatBounds = AABB2( heatBarAnchorPoint , heatBarDimension.x , heatBarDimension.y );
	Rgba indicatorColor = gunOverheatFraction > 0.5f ? RGBA_RED_COLOR : RGBA_BLUE_COLOR /*Rgba( 0 , 150 , 0 , 255)*/;
	g_theRenderer->DrawAABB( heatBounds , indicatorColor );

	// Reset graphPaper
	g_theRenderer->GLPopMatrix();
}

void PlayerTank::Update( float deltaSeconds ) {
	gunOverheatFraction = currentOverheatInTimeUnits / maxOverheatAfterTime;

	if( gunOverheatFraction >= 0.5 )
	{
		// Reduce fire rate by 1
		fireBulletsPerSeconds = defaultFireRate - (int)(3 * gunOverheatFraction);
	}
	else
	{
		// Increase fire rate by 1
		fireBulletsPerSeconds = defaultFireRate;
	}

	if( this->isInvisible )
		return;

	// UPDATE PLAYER_TANK
	// Get Xbox controller inputs
	float magnitude = g_theInput->m_controller->m_xboxStickStates[ XBOX_STICK_LEFT ].magnitude;
	float orientation = g_theInput->m_controller->m_xboxStickStates[ XBOX_STICK_LEFT ].orientationDegree;
	if( magnitude > 0 ) 
	{
		this->moveAheadWithThrottle(magnitude, deltaSeconds);
		this->rotateTankTowardsDegreeAngle(orientation, deltaSeconds);
	}


	// UPDATE PLAYER_TURRET
	// Get Xbox controller inputs
	magnitude = g_theInput->m_controller->m_xboxStickStates[ XBOX_STICK_RIGHT ].magnitude;
	orientation = g_theInput->m_controller->m_xboxStickStates[ XBOX_STICK_RIGHT ].orientationDegree;
	if( magnitude > 0 )
	{
		this->rotateTurretTowardsDegreeAngle(orientation, deltaSeconds);
	}

}

void PlayerTank::moveAheadWithThrottle( float fractionUptoOne, float deltaTime ) {
	// Set velocity of PlayerTank
	float speed = maxSpeed * fractionUptoOne;
	SetVelocityDirectionInDegree(speed, m_orientationAngleInDegree);

	// Set new position of the tank using velocity
	Vector2 newPosition = this->GetPosition()  + (m_velocity * deltaTime) ;
	SetPosition( newPosition );
}

void PlayerTank::rotateTankTowardsDegreeAngle( float degreeAngle, float deltaTime ) {
	// Tank turns towards degreeAngle according to its rotation speed
	m_orientationAngleInDegree = MathUtil::TurnToward( m_orientationAngleInDegree, degreeAngle, m_rotationSpeedInDegreePerSeconds*deltaTime );
}


void PlayerTank::rotateTurretTowardsDegreeAngle( float degreeAngle, float deltaTime ) {
	// Tank turns towards degreeAngle according to its rotation speed
	turretOrientationInDegrees = MathUtil::TurnToward( turretOrientationInDegrees, degreeAngle, turretRotationSpeedDegreesPerSeconds*deltaTime );
}

PlayerBullet* PlayerTank::createABullet( float deltaSeconds ) {

	if( timeElaspedSinceLastFire > 1/fireBulletsPerSeconds ) {
		Vector2 bulletSpawnPosition = Vector2( (m_collosionDisc2.radius - 0.05f) * MathUtil::CosDegree(turretOrientationInDegrees) , (m_collosionDisc2.radius - 0.05f) * MathUtil::SinDegree(turretOrientationInDegrees) );
		bulletSpawnPosition += GetPosition();

		float bulletFireOrientation = turretOrientationInDegrees;

		if( gunOverheatFraction > 0.5f )
		{
			float plusOrMinusSign = MathUtil::GetRandomFloatAsPlusOrMinusOne();
			float randomAngleModifer = MathUtil::GetRandomFloatInRange( 0.f , 25.f * gunOverheatFraction );
			bulletFireOrientation = turretOrientationInDegrees + (plusOrMinusSign * randomAngleModifer );
		}

		PlayerBullet* newBullet = new PlayerBullet( bulletSpawnPosition , bulletFireOrientation, bulletSpeed );
		
		timeElaspedSinceLastFire = 0.f;
		g_theAudioSystem->PlaySound(tankFireSound);

		return newBullet;
	}
	else {
		timeElaspedSinceLastFire += deltaSeconds;

		return nullptr;
	}
}

void PlayerTank::TakeOneHitFromBullet() {
	m_health -= 1;
	if( m_health <= 0)
	{
		isInvisible = true;
	}
}