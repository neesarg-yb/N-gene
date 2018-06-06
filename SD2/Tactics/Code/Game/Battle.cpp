#pragma once
#include "Battle.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Ability.hpp"

Vector2 GetForwardStep( float x, float z );
Vector2 GetRightStep( float x, float z );

Battle::Battle()
{
	m_camera = new OrbitCamera();

	// Setup what it will draw to
	m_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() ); 

	// Setup HOW it draws
	// takes (size, near_z, far_z);
	// calculate nx, fx, ny, fy using size and the color targets aspect
	m_camera->SetProjectionOrtho( 15.0f, 0.0f, 150.0f );  
	m_camera->SetTarget( Vector3( 8.0f, 4.0f, 8.0f ) );
	m_camera->SetSphericalCoordinate( 20.f, 0.f, 60.f );
	// m_camera->LookAt( Vector3( -30.0f, 10.0f, -8.0f ), Vector3( 8.f, 2.f, 8.f ) ); 

	m_indicatorTile = new Tile( Vector3::ZERO, TileDefinition::s_tileDefinitions["MoveIndicator"] );

	m_currentMap = new Map( std::string("map0.png"), 5, BlockDefinition::s_blockDefinitions["Grass"] );
	g_currentMap = m_currentMap;

	AddNewPlayer( *ActorDefinition::s_actorDefinitions[ "Mage" ] );
	AddNewPlayer( *ActorDefinition::s_actorDefinitions[ "Archer" ] );
	AddNewPlayer( *ActorDefinition::s_actorDefinitions[ "Knight" ] );

	m_indicatorTile->m_position = m_currentMap->GiveWorldPositionToRenderOnTopOfTheBlockAt( 1, 0 );

	// Testing
	MoveIndicatorToCurrentPlayerPosition();
}

Battle::~Battle()
{
	DeleteAllPlayers();
	g_currentMap = nullptr;

	delete m_currentMap;
	delete m_indicatorTile;
	delete m_camera;
}

void Battle::AddNewPlayer( ActorDefinition& actorDefinition )
{
	int xSpawnPos = GetRandomIntInRange( 0, m_currentMap->m_dimensions.x - 1 );
	int zSpawnPos = GetRandomIntInRange( 0, m_currentMap->m_dimensions.z - 1 );

	Vector3 worldPos = m_currentMap->GiveWorldPositionToRenderOnTopOfTheBlockAt( xSpawnPos, zSpawnPos );
	Actor* newPlayer = new Actor( *m_currentMap, worldPos, &actorDefinition );
	m_players.push_back( newPlayer );

	if( m_currentPlayer == nullptr )
		m_currentPlayer = newPlayer;
}

void Battle::DeleteAllPlayers()
{
	for (int i = 0; i < m_players.size(); )
	{
		delete m_players[i];
		m_players.erase(m_players.begin() + i);
	}
}

void Battle::Update( float deltaSeconds )
{
	m_timeSinceStartOfTheBattle += deltaSeconds;

	m_currentMap->Update( deltaSeconds );
	for each (Actor* player in m_players)
			 player->Update( deltaSeconds );
	m_indicatorTile->Update( deltaSeconds );

	// Let run the due actions
	if( m_actionsQueue.size() > 0 )
	{
		bool currentActionExecFinished = m_actionsQueue[0]->IsExecutingAction_Update( deltaSeconds );
		if(  currentActionExecFinished  )
		{
			delete m_actionsQueue[0];
			m_actionsQueue[0] = nullptr;

			m_actionsQueue.erase( m_actionsQueue.begin() );
		}

		// Note: If you remove this return, actions will start running in parallel to the player's turn
		return;
	}

	// Note: If no actions are due,
	//	Let current player take its turn..
	bool		currentPlayerFinishedTurn	= false;
	Ability*	actionFromCurrentPlayer		= m_currentPlayer->IsTakingTurn_Update( deltaSeconds, currentPlayerFinishedTurn );
	
	// Player has returned a valid/self-sufficient action
	if( actionFromCurrentPlayer != nullptr )
	{
		m_actionsQueue.push_back( actionFromCurrentPlayer );

		// Proceed the game with cumulative cost time of actions
		ProceedTheGameWithWaitTimeReduction( actionFromCurrentPlayer->m_abilityData->m_cost );
	}

	// Change turn of the player
	if( currentPlayerFinishedTurn )
		CheckAndChangeThePlayerTurn();
}

void Battle::Render() const
{
	g_theRenderer->SetCurrentCameraTo( m_camera ); 
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR ); 
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
	m_currentMap->Render();

	// To Render x-ray tile	
	g_theRenderer->EnableDepth( COMPARE_GREATER, true ); 
	m_indicatorTile->RenderInNewTint( RGBA_GRAY_COLOR );
	g_theRenderer->EnableDepth( COMPARE_LESS, true ); 
	
	m_indicatorTile->Render();

	for each (Actor* player in m_players)
	{
		player->Render();
	}

	// Render currentPlayer specific UI stuffs last, b/c they can interfere with Battle's Rendering
	m_currentPlayer->IsTakingTurn_Render();
}

double Battle::GetTimeSinceBattleStarted() const
{
	return m_timeSinceStartOfTheBattle;
}

void Battle::MoveIndicatorWithLeftStick( float deltaSeconds )
{
	const float	delayBeforeNextInput	= 0.3f;

	static float totalTimeSinceLastInput = delayBeforeNextInput;
	totalTimeSinceLastInput += deltaSeconds;

	XboxController& controller	= g_theInput->m_controller[0];
	Vector2 leftStickPos		= controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	Vector2 xzIncrement			= Vector2::ZERO;

	if( abs(leftStickPos.x) > abs(leftStickPos.y) )
	{
		// Move on x-axis
		float directionFlip = leftStickPos.x < 0.f ? -1.f : 1.f;
		Vector3 camForward	= m_camera->GetForwardVector() * directionFlip;
		xzIncrement			= GetRightStep( -camForward.x, camForward.z );			TODO("Identify why this magic-minus-sign works.. For both Forward & Right!");
	}
	else if( abs(leftStickPos.x) < abs(leftStickPos.y) )
	{
		// Move on z-axis
		float directionFlip	= leftStickPos.y < 0.f ? -1.f : 1.f;
		Vector3 camForward	= m_camera->GetForwardVector() * directionFlip;
		xzIncrement			= GetForwardStep( -camForward.x, camForward.z );
	}
	else
		totalTimeSinceLastInput = delayBeforeNextInput;		// Stick is in middle, so don't delay before taking next input..

	if( totalTimeSinceLastInput > delayBeforeNextInput )
	{
		int world_x = (int)( m_indicatorTile->m_position.x + xzIncrement.x );
		int world_z = (int)( m_indicatorTile->m_position.z + xzIncrement.y );
		m_indicatorTile->m_position = m_currentMap->GiveWorldPositionToRenderOnTopOfTheBlockAt( world_x, world_z );
		totalTimeSinceLastInput = 0;
	}
}

void Battle::RotateCameraWithRightStick( float deltaSeconds )
{
	XboxController& controller	= g_theInput->m_controller[0];
	Vector2 rotAltIncrement		= controller.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition * deltaSeconds * 50.f;

	m_camera->IncrementInSphericalCoordinate( 0.f, -rotAltIncrement.x, rotAltIncrement.y );
}

void Battle::ZoomTheCameraBasedOnTriggers( float deltaSeconds )
{
	float zoomSpeed = 3.5f;		// 1 size per second
	XboxController& controller	= g_theInput->m_controller[0];
	float			zoomIn		= controller.m_xboxTriggerStates[ XBOX_TRIGGER_RIGHT ];
	float			zoomOut		= controller.m_xboxTriggerStates[ XBOX_TRIGGER_LEFT  ];
	float			resultZoom	= zoomIn - zoomOut;
	float			sizeChange	= -1.f * resultZoom * zoomSpeed * deltaSeconds;
	
	m_camera->IncrementCameraSizeBy( sizeChange );
}

void Battle::MoveCurrentPlayerOnTheIndicator()
{
	m_currentPlayer->SetPosition( m_indicatorTile->m_position );
}

void Battle::MoveIndicatorToCurrentPlayerPosition()
{
	IntVector2 currentPlayerPosXZ = IntVector2( (int) m_currentPlayer->GetPosition().x, (int) m_currentPlayer->GetPosition().z );
	m_indicatorTile->m_position = m_currentMap->GiveWorldPositionToRenderOnTopOfTheBlockAt( currentPlayerPosXZ.x, currentPlayerPosXZ.y );
}

void Battle::FocusCameraOnCurrentPlayer( float deltaSeconds )
{
	const Vector3& startPos = m_camera->m_target;
	const Vector3& endPos	= m_currentPlayer->GetPosition();
	const Vector3  lerpPos	= Interpolate( startPos, endPos, deltaSeconds );

	m_camera->SetTarget( lerpPos );
}

void Battle::FocusCameraOnIndicatorTile( float deltaSeconds )
{
	const Vector3& startPos = m_camera->m_target;
	const Vector3& endPos	= m_indicatorTile->m_position;
	const Vector3  lerpPos	= Interpolate( startPos, endPos, deltaSeconds );

	m_camera->SetTarget( lerpPos );
}

void Battle::ProceedTheGameWithWaitTimeReduction( float totalTimeReduction )
{
	for each ( Actor* player in m_players )
	{
		if( player != m_currentPlayer )									// For all other players: reduce their waitTime
			player->IncreaseWaitingTimeBy( -totalTimeReduction );
		else															// For currentPlayer:	  increase the waitTime
			player->IncreaseWaitingTimeBy(  totalTimeReduction );
	}
}

void Battle::CheckAndChangeThePlayerTurn()
{
	for each (Actor* player in m_players)
		if( player->m_waitingTime < m_currentPlayer->m_waitingTime )
			m_currentPlayer = player;
}

Vector2 GetForwardStep( float x, float z )
{
	if( abs(x) >= abs(z) )
		return x >= 0 ? Vector2( 1.f, 0.f ) : Vector2( -1.f, 0.f );
	else
		return z > 0  ? Vector2( 0.f, 1.f ) : Vector2( 0.f, -1.f );
}

Vector2 GetRightStep( float x, float z )
{
	if( abs(x) >= abs(z) )
		return x >= 0 ? Vector2( 0.f, -1.f ) : Vector2(  0.f, 1.f );
	else
		return z > 0  ? Vector2( 1.f,  0.f ) : Vector2( -1.f, 0.f );
}