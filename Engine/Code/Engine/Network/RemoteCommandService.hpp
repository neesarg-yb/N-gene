#pragma once
#include <vector>
#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Network/BytePacker.hpp"

enum eRCSState
{
	RCS_STATE_INITIAL = 0,
	RCS_STATE_HOSTING,
	RCS_STATE_CLIENT,
	NUM_RCS_STATES
};

class RemoteCommandService
{
public:
	 RemoteCommandService( uint16_t port = 29283 );
	~RemoteCommandService();

public:
	std::vector< TCPSocket* >	m_clientSockets;
	std::vector< BytePacker* >	m_bytePackers;
	uint16_t const				m_defaultPortToHost = 29283;
	TCPSocket*					m_hostSocket		= nullptr;
	eRCSState					m_currentState		= RCS_STATE_INITIAL;

public:
	void Update( float deltaSeconds );
	bool ConnectToHost( NetworkAddress const &hostAddress );

private:
	void Update_Initial( float deltaSeconds );		// Tries to either host local or connect to the new host
	void Update_Hosting( float deltaSeconds );		// Update as hosting state
	void Update_Client ( float deltaSeconds );		// Update as client state

private:
	float const m_retryHostingInSeconds		= 10.f;
	float		m_retryHostingTimeElapased	= 0.f;

	void		TryHosting( float deltaSeconds );
	void		ClearHostData();					// deletes host & client connection sockets..
};