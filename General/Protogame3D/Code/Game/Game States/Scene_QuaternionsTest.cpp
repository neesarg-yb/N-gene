#pragma once
#include "Scene_QuaternionsTest.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Profiler/Profiler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DebugRenderer/DebugRenderer.hpp"
#include "Game/theGame.hpp"

Scene_QuaternionsTest::Scene_QuaternionsTest( Clock const *parentClock )
	: GameState( "QUATERNIONS TEST", parentClock )
{
	// Setup the Orbit camera
	m_camera = new Camera();
	m_camera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_camera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_camera->SetPerspectiveCameraProjectionMatrix( 90.f, Window::GetInstance()->GetAspectRatio(), 0.1f, 100.f );
	m_camera->LookAt( Vector3( 0.f, 5.f, -20.f ), Vector3::ZERO );

	// Skybox
	m_camera->SetupForSkybox( "Data\\Images\\Skybox\\skybox.jpg" );
	
	QuaternionsTestCode();
}

Scene_QuaternionsTest::~Scene_QuaternionsTest()
{
	delete m_camera;
}

void Scene_QuaternionsTest::JustFinishedTransition()
{

}

void Scene_QuaternionsTest::BeginFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Debug Renderer
	DebugRendererBeginFrame( m_clock );
}

void Scene_QuaternionsTest::EndFrame()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();
}

void Scene_QuaternionsTest::Update()
{
	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	float deltaSeconds = (float) m_clock->GetFrameDeltaSeconds();

	if( m_t >= 1.f )
	{
		m_currentEulerRotation	= m_targetSlerpEulerRotation;
		m_performSlerpOperation	= false;
		m_t						= 0.f;
	}

	// Level::Update
	m_timeSinceStartOfTheBattle += deltaSeconds;

	UpdateEulerRotationAccordingToInput( deltaSeconds );

	if( g_theInput->WasKeyJustPressed( VK_Codes::ESCAPE ) )
		g_theGame->StartTransitionToState( "LEVEL SELECT" );
	if( g_theInput->WasKeyJustPressed( 'T' ) )
		StoreTheTargetEulerRotationForSlerp();
	if( g_theInput->WasKeyJustPressed( VK_Codes::ENTER ) )
		StartTheSlerp();

	// For Slerp
	if( m_performSlerpOperation )
	{
		m_t = (float)( m_timeSinceStartOfTheBattle - m_timeWhenSlerpStarted ) / m_slerpDurationSeconds;
		ClampFloat01( m_t );
	}
}

void Scene_QuaternionsTest::Render( Camera *gameCamera ) const
{
	UNUSED( gameCamera );

	// Profiler Test
	PROFILE_SCOPE_FUNCTION();

	// Light
	Light mainLight = Light( Vector3( 20.f, 20.f, -10.f ), Vector3::ZERO );
	mainLight.SetUpForPointLight( 1000.f, Vector3( 1.f, 0.f, 0.f) );

	g_theRenderer->EnableLight( 0, mainLight );
	g_theRenderer->SetAmbientLight( m_ambientLight );
	g_theRenderer->UpdateLightUBOs();

	// Camera
	g_theRenderer->BindCamera( m_camera );
	g_theRenderer->ClearColor( RGBA_BLACK_COLOR );
	g_theRenderer->ClearDepth( 1.0f ); 
	g_theRenderer->EnableDepth( COMPARE_LESS, true );

	// Pre-Rendering Effects: Skybox
	m_camera->PreRender( *g_theRenderer );
	
	// ------ //
	// Render //
	// ------ //
	Vector3 eulerBasisPos		= m_eulerBasisWorldPos;
	Vector3 quaternionBasisPos	= m_quaternionBasisWorldPos;
	if( g_theInput->IsKeyPressed( SPACE ) )
	{
		eulerBasisPos		= Vector3::ZERO;
		quaternionBasisPos	= Vector3::ZERO;
	}

	RenderMeshUsingEuler( eulerBasisPos, m_currentEulerRotation );
	RenderMeshUsingQuaternion( quaternionBasisPos, m_currentEulerRotation );
	
	// Post-Rendering Effects: Bloom?
	m_camera->PostRender( *g_theRenderer );

	// DebugText: Ambient Light
	std::string rotationInfo	= std::string( "Pitch:(W, S)  Yaw:(A, D) Roll:(Q, E)" );
	std::string compareInfo		= std::string( "Press SPACE to compare!" );
	std::string descriptionCmp	= std::string( "(Both basis gets snapped to Vector3::ZERO)" );
	std::string slerpString1	= std::string( "Slerp: Press T to set target position.." );
	std::string slerpString2	= std::string( "       & then press ENTER to slerp the quaternion to the target!" );
	DebugRender2DText( 0.f, Vector2(-850.f, 460.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, rotationInfo.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 440.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, compareInfo.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 420.f), 15.f, RGBA_GRAY_COLOR,  RGBA_GRAY_COLOR, descriptionCmp.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 400.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, slerpString1.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 380.f), 15.f, RGBA_WHITE_COLOR, RGBA_WHITE_COLOR, slerpString2.c_str() );
	DebugRender2DText( 0.f, Vector2(-850.f, 360.f), 15.f, RGBA_PURPLE_COLOR, RGBA_PURPLE_COLOR, Stringf("Rotation ( %f, %f, %f )", m_currentEulerRotation.x, m_currentEulerRotation.y, m_currentEulerRotation.z ) );

	DebugRenderTag( 0.f, 1.f, m_eulerBasisWorldPos		- Vector3( 4.f, 4.f, 0.f ), m_camera->m_cameraTransform.GetWorldTransformMatrix().GetJColumn(), m_camera->m_cameraTransform.GetTransformMatrix().GetIColumn(), RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, "EULER ANGLE");
	DebugRenderTag( 0.f, 1.f, m_quaternionBasisWorldPos - Vector3( 4.f, 4.f, 0.f ), m_camera->m_cameraTransform.GetWorldTransformMatrix().GetJColumn(), m_camera->m_cameraTransform.GetTransformMatrix().GetIColumn(), RGBA_YELLOW_COLOR, RGBA_YELLOW_COLOR, "QUATERNIONS");

	// Debug Renderer
	DebugRendererLateRender( m_camera );
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
	Quaternion	test3_Q				= Quaternion::FromEuler( eulerRotationZXY_Test3.x, eulerRotationZXY_Test3.y, eulerRotationZXY_Test3.z );
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

void Scene_QuaternionsTest::RenderMeshUsingEuler( Vector3 const &position, Vector3 const &rotationInDegrees ) const
{
	// Mesh Builder
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddCube( Vector3( 4.0f, 0.3f, 0.3f ), Vector3( 2.f, 0.f, 0.f ), RGBA_RED_COLOR );
	mb.AddCube( Vector3( 0.3f, 4.0f, 0.3f ), Vector3( 0.f, 2.f, 0.f ), RGBA_GREEN_COLOR );
	mb.AddCube( Vector3( 0.3f, 0.3f, 4.0f ), Vector3( 0.f, 0.f, 2.f ), RGBA_BLUE_COLOR );
	mb.End();

	// Mesh & Material
	Mesh		*basisMesh			= mb.ConstructMesh< Vertex_Lit >();
	Material	*defaultMaterial	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );
	
	// Model Matrix
	Matrix44	*eulerModelMatrix	= new Matrix44();
	eulerModelMatrix->Translate3D( position );
	eulerModelMatrix->RotateDegrees3D( rotationInDegrees );
	eulerModelMatrix->Scale3D( Vector3::ONE_ALL );
	
	// Slerp
	if( m_performSlerpOperation )
	{
		Matrix44 targetModelMatrix;
		targetModelMatrix.Translate3D( position );
		targetModelMatrix.RotateDegrees3D( m_targetSlerpEulerRotation );
		targetModelMatrix.Scale3D( Vector3::ONE_ALL );

		*eulerModelMatrix = Matrix44::LerpMatrix( *eulerModelMatrix, targetModelMatrix, m_t );
	}

	// Draw
	g_theRenderer->BindMaterialForShaderIndex( *defaultMaterial );
	g_theRenderer->DrawMesh( *basisMesh, *eulerModelMatrix );

	delete basisMesh;
	delete defaultMaterial;
	delete eulerModelMatrix;
}

void Scene_QuaternionsTest::RenderMeshUsingQuaternion( Vector3 const &position, Vector3 const &rotationInDegrees ) const
{
	// Mesh Builder
	MeshBuilder mb;
	mb.Begin( PRIMITIVE_TRIANGES, true );
	mb.AddCube( Vector3( 4.0f, 0.3f, 0.3f ), Vector3( 2.f, 0.f, 0.f ), RGBA_RED_COLOR );
	mb.AddCube( Vector3( 0.3f, 4.0f, 0.3f ), Vector3( 0.f, 2.f, 0.f ), RGBA_GREEN_COLOR );
	mb.AddCube( Vector3( 0.3f, 0.3f, 4.0f ), Vector3( 0.f, 0.f, 2.f ), RGBA_BLUE_COLOR );
	mb.End();

	// Mesh & Material
	Mesh		*basisMesh			= mb.ConstructMesh< Vertex_Lit >();
	Material	*defaultMaterial	= Material::CreateNewFromFile( "Data\\Materials\\default.material" );

	// Quaternions
//	Matrix44	eulerRotationMatrix = Matrix44();
//	eulerRotationMatrix.RotateDegrees3D( rotationInDegrees );
//	Quaternion	quaternionRotation	= Quaternion::FromMatrix( eulerRotationMatrix );
	Quaternion	quaternionRotation	= Quaternion::FromEuler( rotationInDegrees );
//	Matrix44	quaternionRotMatrix	= quaternionRotation.GetAsMatrix44();

	// Slerp
	if( m_performSlerpOperation )
	{
		Quaternion slerpRoation = Quaternion::Slerp( quaternionRotation, Quaternion::FromEuler( m_targetSlerpEulerRotation ),  m_t );
		quaternionRotation = slerpRoation;
	}

	// Model Matrix
	Matrix44	*eulerModelMatrix	= new Matrix44();
	eulerModelMatrix->Translate3D( position );
	eulerModelMatrix->Append( quaternionRotation.GetAsMatrix44() );
	
	eulerModelMatrix->Scale3D( Vector3::ONE_ALL );

	// Draw
	g_theRenderer->BindMaterialForShaderIndex( *defaultMaterial );
	g_theRenderer->DrawMesh( *basisMesh, *eulerModelMatrix );

	delete basisMesh;
	delete defaultMaterial;
	delete eulerModelMatrix;
}

void Scene_QuaternionsTest::UpdateEulerRotationAccordingToInput( float deltaSeconds )
{
	float rotateAroundX  = g_theInput->IsKeyPressed( 'W' ) ?  1.f : 0.f;
	rotateAroundX		+= g_theInput->IsKeyPressed( 'S' ) ? -1.f : 0.f;

	float rotateAroundY  = g_theInput->IsKeyPressed( 'A' ) ? -1.f : 0.f;
	rotateAroundY		+= g_theInput->IsKeyPressed( 'D' ) ?  1.f : 0.f;

	float rotateAroundZ  = g_theInput->IsKeyPressed( 'Q' ) ?  1.f : 0.f;
	rotateAroundZ		+= g_theInput->IsKeyPressed( 'E' ) ? -1.f : 0.f;

	m_currentEulerRotation += Vector3( rotateAroundX, rotateAroundY, rotateAroundZ ) * m_rotationSpeed * deltaSeconds;

	// Making sure that degrees is in range [ -360, 360 ]
	if( m_currentEulerRotation.x > 360.f || m_currentEulerRotation.x < -360.f )
		m_currentEulerRotation.x = fmodf( m_currentEulerRotation.x, 360.f );
	if( m_currentEulerRotation.y > 360.f || m_currentEulerRotation.y < -360.f )
		m_currentEulerRotation.y = fmodf( m_currentEulerRotation.y, 360.f );
	if( m_currentEulerRotation.z > 360.f || m_currentEulerRotation.z < -360.f )
		m_currentEulerRotation.z = fmodf( m_currentEulerRotation.z, 360.f );
}

void Scene_QuaternionsTest::StartTheSlerp()
{
	// Store time
	m_timeWhenSlerpStarted = m_timeSinceStartOfTheBattle;

	m_performSlerpOperation = true;
}

void Scene_QuaternionsTest::StoreTheTargetEulerRotationForSlerp()
{
	// Store target rotation
	m_targetSlerpEulerRotation = m_currentEulerRotation;
}
