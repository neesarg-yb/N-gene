#pragma once
#include <vector>
#include <functional>
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Network/BytePacker.hpp"

#define RCS_DEFAULT_HOST_PORT (29283)

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
	 RemoteCommandService( Renderer *currentRenderer = nullptr, uint16_t port = RCS_DEFAULT_HOST_PORT );
	~RemoteCommandService();

public:
	std::vector< TCPSocket* >	m_connectionSockets;
	std::vector< BytePacker* >	m_bytePackers;
	NetworkAddress				m_doHostingAtAddress;
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

	void HostAtPort( uint16_t port = RCS_DEFAULT_HOST_PORT );
	void ConnectToNewHost( const char *hostAddress );
	void SendMessageToConnection( uint idx, bool isEcho, char const *msg );
	void SendMessageToAllConnections( bool isEcho, const char *msg, bool sendToMyself = false );
	void SendMessageUsingSocket( TCPSocket &endSocket, bool isEcho, char const *msg );

private:
	void Update_Initial( float deltaSeconds );		// Tries to either host local or connect to the new host
	void Update_Hosting( float deltaSeconds );		// Update as hosting state
	void Update_Client ( float deltaSeconds );		// Update as client state

private:
	float const m_retryHostingInSeconds		= 10.f;
	float		m_retryHostingTimeElapased	= 0.f;

	void		TryHosting( float deltaSeconds );
	bool		ConnectToHost( NetworkAddress const &hostAddress );
	void		ResetConnectionsAndHost();			// Resets: Connections, Host & State
	TCPSocket*	GetSocketAtIndex( uint idx );
	void		ServiceConnections();
	void		PopulateByteBufferForConnection( uint connectionIdx );

	void		ProcessEcho( char const *message );
	void		ProcessCommandForConnection( char const *command, uint connectionId );
	void		SendEchoToConnection( char const *message );

private:
	// Helpers to respond with echo
	uint								m_sendEchoToIdx	= 0;
	std::function<void( const char* )>	m_echoMethod	= std::bind( &RemoteCommandService::SendEchoToConnection, this, std::placeholders::_1 );
};