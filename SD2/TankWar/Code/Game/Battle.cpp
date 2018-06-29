#pragma once
#include <utility>
#include "Battle.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Window.hpp"
#include "Game/Terrain.hpp"
#include "Game/Turret.hpp"
#include "Game/Tank.hpp"

using namespace tinyxml2;

Scene*				  Battle::s_battleScene;
Camera*				  Battle::s_camera;
std::vector< Light* > Battle::s_lightSources;


void Battle::AddNewPointLightToCamareaPosition( Rgba lightColor )
{
	Vector3 cameraPos = s_camera->m_cameraTransform.GetWorldPosition();

	Light* newLight	= new Light( cameraPos, s_camera->m_cameraTransform.GetRotation() );
	newLight->SetUpForPointLight( 40.f, Vector3( 0.f, 0.f, 1.f ), lightColor );
	s_lightSources.push_back( newLight );

	s_battleScene->AddLight( *newLight );
	s_battleScene->AddRenderable( *newLight->m_renderable );

	// DebugRenderWireSphere( 10.f, cameraPos, .5f, RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderSphere( 10.f, cameraPos, .5f, RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderQuad( 10.f, cameraPos, Vector3( 15.f, 45.f, 15.f ), Vector2::ONE_ONE, nullptr, RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderWireCube( 10.f, cameraPos - Vector3( 0.5f, 0.5f, 0.5f ), cameraPos + Vector3( 0.5f, 0.5f, 0.5f ), RGBA_WHITE_COLOR, RGBA_RED_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderBasis( 10.f, newLight->m_renderable->GetTransform().GetWorldTransformMatrix(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderLineSegment( 10.f, cameraPos - Vector3( 0.5f, 0.5f, 0.5f ), RGBA_WHITE_COLOR, cameraPos + Vector3( 0.5f, 0.5f, 0.5f ), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRenderPoint( 10.f, cameraPos - Vector3( 0.f, 0.f, -.5f ), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );
	// DebugRender2DText( 2.5f, Vector2::ZERO, 15.f, RGBA_WHITE_COLOR, RGBA_BLACK_COLOR, "Light Spawned!" );
	// DebugRender2DLine( 1.f, Vector2( -100.f, -100.f), RGBA_WHITE_COLOR, Vector2( 100.f, 100.f ), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );
	// DebugRender2DLine( 1.f, Vector2( -100.f, 100.f), RGBA_WHITE_COLOR, Vector2( 100.f, -100.f ), RGBA_BLUE_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR );
	// DebugRender2DQuad( 2.5f, AABB2( Vector2::ZERO , 10.f, 10.f), RGBA_WHITE_COLOR, RGBA_PURPLE_COLOR );
}

Battle::Battle()
{
	// Setup the UI Camera
	m_uiCamera = new Camera();
	m_uiCamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_uiCamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_uiCamera->SetProjectionOrtho( 2.f, -1.f, 1.f );							// To set NDC styled ortho

	// Fonts
	m_bitmapFonts = g_theRenderer->CreateOrGetBitmapFont("SquirrelFixedFont");
}

Battle::~Battle()
{
	delete m_bitmapFonts;
	delete m_uiCamera;

	delete m_renderingPath;
	delete s_battleScene;

	DebugRendererShutdown();

	// Lights
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		delete s_lightSources[i];

	// GameObject Pool
	for( int type = 0; type < NUM_GAME_OBJECT_TYPES; type++ )
	{
		for( unsigned int i = 0; i < m_allGameObjects[ type ].size(); i++ )
			delete m_allGameObjects[ type ][i];

		m_allGameObjects[ type ].clear();
	}

	delete s_camera;
}

void Battle::Startup()
{
	// Setup the camera
	s_camera = new Camera();
	s_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	s_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() ); 
	s_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 500.f );
	s_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	s_lightSources.push_back( new Light( Vector3( 35.f, 40.f, 20.f ), Vector3( -45.f, 0.f, 0.f ) ) );
	s_lightSources[0]->SetUpForDirectionalLight( 20.f, Vector3( 1.f, 0.f, 0.f ), RGBA_WHITE_COLOR );
	s_lightSources[0]->UsesShadowMap( true );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, s_camera );

	// Battle Scene
	s_battleScene = new Scene();

	s_battleScene->AddLight( *s_lightSources[0] );
	s_battleScene->AddRenderable( *s_lightSources[0]->m_renderable );

	s_battleScene->AddCamera( *s_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );

	// TERRAIN
	m_terrain = new Terrain( Vector3( 10.f, 20.f, 30.f ), IntVector2( 500, 400 ), 30.f, "Data\\Images\\terrain\\heightmapt.png" );
	AddNewGameObject( *m_terrain );

	AABB2	terrainsXZBounds;
	terrainsXZBounds.mins = Vector2( m_terrain->m_worldBounds.mins.x, m_terrain->m_worldBounds.mins.z );
	terrainsXZBounds.maxs = Vector2( m_terrain->m_worldBounds.maxs.x, m_terrain->m_worldBounds.maxs.z );


	// PLAYER TANK
	Vector2 middleOfTheTerrain = (terrainsXZBounds.mins + terrainsXZBounds.maxs) * 0.5f;
	m_playerTank = new Tank( middleOfTheTerrain, *m_terrain, true, s_camera );
	AddNewGameObject( *m_playerTank );

	// TESTING THE ENEMY BASE
	for( uint i = 0; i < 10; i++ )
	{
		Vector2 randPosOnTerrain;
		randPosOnTerrain.x = GetRandomFloatInRange( terrainsXZBounds.mins.x, terrainsXZBounds.maxs.x );
		randPosOnTerrain.y = GetRandomFloatInRange( terrainsXZBounds.mins.y, terrainsXZBounds.maxs.y );

		EnemyBase* testEnemyBase = new EnemyBase( randPosOnTerrain, *m_terrain, 10, 0.65f, 20.f );
		AddNewGameObject( *testEnemyBase );
	};
}

void Battle::BeginFrame()
{

}

void Battle::EndFrame()
{

}

void Battle::Update( float deltaSeconds )
{
	// Battle::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Lights
	ChnageLightAsPerInput( deltaSeconds );
	for( unsigned int i = 0; i < s_lightSources.size(); i++ )
		s_lightSources[i]->Update( deltaSeconds );

	// Cleanup the GameObjects with ZERO health
	DeleteGameObjectsWithZeroOrLessHealth();

	// Update: GameObjects
	for ( int type = 0; type < NUM_GAME_OBJECT_TYPES; type++ )				// For each type
	{
		for( uint idx = 0; idx < m_allGameObjects[ type ].size(); idx++ )	// For each game objects of that type
			m_allGameObjects[ type ][ idx ]->Update( deltaSeconds );
	}

	// Check for collision
	BulletToEnemyCollision();
	BulletToEnemyBaseCollision();
	BulletToTerrainCollision();
	EnemyToTankCollision();

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );


	// Spawn Lights according to input
	if( g_theInput->WasKeyJustPressed( 'R' ) )
		AddNewPointLightToCamareaPosition( RGBA_RED_COLOR );
	if( g_theInput->WasKeyJustPressed( 'G' ) )
		AddNewPointLightToCamareaPosition( RGBA_GREEN_COLOR );
	if( g_theInput->WasKeyJustPressed( 'B' ) )
		AddNewPointLightToCamareaPosition( RGBA_BLUE_COLOR );
	if( g_theInput->WasKeyJustPressed( 'W' ) )
		AddNewPointLightToCamareaPosition( RGBA_WHITE_COLOR );

}

void Battle::Render() const
{
	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", s_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
	m_renderingPath->RenderSceneForCamera( *s_camera, *s_battleScene );
	RenderUI();

	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) + " [ UP, DOWN ]" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);
	std::string toSpawnSpotLights	= std::string( "Spawn new SpotLights," );
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, toSpawnSpotLights);
	toSpawnSpotLights	= std::string( "Keys: R(red), G(green), B(blue), W(white)" );
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_GRAY_COLOR, RGBA_GRAY_COLOR, toSpawnSpotLights);

	DebugRendererRender();
}

void Battle::RenderUI() const
{
	g_theRenderer->BindCamera( m_uiCamera );
	g_theRenderer->UseShader( nullptr );

	g_theRenderer->EnableDepth( COMPARE_ALWAYS, false );

	// Background
	AABB2		bcBounds		= AABB2( g_aspectRatio - 0.55f, 0.78f, g_aspectRatio - 0.01f, 0.97f );
	g_theRenderer->DrawAABB( bcBounds, RGBA_GRAY_COLOR );
	
	// Health
	std::string healthStr		= Stringf( "Health:  %d", (int)m_playerTank->m_health );
	AABB2		healthBounds	= AABB2( g_aspectRatio - 0.5f, 0.9, g_aspectRatio - 0.1f, 1.f );
	g_theRenderer->DrawTextInBox2D( healthStr.c_str(), Vector2(0.f, 0.f), healthBounds, 0.04f, RGBA_RED_COLOR, m_bitmapFonts, TEXT_DRAW_OVERRUN );
	
	// Enemies
	std::string enemiesStr		= Stringf( "Enemies: %d", (int)m_allGameObjects[ GAME_OBJECT_ENEMY ].size() );
	AABB2		enemiesBounds	= AABB2( healthBounds.mins.x, healthBounds.mins.y - 0.05f, healthBounds.maxs.x, healthBounds.maxs.y - 0.05f );
	g_theRenderer->DrawTextInBox2D( enemiesStr.c_str(), Vector2(0.f, 0.f), enemiesBounds, 0.04f, RGBA_RED_COLOR, m_bitmapFonts, TEXT_DRAW_OVERRUN );

	// Enemy Bases
	std::string basesStr		= Stringf( "Bases:   %d", (int)m_allGameObjects[ GAME_OBJECT_ENEMY_BASE ].size() );
	AABB2		basesBounds		= AABB2( enemiesBounds.mins.x, enemiesBounds.mins.y - 0.05f, enemiesBounds.maxs.x, enemiesBounds.maxs.y - 0.05f );
	g_theRenderer->DrawTextInBox2D( basesStr.c_str(), Vector2(0.f, 0.f), basesBounds, 0.04f, RGBA_RED_COLOR, m_bitmapFonts, TEXT_DRAW_OVERRUN );
}

void Battle::AddNewGameObject( GameObject &newGO )
{
	// Add GameObject to pool
	m_allGameObjects[ newGO.m_type ].push_back( &newGO );

	// Add its Renderable to scene
	newGO.AddRenderablesToScene( *s_battleScene );
}

void Battle::DeleteGameObjectsWithZeroOrLessHealth()
{
	// For each types of GameObjects
	for( uint t = 0; t < NUM_GAME_OBJECT_TYPES; t++ )
	{
		// For each GameObjects of that type
		for( uint g = 0; g < m_allGameObjects[t].size(); g++ )
		{
			// If health is not low, go for next
			if( m_allGameObjects[t][g]->m_health > 0.f )
				continue;

			uint lastIndex = (uint) m_allGameObjects[t].size() - 1U;

			// swap it with the last one
			std::swap( m_allGameObjects[t][g], m_allGameObjects[t][lastIndex] );

			// Remove its renderable
			m_allGameObjects[t][lastIndex]->RemoveRenderablesFromScene( *s_battleScene );

			// delete it
			delete m_allGameObjects[t][lastIndex];

			// pop back
			m_allGameObjects[t].pop_back();

			// decrement the index b/c we just popped back one game object
			g--;
		}
	}
}

void Battle::BulletToEnemyCollision()
{
	// Sphere to sphere collision
	BulletList &bullets = * (BulletList*)	( &m_allGameObjects[ GAME_OBJECT_BULLET ] );
	EnemyList &enemies	= * (EnemyList*)	( &m_allGameObjects[ GAME_OBJECT_ENEMY ] );
	
	// For each bullets
	for( uint b = 0; b < bullets.size(); b++ )
	{
		// For each enemies
		for( uint e = 0; e < enemies.size(); e++ )
		{
			// distance between two
			float dist			= ( bullets[b]->m_transform.GetWorldPosition() - enemies[e]->m_transform.GetWorldPosition() ).GetLength();
			float sumOfRadius	= bullets[b]->m_radius + enemies[e]->m_radius;

			// collision - both should die!
			if( dist <= sumOfRadius )
			{
				bullets[b]->m_health = 0.f;
				enemies[e]->m_health = 0.f;
			}
		}
	}
}

void Battle::BulletToEnemyBaseCollision()
{
	// If a point is inside AABB3
	BulletList		&bullets = * (BulletList*)		( &m_allGameObjects[ GAME_OBJECT_BULLET ] );
	EnemyBaseList	&eBases	 = * (EnemyBaseList*)	( &m_allGameObjects[ GAME_OBJECT_ENEMY_BASE ] );

	// For each enemy bases
	for( uint e = 0; e < eBases.size(); e++ )
	{
		// Get AABB3 bounds of the base
		AABB3 eBaseBounds = eBases[e]->m_worldBounds;

		// For each bullets
		for( uint b = 0; b < bullets.size(); b++ )
		{
			// If bullet is inside the AABB3
			bool collisionHappened = eBaseBounds.IsPointInsideMe( bullets[b]->m_transform.GetWorldPosition() );

			if( collisionHappened )
			{
				// Reduce one health
				eBases[e]->m_health		-= 1.f;
				bullets[b]->m_health	-= 1.f;
			}
		}
	}
}

void Battle::BulletToTerrainCollision()
{
	BulletList &bullets = * (BulletList*) (&m_allGameObjects[ GAME_OBJECT_BULLET ]);

	// For each bullets
	for( uint b = 0; b < bullets.size(); b++ )
	{
		Vector3 bulletPos		= bullets[b]->m_transform.GetWorldPosition();
		float yCoordOfBullet	= bulletPos.y;
		float yCoordOfTerrain	= m_terrain->GetYCoordinateForMyPositionAt( bulletPos.x, bulletPos.z );

		// If bullet is under terrain
		if( yCoordOfBullet <= yCoordOfTerrain )
			bullets[b]->m_health = 0.f;
	}
}

void Battle::EnemyToTankCollision()
{
	EnemyList &enemies = * (EnemyList*) (&m_allGameObjects[ GAME_OBJECT_ENEMY ] );

	// For each enemies
	for( uint e = 0; e < enemies.size(); e++ )
	{
		Vector3 enemyPos	= enemies[e]->m_transform.GetWorldPosition();
		float	dist		= ( enemyPos - m_playerTank->m_transform.GetWorldPosition() ).GetLength();
		float	sumOfRadius	= enemies[e]->m_radius + 2.f;

		// Reduce health on collision
		if( dist < sumOfRadius )
		{
			// m_playerTank->m_health	-= 1.f;
			enemies[e]->m_health	-= 1.f;
		}
	}
}

double Battle::GetTimeSinceBattleStarted() const
{
	return m_timeSinceStartOfTheBattle;
}

void Battle::RotateTheCameraAccordingToPlayerInput( float deltaSeconds )
{
	static float const rotationSpeed = 45.f;	// Degrees per seconds

	XboxController &inputController	= g_theInput->m_controller[0];
	Vector2 axisChange				= inputController.m_xboxStickStates[ XBOX_STICK_RIGHT ].correctedNormalizedPosition;
	Vector2 finalYXEulerRotation	= axisChange * rotationSpeed * deltaSeconds;

	TODO("I'm doing Anti-Clockwise rotation, under the hood. But it seems the camera is doing Clockwise rotation.. :/");
	s_camera->RotateCameraBy( Vector3( -finalYXEulerRotation.y, finalYXEulerRotation.x, 0.f ) );
}

void Battle::ChnageLightAsPerInput(float deltaSeconds)
{
	// Ambient Light
	static float const ambientFactorChangeSpeed = 0.35f;		// Per seconds

	if (g_theInput->IsKeyPressed(UP))
	{
		m_ambientLight.w += ambientFactorChangeSpeed * deltaSeconds;
		m_ambientLight.w  = ClampFloat01( m_ambientLight.w );
	}
	if (g_theInput->IsKeyPressed(DOWN))
	{
		m_ambientLight.w -= ambientFactorChangeSpeed * deltaSeconds;
		m_ambientLight.w  = ClampFloat01( m_ambientLight.w );
	}

	g_theRenderer->SetAmbientLight( m_ambientLight );
}