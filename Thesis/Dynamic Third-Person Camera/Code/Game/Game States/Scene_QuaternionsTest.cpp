#pragma once
#include "Scene_QuaternionsTest.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

Scene_QuaternionsTest::Scene_QuaternionsTest()
	: GameState( "QUATERNIONS TEST" )
{
	// Setup the Orbit camera
	m_camera = new Camera();
	m_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_camera->SetPerspectiveCameraProjectionMatrix( 90.f, g_aspectRatio, 0.1f, 100.f );
	m_camera->LookAt( Vector3( 10.f, 10.f, -10.f ), Vector3::ZERO );

	// Skybox
//	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );

	// Setup the Lighting
	m_lightSources.push_back( new Light( Vector3( 0.f, 10.f, -10.f ), Vector3( -45.f, 0.f, 0.f ) ) );
	m_lightSources[0]->SetUpForDirectionalLight( 20.f, Vector3( 1.f, 0.f, 0.f), RGBA_WHITE_COLOR );
	m_lightSources[0]->UsesShadowMap( false );

	// Setup the DebugRenderer
	DebugRendererStartup( g_theRenderer, m_camera );

	// Battle Scene
	m_levelScene = new Scene();
	m_levelScene->AddLight( *m_lightSources[0] );
	m_levelScene->AddRenderable( *m_lightSources[0]->m_renderable );
	m_levelScene->AddCamera( *m_camera );

	m_renderingPath = new ForwardRenderingPath( *g_theRenderer );

	QuaternionsTestCode();
}

Scene_QuaternionsTest::~Scene_QuaternionsTest()
{
	delete m_renderingPath;
	delete m_levelScene;

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

void Scene_QuaternionsTest::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Scene_QuaternionsTest::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Scene_QuaternionsTest::Update( float deltaSeconds )
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Level::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	// Game Objects
	for each( GameObject* go in m_allGameObjects )
		go->Update( deltaSeconds );

	// Debug Renderer
	DebugRendererUpdate( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
}

void Scene_QuaternionsTest::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Bind all the Uniforms
	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "lit" ) );
	g_theRenderer->SetUniform( "EYE_POSITION", m_camera->GetCameraModelMatrix().GetTColumn() );

	////////////////////////////////
	// 							  //
	//  START DRAWING FROM HERE.. //
	//							  //
	////////////////////////////////
//	m_renderingPath->RenderSceneForCamera( *m_camera, *m_levelScene );

	RenderMeshUsingEuler();

	// DebugText for Lighting and Shader..
	std::string ambLightIntensity	= std::string( "Ambient Light: " + std::to_string(m_ambientLight.w) );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, ambLightIntensity);

	DebugRendererRender();
}

void Scene_QuaternionsTest::QuaternionsTestCode() const
{
	// TESTING QUATERNIONS
	// Test 1 - Simple Rotation & GetAsMatrix
	Quaternion	rotateAroundX_Q	= Quaternion( Vector3::FRONT, 45.f );
	Vector3		rotatedVec1		= rotateAroundX_Q.RotatePoint( Vector3::UP );
	Matrix44	qRotate1_M		= rotateAroundX_Q.GetAsMatrix44();
	Vector3		eulerQRotate1_M	= qRotate1_M.GetEulerRotation();

	// Test 2 - Rotate with Matrix
	Matrix44 rotateAroundX_M; 
	rotateAroundX_M.RotateDegrees3D( Vector3( 0.f, 0.f , 45.f) );
	Vector3	rotatedVec2				= rotateAroundX_M.Multiply( Vector3::UP, 0.f );
	Vector3 eulerRotateAroundX_M	= rotateAroundX_M.GetEulerRotation();

	// Test 3 - From Euler
	Vector3 eulerRotationZXY_Test3	= Vector3( 45.f, 90.f, 45.f);							// Roll counter clock wise => look up => turn right
																							// My Rotation Matrix applies y & z rotation as left-hand-rule; where quaternions applies it as right-hand-rule => Multiply with -1.f
	Quaternion	test3_Q				= Quaternion::FromEuler( eulerRotationZXY_Test3.x, -1.f * eulerRotationZXY_Test3.y, -1.f * eulerRotationZXY_Test3.z );
	Matrix44	test3_M				= Matrix44();
	test3_M.RotateDegrees3D( eulerRotationZXY_Test3 );

	Vector3 test3Vec_M	= Vector3::FRONT;
	Vector3 test3Vec_Q	= Vector3::FRONT;
	test3Vec_M			= test3_M.Multiply( test3Vec_M, 0.f );
	test3Vec_Q			= test3_Q.RotatePoint( test3Vec_Q );

	// Test 4 - From Matrix
	Matrix44	rotationMatFromEuler = test3_M;
	Quaternion	qFromMat			 = Quaternion::FromMatrix( rotationMatFromEuler );		// Is (qFromMat == test3_Q) ?? => YES!
	Vector3		sameAsTest3Vec_Q	 = Vector3::FRONT;
	sameAsTest3Vec_Q				 = qFromMat.RotatePoint( sameAsTest3Vec_Q );
}

void Scene_QuaternionsTest::RenderMeshUsingEuler() const
{
	// Get Mesh, Material & Model Matrix
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddCube( Vector3( 4.f, 1.f, 1.f ), Vector3( 2.f, 0.f, 0.f ), RGBA_RED_COLOR );
	mb.AddCube( Vector3( 1.f, 4.f, 1.f ), Vector3( 0.f, 2.f, 0.f ), RGBA_GREEN_COLOR );
	mb.AddCube( Vector3( 1.f, 1.f, 4.f ), Vector3( 0.f, 0.f, 2.f ), RGBA_BLUE_COLOR );
	mb.End();

	Mesh		*basisMesh			= mb.ConstructMesh< Vertex_Lit >();
	Material	*defaultMaterial	= Material::CreateNewFromFile( "Data\\Materials\\Block_Water.material" );
	Matrix44	*eulerModelMatrix	= new Matrix44();

	Light		mainLight = Light( Vector3( 20.f, 20.f, -10.f ), Vector3::ZERO );
	mainLight.SetUpForPointLight( 1000.f, Vector3( 1.f, 0.f, 0.f) );


	g_theRenderer->EnableLight( 0, mainLight );
	g_theRenderer->SetAmbientLight( m_ambientLight );
	g_theRenderer->UpdateLightUBOs();

	// Draw for each Shaders present in ShaderGroup
	g_theRenderer->BindCamera( m_camera/*g_theGame->m_gameCamera*/ );

	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth();
	g_theRenderer->EnableDepth( COMPARE_ALWAYS, true );

	g_theRenderer->BindMaterialForShaderIndex( *defaultMaterial );
	g_theRenderer->DrawMesh( *basisMesh, *eulerModelMatrix );


// WORKS:
//
//	g_theRenderer->UseShader( g_theRenderer->CreateOrGetShader( "default" ) );
//	g_theRenderer->EnableDepth( COMPARE_ALWAYS, true );
//	g_theRenderer->SetUniform( "EYE_POSITION", m_camera->GetCameraModelMatrix().GetTColumn() );
//	g_theRenderer->SetCurrentDiffuseTexture( defaultMaterial->m_textureBindingPairs[0] );
//	g_theRenderer->BindCamera( m_camera );
//	g_theRenderer->DrawMesh( *basisMesh, *eulerModelMatrix );

	DebugRenderLineSegment( 0.f, Vector3::ZERO, RGBA_RED_COLOR, Vector3( 4.f, 0.f, 0.f), RGBA_RED_COLOR, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, DEBUG_RENDER_IGNORE_DEPTH );

	delete basisMesh;
	delete defaultMaterial;
	delete eulerModelMatrix;
}
