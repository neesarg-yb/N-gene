#pragma once
#include <bitset>
#include "NetworkConnection.hpp"
#include "Engine/Core/Clock.hpp"
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

void NetworkConnection::OnReceivePacket( NetworkPacketHeader receivedPacketHeader )
{
	// Last Received Time
	m_lastReceivedTimeHPC = Clock::GetCurrentHPC();
	
	uint16_t receivedAck = receivedPacketHeader.ack;
	if( m_highestReceivedAck != INVALID_PACKET_ACK )
	{
		if( receivedAck > m_highestReceivedAck )
		{
			// Shift one bit, and mark myself as received
			m_receivedAcksBitfield <<= 1;
			m_receivedAcksBitfield |= 0x0001;

			// Shift the remaining difference
			int d = (receivedAck - m_highestReceivedAck) - 1;
			m_receivedAcksBitfield <<= d;

			m_highestReceivedAck = receivedAck;
		}
		else
		{
			// Mark bit for the receivedPacket's ack
			int s = (m_highestReceivedAck - receivedAck) - 1;
			
			uint16_t maskBit = 0x0001;
			maskBit <<= s;

			m_receivedAcksBitfield |= maskBit;
		}
	}
	else
	{
		// First ack ever received..
		// Bit field is 0U, already.

		// We just need to change highest received ack
		m_highestReceivedAck = receivedAck;
	}


	// Update received acks
	if( receivedPacketHeader.ack != INVALID_PACKET_ACK )
	{
		ConfirmPacketReceived( receivedPacketHeader.highestReceivedAck );

		std::bitset<16> acksBitField = m_receivedAcksBitfield;
		for( int i = 0; i < 16; i++ )
		{
			bool isReceived = acksBitField.test( i );
			int  ackToCheck = m_highestReceivedAck - i - 1;
			if( isReceived && ackToCheck >= 0 )
				ConfirmPacketReceived( (uint16_t)ackToCheck );
		}

	}
}

void NetworkConnection::ConfirmPacketReceived( uint16_t ack )
{
	// Packet Tracker
	int idx = ack % MAX_TRACKED_PACKETS;
	PacketTracker &tracker = m_packetTrackers[ idx ];
	
	if( tracker.ack == INVALID_PACKET_ACK )
		return;

	// Calculate RTT
	uint64_t rttHPC			= Clock::GetCurrentHPC() - tracker.sentTimeHPC;
	double	 secondsThisRTT	= Clock::GetSecondsFromHPC( rttHPC );
	m_rtt = (0.9f * m_rtt) + (0.1f * (float)secondsThisRTT);
	
	// Invalidate
	tracker.Invalidate();
}

void NetworkConnection::Send( NetworkMessage &msg )
{
	// Set the header
	std::string msgDefinitionName				= msg.m_name;
	msg.m_header.networkMessageDefinitionIndex	= (uint8_t)m_parentSession.m_registeredMessages[ msgDefinitionName ].id;

	// Push to the outgoing messages
	NetworkMessage *msgToSend = new NetworkMessage( msg );
	m_outgoingUnreliableMessages.push_back( msgToSend );
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
	
	// Populate messages into thisPacket & its header
	NetworkPacket		 thisPacket;
	NetworkPacketHeader &thisHeader		= thisPacket.m_header;
	thisHeader.messageCount				= 0x00;
	thisHeader.connectionIndex			= (uint8_t)m_indexInSession;
	thisHeader.ack						= GetNextAckToSend();
	thisHeader.highestReceivedAck		= m_highestReceivedAck;
	thisHeader.receivedAcksHistory		= m_receivedAcksBitfield;
	
	// Reliable Messages
	// ...
	
	// Unreliable Messages
	while ( m_outgoingUnreliableMessages.size() > 0 )
	{
		bool writeSuccessfull = thisPacket.WriteMessage( *m_outgoingUnreliableMessages.front() );
		if( writeSuccessfull )
		{
			// Delete the message from queue
			std::swap( m_outgoingUnreliableMessages.front(), m_outgoingUnreliableMessages.back() );
			delete m_outgoingUnreliableMessages.back();
			m_outgoingUnreliableMessages.back() = nullptr;

			m_outgoingUnreliableMessages.pop_back();
		}
		else
		{
			// That would mean that package can't store more messages
			// Unreliable messages => we don't care about leftover messages
			break;
		}
	}

	// Send if not empty
	if( thisPacket.HasMessages() )
	{
		// Track the packet
		AddTrackedPacket( thisPacket.m_header.ack );

		// Send it
		m_parentSession.SendPacket( thisPacket );
		IncrementSentAck();

		// Update Analytics
		m_lastSendTimeHPC = Clock::GetCurrentHPC();
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

uint16_t NetworkConnection::GetNextAckToSend()
{
	// If we wrapped on last increment, increment it again to make it a valid ack
	if( m_nextSentAck == INVALID_PACKET_ACK )
		m_nextSentAck++;

	return m_nextSentAck;
}

void NetworkConnection::IncrementSentAck()
{
	m_nextSentAck++;
}

PacketTracker* NetworkConnection::AddTrackedPacket( uint16_t ack )
{
	int index = ack % MAX_TRACKED_PACKETS;
	m_packetTrackers[ index ].TrackForAck( ack );

	return &m_packetTrackers[ index ];
}

