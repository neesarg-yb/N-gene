#include "Game/Asteroid.hpp"
#include "Game/Ship.hpp"
#include "Game/Polygon2.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/GameCommon.hpp"


const int MAX_ASTEROIDS = 1000;
const int MAX_BULLETS = 500;
const int MAX_PLAYER_LIVES = 3;
const int FIRST_WAVE_ASTEROIDS_COUNT = 4;

enum AsteroidSizes {
	ASTEROID_SMALL = 0,
	ASTEROID_MEDIUM = 1,
	ASTEROID_LARGE = 2,
	ASTEROID_SIZE_NUM_MAX = 3
};

class theGame
{
public:

	const Vector2 worldSize = Vector2(1000, 1000);

	bool slowMotionON = false;
	bool gamePaused = false;
	bool accelerateShip = false;
	float accelerateShipMultiplier = 1.f;
	bool rotateShipAntiClockWise = false;
	bool rotateShipClockWise = false;
	bool debugModeON = false;
	bool shootBullet = false;
	bool shipIsDead = false;

	theGame();
	~theGame();

	void Update();
	void Render();
	void spawnNewShip();
	void createNewAsteroid();
	void createNewAsteroids(AsteroidSizes theSize, int totalNew, Vector2 position);
	void deleteFirstAsteroid();
	void shootNewBullet();
	bool getEmptySlotIndexFromAsteroidsArray(int &referenceIndex);			// return false if no empty slots found
	bool getEmptySlotIndexFromBulletsArray(int &referenceIndex);
	

private:
	Asteroid* asteroids[MAX_ASTEROIDS] = {nullptr};
	Bullet* bullets[MAX_BULLETS] = {nullptr};
	Ship* playerSpaceShip;
	Ship* remainingLivesIcons[MAX_PLAYER_LIVES] = {nullptr};
	int remainingLivesCount = MAX_PLAYER_LIVES;
	int asteroidsCount = 0;													// keeps track of asteroids in the game, right now
	int nextWaveOfAsteroids = FIRST_WAVE_ASTEROIDS_COUNT;											// these many asteroids will spawn in next wave
	double lastFramesTime;													// to help CalculateDeltaTime function
	const float asteroidSizes[3] = { worldSize.x/30,						// There are three different sizes of asteroids in this game
									 worldSize.x/20, 
									 worldSize.x/10 };
	
	float CalculateDeltaTime();
	void checkForCollosion();
	void ReflectAllInputChanges();
	bool getSmallerSizeOfAsteroidFromReference(AsteroidSizes& smallerSize, const Asteroid* referenceAsteroid) const;	// To determine "Which size of asteroids to spawn when current one breaks"
	void DisplayRemainingPlayerLives();										// Displays icons of remainingPlayerLives
	void ResetGameAfterSeconds(float resetTimer, float deltaSeconds);		// Resets game after resetTimer seconds

};

