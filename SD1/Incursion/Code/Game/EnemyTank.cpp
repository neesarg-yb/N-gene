#include "Game/EnemyTank.hpp"
#include "Game/GameCommon.hpp"


EnemyTank::EnemyTank()
{
	// Set all member variables of Entity class
	Vector2 tankPosition = Vector2(10.f, 10.f);
	this->SetPosition( tankPosition );			// Will set center if collision & visual discs, as well
	SetVelocityDirectionInDegree(0.f , 90.f);
	m_orientationAngleInDegree	= 90.f;
	m_visualDisc2.radius		= 0.4f;
	m_collosionDisc2.radius		= 0.32f;
	m_rotationSpeedInDegreePerSeconds = 90.f;

	textureBounds = AABB2( Vector2(0.f, 0.f) , m_visualDisc2.radius , m_visualDisc2.radius );
	tankTexture		= g_theRenderer->CreateOrGetTexture(pathToImagePNG);
	
	m_health = 3.f;


	tankFireSound = g_theAudioSystem->CreateOrGetSound(pathToSound);
}

EnemyTank::EnemyTank( Vector2 position, float orientationInDegrees ) {
	// Set all member variables of Entity class
	this->SetPosition( position );			// Will set center if collision & visual discs, as well
	SetVelocityDirectionInDegree(0.f , orientationInDegrees);
	m_orientationAngleInDegree	= orientationInDegrees;
	m_visualDisc2.radius		= 0.4f;
	m_collosionDisc2.radius		= 0.32f;
	m_rotationSpeedInDegreePerSeconds = 90.f;

	textureBounds = AABB2( Vector2(0.f, 0.f) , m_visualDisc2.radius , m_visualDisc2.radius );
	tankTexture		= g_theRenderer->CreateOrGetTexture(pathToImagePNG);

	m_health = 3.f;


	tankFireSound = g_theAudioSystem->CreateOrGetSound(pathToSound);
}

EnemyTank::~EnemyTank()
{

}

void EnemyTank::Render() {
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

}

void EnemyTank::Update( float deltaSeconds ) {
	Vector2 playerTankPos = g_theGame->currentMap->theTank->GetPosition();
	Vector2 posOfSelf = this->GetPosition();

	static bool followUpToLastPos = false;

	// If I can see player within range
	bool canSeeThePlayer = g_theGame->currentMap->HasLineOfSight( posOfSelf, playerTankPos );
	if( canSeeThePlayer && !g_theGame->currentMap->theTank->isInvisible)
	{
		// Set lastKnownPlayerPos
		goalPosition = playerTankPos;
		followUpToLastPos = true;
		timeElaspedSinceLastGoalChanged = 0.f;

		// Turn my orientation slowly towards player ( 90 degrees/seconds )
		Vector2 directionTowardPlayer = playerTankPos - posOfSelf;
		directionTowardPlayer.NormalizeAndGetLength();
		float targetOrientationDegrees = directionTowardPlayer.GetOrientationDegrees();
		this->rotateTankTowardsDegreeAngle(targetOrientationDegrees , deltaSeconds);

		// If player is in within 90 degree forward direction, drive at moderate pace
		Vector2 selfDirection = Vector2( MathUtil::CosDegree(m_orientationAngleInDegree) , MathUtil::SinDegree(m_orientationAngleInDegree) );
		float minDotProduct = Vector2::DotProductForAngleDifference(45.f);
		if( Vector2::DotProduct(selfDirection, directionTowardPlayer) >= minDotProduct)
		{
			this->moveAheadWithThrottle(maxSpeed , deltaSeconds);


			// If my forward angle is within 5 degree, fire bullets at 2 bullets per seconds
			minDotProduct = Vector2::DotProductForAngleDifference(5.f);
			if( Vector2::DotProduct(selfDirection , directionTowardPlayer) >= minDotProduct)
			{
				EnemyBullet* newBullet = createABullet(deltaSeconds);
				if( newBullet != nullptr ) {
					g_theAudioSystem->PlaySound(tankFireSound, false, 0.06f, 0.0f, 1.0f, false );
					g_theGame->currentMap->AddEntityToList( newBullet , ENTITY_TYPE_ENEMY_BULLET);
				}
			}
		}
	}
	else
	{
		// else go to last remembered position of player
		if( followUpToLastPos == true )
		{
			// If reached to goal..
			if( Disc2::DoDiscAndPointOverlap( this->m_visualDisc2 , goalPosition ) )
			{
				// stop tracing
				followUpToLastPos = false;
			}

			Vector2 directionTowardslastPos = goalPosition - posOfSelf;
			directionTowardslastPos.NormalizeAndGetLength();
			float targetOrientation = directionTowardslastPos.GetOrientationDegrees();

			this->rotateTankTowardsDegreeAngle(targetOrientation, deltaSeconds);
			this->moveAheadWithThrottle(maxSpeed, deltaSeconds);
		}
		else // Wander
		{
			timeElaspedSinceLastGoalChanged += deltaSeconds;
			if( timeElaspedSinceLastGoalChanged >= goalPositionChangeAfterSeconds )
			{
				IntVector2 lastTile = IntVector2( g_theGame->currentMap->gridSize.x - 1 , g_theGame->currentMap->gridSize.y - 1);
				int maxIndexOfTiles = g_theGame->currentMap->GetIndexOfTileAt( lastTile );

				IntVector2 randomTargetPosition = g_theGame->currentMap->m_tiles[ MathUtil::GetRandomNonNegativeIntLessThan( maxIndexOfTiles ) ].m_tileCoords;
				goalPosition = Vector2( (float)randomTargetPosition.x , (float)randomTargetPosition.y );

				timeElaspedSinceLastGoalChanged = 0.f;
			}

			Vector2 directionTowardsGoalPos = goalPosition - posOfSelf;
			directionTowardsGoalPos.NormalizeAndGetLength();
			float targetOrientation = directionTowardsGoalPos.GetOrientationDegrees();

			this->rotateTankTowardsDegreeAngle(targetOrientation, deltaSeconds);
			this->moveAheadWithThrottle(maxSpeed, deltaSeconds);
		}
	}		
}

void EnemyTank::moveAheadWithThrottle( float fractionUptoOne, float deltaTime ) {
	// Set velocity of EnemyTank
	float speed = maxSpeed * fractionUptoOne;
	SetVelocityDirectionInDegree(speed, m_orientationAngleInDegree);

	// Set new position of the tank using velocity
	Vector2 newPosition = this->GetPosition()  + (m_velocity * deltaTime) ;
	SetPosition( newPosition );
}

void EnemyTank::rotateTankTowardsDegreeAngle( float degreeAngle, float deltaTime ) {
	// Tank turns towards degreeAngle according to its rotation speed
	m_orientationAngleInDegree = MathUtil::TurnToward( m_orientationAngleInDegree, degreeAngle, m_rotationSpeedInDegreePerSeconds*deltaTime );
}

EnemyBullet* EnemyTank::createABullet( float deltaSeconds ) {

	if( timeElaspedSinceLastFire > 1/fireBulletsPerSeconds ) {
		Vector2 bulletSpawnPosition = Vector2( (m_collosionDisc2.radius - 0.05f) * MathUtil::CosDegree(m_orientationAngleInDegree) , (m_collosionDisc2.radius - 0.05f) * MathUtil::SinDegree(m_orientationAngleInDegree) );
		bulletSpawnPosition += GetPosition();
		EnemyBullet* newBullet = new EnemyBullet( bulletSpawnPosition , m_orientationAngleInDegree, bulletSpeed );

		timeElaspedSinceLastFire = 0.f;

		return newBullet;
	}
	else {
		timeElaspedSinceLastFire += deltaSeconds;

		return nullptr;
	}
}

void EnemyTank::drawDebugInformations()
{
	// DRAW DISC AT GOAL POSITION
	g_theRenderer->DrawPolygon(goalPosition, 0.1f, 3.f, 0.0f);


	Rgba magentaColor = Rgba(255, 0, 255, 255);
	Rgba cyanColor = Rgba(0, 255, 255, 255);
	Rgba yellowColor = Rgba(255, 255, 0, 255);

	// Drawing VisualDisc
	g_theRenderer->DrawDottedPolygon(m_visualDisc2.center, m_visualDisc2.radius, 24.f, 0.f, magentaColor);

	// Drawing CollosionDisc
	g_theRenderer->DrawDottedPolygon(m_collosionDisc2.center, m_collosionDisc2.radius, 24.f, 0.f, cyanColor);

	// Drawing VelocityVector
	Vector2 endOfVelocityVector = GetPosition() + m_velocity;
	g_theRenderer->DrawLine(GetPosition(), endOfVelocityVector, yellowColor, yellowColor, 1.5f);
}