#include "theGame.hpp"


theGame::theGame()
{
	lastFramesTime = GetCurrentTimeSeconds();
	MathUtil::NewSeedForRandom();

	spawnNewShip();

	for(int i=0; i<MAX_PLAYER_LIVES; i++) {
		remainingLivesIcons[i] = new Ship();
	}
}

theGame::~theGame()
{
	for(int i=0; i<asteroidsCount; i++) {
		delete asteroids[i];
	}
}


void theGame::Update() {
	// Reflect all inputs
	ReflectAllInputChanges();

	// Calculating deltaTime
	float deltaSeconds = CalculateDeltaTime();
	
	// Handling SlowMo and TimePause
	if(slowMotionON) {
		deltaSeconds *= 0.10f;
	}
	if(gamePaused) {
		deltaSeconds = 0;
	}

	if(remainingLivesCount == 0) {
		ResetGameAfterSeconds(5.f, deltaSeconds);		
	}

	// Spawn new wave of Asteroids if there are none in the current game
	if(asteroidsCount == 0) {
		for(int i=0; i<nextWaveOfAsteroids; i++) {
			createNewAsteroid();
		}

		// new wave will be increased by one more Asteroid
		nextWaveOfAsteroids++;
	}

	// For this frame, check collisions
	checkForCollosion();
	
	// Manipulate playerShip
	if(shipIsDead == false) {
		playerSpaceShip->m_thrustButtonPressed = accelerateShip;		// Will help to draw flames
		if(accelerateShip == true) {
			playerSpaceShip->thrustShipAheadByUnitsPerSeconds(200.f * accelerateShipMultiplier, deltaSeconds);
		}

		if(rotateShipAntiClockWise == true) {
			playerSpaceShip->rotateShipByDegreesPerSeconds(1.5f * 180.f, deltaSeconds);
		}

		if(rotateShipClockWise == true) {
			playerSpaceShip->rotateShipByDegreesPerSeconds(-1.5f * 180.f, deltaSeconds);
		}
		
		playerSpaceShip->Update(deltaSeconds);
	}
	

	// Shoot bullets is Fire key is down
	if(shootBullet == true) {
		shootNewBullet();		// Bullet gets added to bullets array
		shootBullet = false;
	}

	// Update all asteroids
	for(int i=0; i<MAX_ASTEROIDS; i++) {
		if(asteroids[i] != nullptr) {
			asteroids[i]->Update(deltaSeconds);
		}
	}

	// Update all bullets
	for(int i=0; i<MAX_BULLETS; i++) {
		if(bullets[i] != nullptr) {
			if(bullets[i]->m_isDead) {
				delete bullets[i];
				bullets[i] = nullptr;
			} else {
				bullets[i]->Update(deltaSeconds);
			}
		}
	}

}


void theGame::Render() {
	DisplayRemainingPlayerLives();

	for(int i=0; i<MAX_ASTEROIDS; i++) {
		if(asteroids[i] != nullptr) {
			asteroids[i]->drawAsteroid();
			if(debugModeON == true) {
				asteroids[i]->drawDebugInformations();
			}
		}
	}

	for(int i=0; i<MAX_BULLETS; i++) {
		if(bullets[i] != nullptr) {
			bullets[i]->drawBullet();
			if(debugModeON == true) {
				bullets[i]->drawDebugInformations();
			}
		}
	}

	if(shipIsDead == false) {
		playerSpaceShip->drawShip();
		if(debugModeON == true) {
			playerSpaceShip->drawDebugInformations();
		}
	}
}


void theGame::createNewAsteroid() {

	int indexOfEmptyAsteroidSlot;
	bool success = getEmptySlotIndexFromAsteroidsArray(indexOfEmptyAsteroidSlot);
	if(success) {
		float x = MathUtil::GetRandomFloatInRange(1200, 1800.f);	
		float y = MathUtil::GetRandomFloatInRange(1200.f, 1800.f);
		float size = asteroidSizes[ ASTEROID_LARGE ];

		asteroids[indexOfEmptyAsteroidSlot] = new Asteroid( x , y, size);
					
		asteroidsCount++;
	}
}

void theGame::createNewAsteroids(AsteroidSizes theSize, int totalNew, Vector2 position) {
	int indexOfEmptyAsteroidSlot;
	for(int i=0; i<totalNew; i++) {
		bool success = getEmptySlotIndexFromAsteroidsArray(indexOfEmptyAsteroidSlot);
		if(success) {
			float x = position.x;	
			float y = position.y;
			float size = asteroidSizes[ theSize ];

			asteroids[indexOfEmptyAsteroidSlot] = new Asteroid( x , y, size);

			asteroidsCount++;
		}
	}
}


void theGame::deleteFirstAsteroid() {
	for(int i=0; i<MAX_ASTEROIDS; i++) {
		if(asteroids[i] != nullptr) {
			delete asteroids[i];
			asteroids[i] = nullptr;
			asteroidsCount--;

			break;
		}
	}

}

void theGame::shootNewBullet() {
	if(shipIsDead == true) {
		return;
	}

	int referenceIndexToBulletSlot;
	if(getEmptySlotIndexFromBulletsArray(referenceIndexToBulletSlot) == true) {
		Vector2 spawnPoint = playerSpaceShip->getBulletSpawnPoint();
		float orientationDegree = playerSpaceShip->m_orientationAngleInDegree;
		Vector2 bulletUnitVelocity = Vector2(MathUtil::CosDegree(orientationDegree), MathUtil::SinDegree(orientationDegree));
		bullets[referenceIndexToBulletSlot] = new Bullet( spawnPoint , bulletUnitVelocity );
	}

}

bool theGame::getEmptySlotIndexFromAsteroidsArray(int &referenceIndex) {
	for(int i=0; i<MAX_ASTEROIDS; i++) {
		if(asteroids[i] == nullptr) {
			referenceIndex = i;
			return true;
		}
	}

	return false;
}

bool theGame::getEmptySlotIndexFromBulletsArray(int &referenceIndex) {
	for(int i=0; i<MAX_BULLETS; i++) {
		if(bullets[i] == nullptr) {
			referenceIndex = i;
			return true;
		}
	}

	return false;
}

void theGame::checkForCollosion() {
	// for all asteroids there, untill we find every asteroids in game
	for(int i=0; i<MAX_ASTEROIDS; i++) {

		// found an asteroid
		// check for collosion
		if(asteroids[i] != nullptr) {
			
			// with player in game
			if(shipIsDead == false) {
				if( Disc2::DoDiscsOverlap(playerSpaceShip->m_collosionDisc2 , asteroids[i]->m_collosionDisc2) == true ) {
					// Spawns smaller asteroids
					AsteroidSizes spawnAsteroidOfSize;
					bool spawnGreenLight = getSmallerSizeOfAsteroidFromReference(spawnAsteroidOfSize, asteroids[i]);
					if(spawnGreenLight == true) {
						createNewAsteroids(spawnAsteroidOfSize, 2, asteroids[i]->m_visualDisc2.center);
					}

					// delete collided asteroid
					delete asteroids[i];
					asteroids[i] = nullptr;
					asteroidsCount--;

					// hide playerSpaceShip
					shipIsDead = true;

					// reduce player lives
					remainingLivesCount--;
					delete remainingLivesIcons[remainingLivesCount];
					remainingLivesIcons[remainingLivesCount] = nullptr;

					break;
				}
			}

			// with every bullets in game
			for(int j=0; j<MAX_BULLETS; j++) {
				if(bullets[j] != nullptr) {

					if( Disc2::DoDiscsOverlap(bullets[j]->m_collosionDisc2 , asteroids[i]->m_collosionDisc2) == true ) {
						// Spawns smaller asteroids
						AsteroidSizes spawnAsteroidOfSize;
						bool spawnGreenLight = getSmallerSizeOfAsteroidFromReference(spawnAsteroidOfSize, asteroids[i]);
						if(spawnGreenLight == true) {
							createNewAsteroids(spawnAsteroidOfSize, 2, asteroids[i]->m_visualDisc2.center);
						}

						// delete collided asteroid
						delete asteroids[i];
						asteroids[i] = nullptr;
						asteroidsCount--;

						// delete bullet
						delete bullets[j];
						bullets[j] = nullptr;

						break;
					}
				}
			}
		}
	}
}

void theGame::spawnNewShip() {
	shipIsDead = false;
	if(playerSpaceShip == nullptr) {
		playerSpaceShip = new Ship();
	} else {
		*playerSpaceShip = Ship();			// reset ship
	}
	
}

void theGame::ReflectAllInputChanges() {
	// Xbox Controller Inputs
	float magnitude_xinput = g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].magnitude;
	float theta_xinput	   = g_theInput->m_controller[0].m_xboxStickStates[ XBOX_STICK_LEFT ].orientationDegree;

	if( g_theInput->WasKeyJustPressed( VK_Codes::I ) ) {
		createNewAsteroid();
	}
	if( g_theInput->WasKeyJustPressed( VK_Codes::O ) ) {
		deleteFirstAsteroid();
	}

	slowMotionON = g_theInput->IsKeyPressed( VK_Codes::T );
	if( g_theInput->WasKeyJustPressed( VK_Codes::P ) == true ) {
		gamePaused = !gamePaused;
	}
	if( g_theInput->WasKeyJustPressed( VK_Codes::F1 ) == true ) {
		debugModeON = !debugModeON;
	}

	if( g_theInput->WasKeyJustPressed( VK_Codes::N ) == true) {
		if(remainingLivesCount > 0) {
			spawnNewShip();
		}
	} else if( g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_START ].keyJustPressed ) {
		if(remainingLivesCount > 0) {
			spawnNewShip();
		}
	}

	if ( g_theInput->IsKeyPressed( VK_Codes::UP ) || g_theInput->IsKeyPressed( VK_Codes::E ) ) {
		accelerateShipMultiplier = 1.f;
		accelerateShip = true;

	} else if( magnitude_xinput > 0 ) {
		playerSpaceShip->m_orientationAngleInDegree = theta_xinput;
		accelerateShipMultiplier = magnitude_xinput;
		accelerateShip = true;

	} else { accelerateShip = false; }

	if ( g_theInput->IsKeyPressed( VK_Codes::LEFT ) || g_theInput->IsKeyPressed( VK_Codes::S ) ) {
		rotateShipAntiClockWise = true;
	} else { rotateShipAntiClockWise = false; }

	if ( g_theInput->IsKeyPressed( VK_Codes::RIGHT ) || g_theInput->IsKeyPressed( VK_Codes::F ) ) {
		rotateShipClockWise = true;
	} else { rotateShipClockWise = false; }

	shootBullet = ( g_theInput->WasKeyJustPressed( VK_Codes::SPACE ) || g_theInput->m_controller[0].m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed );
}

float theGame::CalculateDeltaTime() {
	double currentTime = GetCurrentTimeSeconds();
	float deltaSeconds = (float)(currentTime - lastFramesTime);
	lastFramesTime = currentTime;

	return deltaSeconds;
}

bool theGame::getSmallerSizeOfAsteroidFromReference(AsteroidSizes& smallerSize, const Asteroid* referenceAsteroid) const {

	if( referenceAsteroid->m_visualDisc2.radius == worldSize.x/30 ) {
		return false;

	} else if( referenceAsteroid->m_visualDisc2.radius == worldSize.x/20 ) {
		smallerSize = ASTEROID_SMALL;

	} else if( referenceAsteroid->m_visualDisc2.radius == worldSize.x/10 ) {
		smallerSize = ASTEROID_MEDIUM;

	}

	return true;
}

void theGame::DisplayRemainingPlayerLives() {
	for(int i=0; i<remainingLivesCount; i++) {
		float iconWidth = remainingLivesIcons[i]->m_visualDisc2.radius / 1.5f;

		// Set Position for icons
		remainingLivesIcons[i]->m_position = Vector2( ( iconWidth*(i+1) ), worldSize.y - iconWidth );
		remainingLivesIcons[i]->drawShipWithScale(iconWidth);
	}
}

void theGame::ResetGameAfterSeconds(float resetTimer, float deltaSeconds) {
	static float timeElasped = 0;								// to keep track of how much time passed
	timeElasped += deltaSeconds;

	if(timeElasped >= resetTimer) {								// if it is time to reset the game
		// delete all asteroids
		int asteroidsToDelete = asteroidsCount;
		for(int i=0; i<asteroidsToDelete; i++) {
			deleteFirstAsteroid();
		}

		// reset next wave to original value
		nextWaveOfAsteroids = FIRST_WAVE_ASTEROIDS_COUNT;

		// spawn next wave of asteroids
		for(int i=0; i<nextWaveOfAsteroids; i++) {
			createNewAsteroid();
		}

		// create remaining lives' ship icons
		for(int i=0; i<MAX_PLAYER_LIVES; i++) {
			remainingLivesIcons[i] = new Ship();
			remainingLivesCount++;
		}

		// spawn the ship
		spawnNewShip();

		// reset time elapsed, for next time's usability
		timeElasped = 0;
	}
}