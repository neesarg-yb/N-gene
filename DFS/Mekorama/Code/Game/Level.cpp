#pragma once
#include "Level.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/File/ModelLoader.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Transform.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/HeatMap3D.hpp"
#include "Game/World/BlockDefinition.hpp"
#include "Game/World/TowerDefinition.hpp"

using namespace tinyxml2;

Level::Level( std::string definitionName, Robot &playerRobot )
	: m_pickBuffer( *g_theRenderer )
	, m_definition( *LevelDefinition::s_definitions[ definitionName ] )
	, m_playerRobot( playerRobot )
{

}

Level::~Level()
{
	delete m_tower;

	delete m_renderingPath;
	delete m_levelScene;

	delete m_sphere;
	
	DebugRendererShutdown();

	// Lights
	for( unsigned int i = 0; i < m_lightSources.size(); i++ )
		delete m_lightSources[i];
	m_lightSources.clear();

	// GameObject Pool
	for( unsigned int i = 0; i < m_allGameObjects.size(); i++ )
		delete m_allGameObjects[i];

	m_allGameObjects.clear();
	
	delete m_camera;
}

void Level::Startup()
{
	// Setup the camera
	m_camera = new OrbitCamera( Vector3::ZERO );
	// Set Color Targets
	m_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	// Projection Matrix
	m_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.5f, 500.f ); 
	// Orbit Camera
	m_camera->SetSphericalCoordinate( 15.f, -90.f, 90.f );
	// Skybox
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	m_lightSources.push_back( new Light( Vector3::ZERO ) );
	m_lightSources[0]->SetUpForPointLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	m_lightSources[0]->EnablePerimeterRotationAround( Vector3( 0.f, 0.f, 5.f ), 10.f );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, m_camera );

	// Placeholder sphere for Tank
	m_sphereMesh		= MeshBuilder::CreateSphere( 1.f, 30, 30 );
	m_sphereMaterial	= Material::CreateNewFromFile( "Data\\Materials\\stone_sphere.material" );
	m_sphere			= new Renderable( Vector3( 10.f, 3.2f, 20.f) );

	m_sphere->SetBaseMesh( m_sphereMesh );
	m_sphere->SetBaseMaterial( m_sphereMaterial );

	// Battle Scene
	m_levelScene = new Scene();

	m_levelScene->AddLight( *m_lightSources[0] );

	m_levelScene->AddRenderable( *m_sphere );
	m_levelScene->AddRenderable( *m_lightSources[0]->m_renderable );

	m_levelScene->AddCamera( *m_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );

	// Prepare the Tower
	m_tower = new Tower( Vector3::ZERO, m_definition.m_towerName );
	m_tower->SetFinishBlockAt( m_definition.m_spawnFinishAt );

	for( uint i = 0; i < m_tower->m_allBlocks.size(); i++ )
		m_levelScene->AddRenderable( *m_tower->m_allBlocks[i]->m_renderable );

	// Setup the Player Robot
	IntVector3 playerActualPos = m_definition.m_spawnPlayerOn + IntVector3::UP;		// Note: to spawn on top of that block, not at it!
	m_playerRobot.SetParentTower( *m_tower );
	m_playerRobot.m_posInTower = playerActualPos;

	m_levelScene->AddRenderable( *m_playerRobot.m_renderable );
}

void Level::BeginFrame()
{

}

void Level::EndFrame()
{

}

void Level::Update( float deltaSeconds )
{
	// Battle::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Target Block
	ChangeTargetBlockOnMouseClick();

	// Show selected block
	if( m_targetBlock != nullptr )
		m_targetBlock->ObjectSelected();

	// Tell Robot to move at target blocks
	if( m_targetBlock != nullptr )	
	{
		Block* targetBlock = m_tower->GetBlockOnTopOfMe( *m_targetBlock );
		targetBlock->ObjectSelected();
		m_playerRobot.MoveAtBlock( *targetBlock );
	}

	// Update Robot
	m_playerRobot.Update( deltaSeconds );

	// Camera Movement
	RotateTheCameraAccordingToPlayerInput( deltaSeconds );
	
	// Lights
	ChnageLightAsPerInput( deltaSeconds );
	for( unsigned int i = 0; i < m_lightSources.size(); i++ )
		m_lightSources[i]->Update( deltaSeconds );

	// Game Objects
	for each( GameObject* go in m_allGameObjects )
		go->Update( deltaSeconds );

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );
}

void Level::Render() const
{
	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", m_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
	m_renderingPath->RenderSceneForCamera( *m_camera, *m_levelScene );
	
	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) + " [ UP, DOWN ]" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);
	
	// Basis at Origin
	DebugRenderBasis( 0.f, Matrix44(), RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

	// HeatMap Debug Render
	HeatMap3D *newHeatMap = m_tower->GetNewHeatMapForTargetPosition( IntVector3( 1, 1, 0 ) );
	Matrix44 cameraTransform = m_camera->m_cameraTransform.GetWorldTransformMatrix();
	for( int z = 0; z < m_tower->m_dimensionXYZ.z; z++ )
		for( int y = 0; y < m_tower->m_dimensionXYZ.y; y++ )
			for( int x = 0; x < m_tower->m_dimensionXYZ.x; x++ )
			{
				uint	blockIdx		= m_tower->GetIndexOfBlockAt( IntVector3(x,y,z) );
				float	blockHeat		= newHeatMap->GetHeat( IntVector3(x, y, z) ); 
				Vector3 blockWorldPos	= m_tower->m_allBlocks[ blockIdx ]->m_transform.GetWorldPosition();

				if( blockHeat < newHeatMap->m_initialHeatValue )
					DebugRenderTag( 0.f, 0.2f, blockWorldPos, cameraTransform.GetJColumn(), cameraTransform.GetIColumn(), RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, Stringf( "%d", (int)blockHeat ) );
			}
	delete newHeatMap;

	DebugRendererRender();
}

double Level::GetTimeSinceBattleStarted() const
{
	return m_timeSinceStartOfTheBattle;
}

void Level::RotateTheCameraAccordingToPlayerInput( float deltaSeconds )
{
	static float const rotationSpeed = 45.f;	// Degrees per seconds
	static float const altitudeSpeed = 20.f;

	float turnLeftAxis		= 0.f;
	float altitudeUpAxis	= 0.f;

	// Theta Change
	if( g_theInput->IsKeyPressed( 'A' ) )
		turnLeftAxis += 1.f;
	if( g_theInput->IsKeyPressed( 'D' ) )
		turnLeftAxis -= 1.f;

	// Altitude Change
	if( g_theInput->IsKeyPressed( 'W' ) )
		altitudeUpAxis += 1.f;
	if( g_theInput->IsKeyPressed( 'S' ) )
		altitudeUpAxis -= 1.f;

	float cameraRadius	 = m_camera->m_radius;
	float cameraAltitude = m_camera->m_altitude;
	float cameraRotation = m_camera->m_rotation;
	
	// Set Rotation & Altitude
	cameraRotation		+= deltaSeconds * rotationSpeed * -turnLeftAxis;
	cameraAltitude		+= deltaSeconds * altitudeSpeed * -altitudeUpAxis;
	cameraAltitude		 = ClampFloat( cameraAltitude, 30.f, 30.f + 90.f );

	m_camera->SetSphericalCoordinate( cameraRadius, cameraRotation, cameraAltitude );
}

void Level::ChnageLightAsPerInput(float deltaSeconds)
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

void Level::ChangeTargetBlockOnMouseClick()
{
	// Mouse Position
	Vector2 mousClientPos = g_theInput->GetMouseClientPosition();
	std::string posString = Stringf( "Mouse Pos: ( %f, %f )", mousClientPos.x, mousClientPos.y );
	DebugRender2DText( 0.f, Vector2(-850.f, -460.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, posString.c_str() );

	// PickBuffer
	m_pickBuffer.GeneratePickBuffer( *m_camera, *m_levelScene );
	uint		pickID				= m_pickBuffer.GetPickID( mousClientPos );
	GameObject *selectedGameObject	= GameObject::GetFromPickID( pickID );
	std::string pickedObjectStr		= Stringf( "Selected PickID: %u", pickID );
	DebugRender2DText( 0.f, Vector2(-850.f, -420.f), 15.f, RGBA_GREEN_COLOR, RGBA_GREEN_COLOR, pickedObjectStr.c_str() );

	// Selected Game Object
	bool selectButtonJustPressed = g_theInput->WasKeyJustPressed( SPACE );
	if( selectedGameObject != nullptr && selectButtonJustPressed )
	{
		selectedGameObject->ObjectSelected();

		switch ( selectedGameObject->m_type )
		{
		case GAMEOBJECT_TYPE_BLOCK: {	// Change the targetBlock
			Block* selectedBlock	= (Block*) selectedGameObject;
			m_targetBlock			= selectedBlock;
			}
			break;

		case GAMEOBJECT_TYPE_ROBOT:
			break;
		
		default:
			// If TOWER or NUM_GAMEOBJECT_TYPES got selected
			GUARANTEE_RECOVERABLE( false, "Invalid GameObject got selected!" );
			break;
		}
	}

}
