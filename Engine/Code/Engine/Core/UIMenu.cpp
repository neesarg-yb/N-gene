#pragma once
#include "UIMenu.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

UIMenu::UIMenu( InputSystem& inputSystem, Renderer& renderer, AABB2 zero_to_one_drawBounds /* = AABB2( 0.01f, 0.01f, 0.14f, 0.14f ) */ )
	: m_activeInput( inputSystem )
	, m_activeRenderer( renderer )
{
	Vector2 bounds_mins = Vector2( RangeMapFloat( zero_to_one_drawBounds.mins.x, 0.f, 1.f, -g_aspectRatio, g_aspectRatio ),
								   RangeMapFloat( zero_to_one_drawBounds.mins.y, 0.f, 1.f, -1.f, 1.f ) );
	Vector2 bounds_maxs = Vector2( RangeMapFloat( zero_to_one_drawBounds.maxs.x, 0.f, 1.f, -g_aspectRatio, g_aspectRatio ),
								   RangeMapFloat( zero_to_one_drawBounds.maxs.y, 0.f, 1.f, -1.f, 1.f ) );

	m_drawBounds = AABB2( bounds_mins, bounds_maxs );
	m_UICamera = new Camera();

	m_UICamera->SetColorTarget( Renderer::GetDefaultColorTarget() );
	m_UICamera->SetDepthStencilTarget( Renderer::GetDefaultDepthTarget() );
	m_UICamera->SetProjectionOrtho( 2.f, -1.f, 1.f );							// To set NDC styled ortho
																				//	  set it to identity
	
	m_font = m_activeRenderer.CreateOrGetBitmapFont("SquirrelFixedFont");
}

UIMenu::~UIMenu()
{
	delete m_UICamera;
}

void UIMenu::Update( float deltaSeconds )
{
	HandleXboxInput( deltaSeconds );
}

void UIMenu::Render() const
{
	m_activeRenderer.SetCurrentCameraTo( m_UICamera );
	m_activeRenderer.ClearDepth(1.f);
	m_activeRenderer.EnableDepth( COMPARE_LESS, false );

	// Draw background
	Rgba bgColorWithAlpha	 = Rgba( m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundAlpha ); 
	m_activeRenderer.DrawAABB( m_drawBounds, bgColorWithAlpha );

	int numOfActionsInMenu = (int) m_menuActions.size();
	if( numOfActionsInMenu <= 0 )
		return;

	const float	 totalHeight	= ( m_drawBounds.maxs.y - m_drawBounds.mins.y );
	const float	 totalWidth		= ( m_drawBounds.maxs.x - m_drawBounds.maxs.y );
	const float	 lineHeight		= ( totalHeight / numOfActionsInMenu );
	const float  widthPadding	= totalWidth * 0.1f;
	const float  heightPadding	= lineHeight * 0.1f;
	const AABB2	 boxForALine = AABB2( m_drawBounds.mins.x + widthPadding,  m_drawBounds.mins.y				 + heightPadding, 
									  m_drawBounds.maxs.x - widthPadding, (m_drawBounds.mins.y + lineHeight) - heightPadding);

	// Draw Menu Actions
	for( int i = 0; i < numOfActionsInMenu; i++ )
	{
		AABB2 boundsForCurrentAction = AABB2( boxForALine.mins.x, boxForALine.mins.y + (i * lineHeight), 
											  boxForALine.maxs.x, boxForALine.maxs.y + (i * lineHeight) );

		Rgba tintColor = m_textColor;
		if( i == m_selectionIndex )
			 tintColor = m_selectionColor;

		m_activeRenderer.DrawTextInBox2D( m_menuActions[i]->m_actionName, Vector2(0.5f, 0.5f), boundsForCurrentAction, lineHeight, tintColor, m_font, TEXT_DRAW_SHRINK_TO_FIT );
	}
}

void UIMenu::AddNewMenuAction(const MenuAction& newAction)
{
	bool actionExists = false;

	for( std::vector< MenuAction* >::iterator it = m_menuActions.begin(); it != m_menuActions.end(); it++ )
	{
		if( (*it)->m_actionName == newAction.m_actionName )
		{
			actionExists = true;

			delete *it;
			*it = new MenuAction( newAction );
			break;
		}
	}

	if( actionExists != true )
		m_menuActions.push_back( new MenuAction(newAction) );

	m_selectionIndex = 0;
}

void UIMenu::RemoveAMenuAction(const char* actionName)
{
	for( std::vector< MenuAction* >::iterator it = m_menuActions.begin(); it != m_menuActions.end(); it++ )
	{
		if( (*it)->m_actionName == actionName )
		{
			delete *it;
			m_menuActions.erase( it );
			break;
		}
	}
}

int UIMenu::TotalMenuActions() const
{
	return (int) m_menuActions.size();
}

void UIMenu::ChangeSelectionBy(int indexChange)
{
	int newIndex			= m_selectionIndex + indexChange;
	int totalMenuActions	= TotalMenuActions();

	if( totalMenuActions > 0 )
		m_selectionIndex = ClampInt( newIndex, 0, totalMenuActions - 1 );
}

void UIMenu::ExecuteSelectedAction()
{
	GUARANTEE_RECOVERABLE( m_selectionIndex != -1, "[ERROR] UIMenu: Selected index can't be -1!!" );

	//  It is like,
	//    (*functionPointer)( arguments );
	(*m_menuActions[ m_selectionIndex ]->m_callbackFunction) ( m_menuActions[ m_selectionIndex ]->m_actionName.c_str() );
}

void UIMenu::HandleXboxInput( float deltaSeconds )
{
	ChangeSelectionAccordingToLeftStick( deltaSeconds );
	CheckForFaceButtonPress();
}

void UIMenu::ChangeSelectionAccordingToLeftStick( float deltaSeconds )
{
	const float	delayBeforeNextInput	= 0.25f;

	static float totalTimeSinceLastInput = delayBeforeNextInput;
	totalTimeSinceLastInput += deltaSeconds;

	XboxController& controller	= m_activeInput.m_controller[0];
	Vector2 leftStickPos		= controller.m_xboxStickStates[ XBOX_STICK_LEFT ].correctedNormalizedPosition;
	int indexIncrement			= 0;

	if( abs(leftStickPos.x) > abs(leftStickPos.y) )
	{
		// x-axis
	}
	else if( abs(leftStickPos.x) < abs(leftStickPos.y) )
	{
		// y-axis
		indexIncrement	= leftStickPos.y < 0.f ? -1 : 1;
	}
	else
		totalTimeSinceLastInput = delayBeforeNextInput;		// Stick is in middle, so don't delay before taking next input..

	if( totalTimeSinceLastInput > delayBeforeNextInput )
	{
		ChangeSelectionBy( indexIncrement );
		totalTimeSinceLastInput = 0;
	}
}

void UIMenu::CheckForFaceButtonPress()
{
	XboxController& controller	= m_activeInput.m_controller[0];
	
	// A-Button pressed
	if( controller.m_xboxButtonStates[ XBOX_BUTTON_A ].keyJustPressed )
		ExecuteSelectedAction();
}