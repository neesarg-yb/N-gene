#pragma once
#include "NetworkConnection.hpp"
#include "Engine/NetworkSession/NetworkSession.hpp"
#include "Engine/NetworkSession/NetworkPacket.hpp"

NetworkConnection::NetworkConnection( int idx, NetworkAddress &addr, NetworkSession &parentSession )
	: m_indexInSession( idx )
	, m_parentSession( parentSession )
	, m_address( addr )
	, m_sendRateTimer( GetMasterClock() )
	, m_heartbeatTimer( GetMasterClock() )
{
	SetSendFrequencyTo( m_sendFrequency );
	UpdateHeartbeatTimer();
}

NetworkConnection::~NetworkConnection()
{

}

void NetworkConnection::Send( NetworkMessage &msg )
{
	// Set the header
	std::string msgDefinitionName				= msg.m_name;
	msg.m_header.networkMessageDefinitionIndex	= (uint8_t)m_parentSession.m_registeredMessages[ msgDefinitionName ].id;

	// Push to the outgoing messages
	NetworkMessage *msgToSend = new NetworkMessage( msg );
	m_outgoingMessages.push_back( msgToSend );
}

void NetworkConnection::FlushMessages()
{
	// If it hasn't been time to send, return
	if( m_sendRateTimer.CheckAndReset() != true )
		return;

	// If we need a heartbeat to be sent
	if( m_heartbeatTimer.CheckAndReset() == true )
	{
		NetworkMessage heartbeat( "heartbeat" );
		Send( heartbeat );
	}

	// Batch all the messages into packets
	NetworkPacketList packetsToSend;

	NetworkPacket *thisPacket = nullptr;
	while ( m_outgoingMessages.size() > 0 )
	{
		// If we have messages to send, but thisPacket is nullptr
		if( thisPacket == nullptr )
			thisPacket = new NetworkPacket( (uint8_t)m_indexInSession );					// Create a new packet

		bool writeSuccessfull = thisPacket->WriteMessage( *m_outgoingMessages.front() );
		if( writeSuccessfull )
		{
			// Delete the message from queue
			std::swap( m_outgoingMessages.front(), m_outgoingMessages.back() );
			delete m_outgoingMessages.back();
			m_outgoingMessages.back() = nullptr;

			m_outgoingMessages.pop_back();
		}
		else
		{
			// That would mean that package can't store more messages
			//		Add thisPacket to packetsToSend & Start using a new thisPacket
			packetsToSend.push_back( thisPacket );
			thisPacket = nullptr;															// Set this packet to nullptr, its work is over
		}
	}

	// If there's any last packet left, add it to sendList
	if( thisPacket != nullptr )
	{
		packetsToSend.push_back( thisPacket );
		thisPacket = nullptr;
	}

	// Send all the packets
	while ( packetsToSend.size() > 0 )
	{
		// Send it
		m_parentSession.SendPacket( *packetsToSend.front() );

		// Fast delete the packet
		std::swap( packetsToSend.front(), packetsToSend.back() );
		delete packetsToSend.back();
		packetsToSend.back() = nullptr;

		packetsToSend.pop_back();
	}
}

uint8_t NetworkConnection::GetCurrentSendFrequency() const
{
	// Get min frequency between mine & parent's
	uint8_t parentFrequency	= m_parentSession.GetSimulatedSendFrequency();
	uint8_t minFrequency	= ( m_sendFrequency < parentFrequency ) ? m_sendFrequency : parentFrequency;

	return minFrequency;
}

void NetworkConnection::SetSendFrequencyTo( uint8_t frequencyHz )
{
	// Change local variable
	m_sendFrequency = frequencyHz;

	uint8_t sendFrequency = GetCurrentSendFrequency();

	// Frequency can't be ZERO
	GUARANTEE_RECOVERABLE( sendFrequency != 0, "NetworkConnection: Send Frequency can't be ZERO!!" );

	// Set the timer
	double intervalInSeconds = 1.0 / sendFrequency;
	m_sendRateTimer.SetTimer( intervalInSeconds );
}

void NetworkConnection::UpdateHeartbeatTimer()
{
	double intervalInSeconds = 1.0 / m_parentSession.GetHeartbeatFrequency();
	m_heartbeatTimer.SetTimer( intervalInSeconds );
}
