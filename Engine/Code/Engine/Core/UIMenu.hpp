#pragma once
#include <vector>
#include "Engine/Core/MenuAction.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/InputSystem.hpp"

class UIMenu
{
public:
	 UIMenu( InputSystem& inputSystem, Renderer& renderer, AABB2 zero_to_one_drawBounds = AABB2( 0.01f, 0.01f, 0.14f, 0.14f ) );
	~UIMenu();

	// References from theGame
	InputSystem&					m_activeInput;
	Renderer&						m_activeRenderer;

	// Data
	std::vector< MenuAction* >		m_menuActions;
	int								m_selectionIndex = -1;
	
	// Maintenance
	void Update( float deltaSeconds );
	void Render() const;
	
	void AddNewMenuAction( const MenuAction& newAction );
	void RemoveAMenuAction( const char* actionName );
	int	 TotalMenuActions() const;

private:
	// Representation
	Camera*			m_UICamera;			// Just like a NDC space
	BitmapFont*		m_font;
	const Rgba&		m_textColor			= RGBA_WHITE_COLOR;
	const Rgba&		m_selectionColor	= RGBA_GREEN_COLOR;
	const Rgba&		m_backgroundColor	= RGBA_GRAY_COLOR;
	unsigned char	m_backgroundAlpha	= 128;
	AABB2			m_drawBounds;

	// Helper functions
	void	ChangeSelectionBy( int indexChange );
	void	ExecuteSelectedAction();
	void	HandleXboxInput( float deltaSeconds );
	void	ChangeSelectionAccordingToLeftStick( float deltaSeconds );
	void	CheckForFaceButtonPress();
};