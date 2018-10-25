#pragma once
#include <bitset>
#include "NetworkConnection.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/StringUtils.hpp"
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
	if( receivedAck != INVALID_PACKET_ACK )
	{
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
	}
	
	// Go through packet's history of ack, see if there's something that connection already received & we didn't know
	if( receivedPacketHeader.highestReceivedAck != INVALID_PACKET_ACK )
	{
		// The highest received from their side
		ConfirmPacketReceived( receivedPacketHeader.highestReceivedAck );

		// Previous 16 acks from its highest received
		std::bitset<16> acksBitField = receivedPacketHeader.receivedAcksHistory;
		for( int i = 0; i < 16; i++ )
		{
			bool isReceived = acksBitField.test( i );
			int  ackToCheck = receivedPacketHeader.highestReceivedAck - i - 1;

			// If we are about to wrap back from ZERO: skip the INVALID(0xffff) by subtracting an extra 1
			if( ackToCheck < 0 )
				ackToCheck = (uint16_t)ackToCheck - 1U;

			if( isReceived )
			{
				ConfirmPacketReceived( (uint16_t)ackToCheck );
			}
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

	if( ack == tracker.ack )
	{
		// Calculate RTT
		uint64_t rttHPC			= Clock::GetCurrentHPC() - tracker.sentTimeHPC;
		double	 secondsThisRTT	= Clock::GetSecondsFromHPC( rttHPC );
		m_rtt = (0.9f * m_rtt) + (0.1f * (float)secondsThisRTT);

		// Invalidate
		tracker.Invalidate();
	}
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
		uint16_t ackToSend = GetNextAckToSend();
		IncrementSentAck();

		// If about to wrap around, calculate the loss
		if( (ackToSend % MAX_TRACKED_PACKETS) == 0 )
			m_loss = CalculateLoss();

		// Set Ack for this packet
		thisHeader.ack						= ackToSend;
		thisHeader.highestReceivedAck		= m_highestReceivedAck;
		thisHeader.receivedAcksHistory		= m_receivedAcksBitfield;

		// Track the packet
		AddTrackedPacket( thisPacket.m_header.ack );

		// Send it
		m_parentSession.SendPacket( thisPacket );

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

float NetworkConnection::CalculateLoss() const
{
	int lostPackets = 0U;

	for( int i = 0; i < MAX_TRACKED_PACKETS; i++ )
	{
		PacketTracker const &tracker = m_packetTrackers[i];
		// If that tracker is still tracking some old packet
		if( tracker.IsValid() )
		{
			// Consider that packet as lost
			lostPackets++;
		}
	}

	float  loss = (float)lostPackets / (float)MAX_TRACKED_PACKETS;
	return loss;
}

PacketTracker* NetworkConnection::AddTrackedPacket( uint16_t ack )
{
	int index = ack % MAX_TRACKED_PACKETS;
	PacketTracker &tracker = m_packetTrackers[ index ];

	tracker.TrackForAck( ack );

	return &m_packetTrackers[ index ];
}

bool NetworkConnection::IsActivePacketTracker( uint16_t ack )
{
	// Packet Tracker
	int idx = ack % MAX_TRACKED_PACKETS;
	PacketTracker &tracker = m_packetTrackers[ idx ];

	if( tracker.ack == INVALID_PACKET_ACK )
		return false;
	else
		return true;
}

