#pragma once
#include <vector>
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Network/BytePacker.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"

enum eRCSState
{
	RCS_STATE_INITIAL = 0,
	RCS_STATE_HOSTING,
	RCS_STATE_CLIENT,
	NUM_RCS_STATES
};

class Camera;
class Renderer;
class BitmapFont;

class RemoteCommandService
{
public:
	 RemoteCommandService( Renderer *currentRenderer = nullptr, uint16_t port = 29283 );
	~RemoteCommandService();

public:
	std::vector< TCPSocket* >	m_clientSockets;
	std::vector< BytePacker* >	m_bytePackers;
	uint16_t const				m_defaultPortToHost = 29283;
	TCPSocket*					m_hostSocket		= nullptr;
	eRCSState					m_currentState		= RCS_STATE_INITIAL;

	Renderer*					m_theRenderer		= nullptr;
	Camera*						m_uiCamera			= nullptr;
	BitmapFont*					m_fonts				= nullptr;
	Vector2 const				m_screenBottomLeft	= Vector2( -g_aspectRatio, -1.f );
	Vector2 const				m_screenTopRight	= Vector2(  g_aspectRatio,  1.f );
	AABB2	const				m_screenBounds		= AABB2  ( m_screenBottomLeft, m_screenTopRight );
	Rgba	const				m_uiBackgroundColor = Rgba( 180, 180, 180, 100 );

public:
	void Update( float deltaSeconds );
	void Render() const;

	bool ConnectToHost( NetworkAddress const &hostAddress );
	void SendMessageToClient( uint idx, bool isEcho, char const *msg );
	void SendMessageUsingSocket( TCPSocket &endSocket, bool isEcho, char const *msg );

private:
	void Update_Initial( float deltaSeconds );		// Tries to either host local or connect to the new host
	void Update_Hosting( float deltaSeconds );		// Update as hosting state
	void Update_Client ( float deltaSeconds );		// Update as client state

private:
	float const m_retryHostingInSeconds		= 10.f;
	float		m_retryHostingTimeElapased	= 0.f;

	void		TryHosting( float deltaSeconds );
	void		ClearHostData();					// deletes host & client connection sockets..
	TCPSocket*	GetSocketAtIndex( uint idx );
	void		ServiceClientConnections();
	void		PopulateByteBufferForClient( uint clientIdx );
};