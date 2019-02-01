#pragma once
#include "Engine/Math/Complex.hpp"
#include "Game/Game States/GameState.hpp"

class Scene_ComplexRotation : public GameState
{
public:
	 Scene_ComplexRotation( Clock *parentClock );
	~Scene_ComplexRotation();

private:
	float	m_rotationSpeed		= 45.f;				// Degrees per seconds
	Complex	m_currentRotation	= Complex( 0.f );	// Zero degrees
	Complex	m_targetRotation	= Complex( 180.f );	// 180  degrees

	bool	m_chasingTheTarget	= false;

public:
	void JustFinishedTransition();

	void BeginFrame();
	void EndFrame();

	void Update();
	void Render( Camera *gameCamera ) const;
};
