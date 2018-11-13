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
	, m_confirmReliablesTimer( GetMasterClock() )
{
	SetSendFrequencyTo( m_sendFrequency );
	UpdateHeartbeatTimer();

	m_confirmReliablesTimer.SetTimer( 0.1 );
}

NetworkConnection::~NetworkConnection()
{
	
}

void NetworkConnection::OnReceivePacket( NetworkPacketHeader receivedPacketHeader )
{
	// We need to send back updated acks, asap
	m_immediatlyRespondForAck = true;

	// Last Received Time
	m_lastReceivedTimeHPC = Clock::GetCurrentHPC();
	
	// Update Bit Field - Received Acks
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

		UpdateHigestConfirmedReliableID( tracker );

		// Invalidate
		tracker.Invalidate();
	}
}

void NetworkConnection::ProcessReceivedMessage( NetworkMessage &receivedMessage, NetworkSender sender )
{
	if( receivedMessage.IsReliable() == false )
		receivedMessage.GetDefinition()->callback( receivedMessage, sender );		// Just do the call back, msg is not reliable thus we don't need to check anything
	else
	{
		uint16_t reliableID = receivedMessage.m_header.reliableID;
		if( HasReceivedReliableID( reliableID ) == true )
			return;
		else
		{
			MarkReliableReceived( reliableID );

			// Do the callback
			receivedMessage.GetDefinition()->callback( receivedMessage, sender );
		}
	}
}

bool NetworkConnection::CanSendReliableID( uint16_t reliableID )
{
	// No unconfirmed messages
	if( m_unconfirmedSentReliables.size() == 0 )
		return true;

	uint16_t oldestUnconfirmedReliableID = 0xffff;
	for each (NetworkMessage* unconfirmedMsg in m_unconfirmedSentReliables)
	{
		uint16_t unconfirmedID = unconfirmedMsg->m_header.reliableID;

		if( unconfirmedID < oldestUnconfirmedReliableID )
			oldestUnconfirmedReliableID = unconfirmedID;
	}

	if( reliableID < oldestUnconfirmedReliableID + RELIABLE_MESSAGES_WINDOW )
		return true;
	else
		return false;
}

bool NetworkConnection::HasNewMessagesToSend() const
{
	return (m_outgoingReliables.size() > 0) || (m_outgoingUnreliables.size() > 0);
}

void NetworkConnection::Send( NetworkMessage &msg )
{
	// Update the index of this messageToSend
	NetworkMessageDefinition const *msgDef = m_parentSession.GetRegisteredMessageDefination( msg.m_name );
	msg.SetDefinition( msgDef );
	
	// Push to the outgoing messages
	NetworkMessage *msgToSend = new NetworkMessage( msg );

	if( msgToSend->IsReliable() )
		m_outgoingReliables.push_back( msgToSend );
	else
		m_outgoingUnreliables.push_back( msgToSend );
}

void NetworkConnection::FlushMessages()
{
	// If it hasn't been time to send, return
	if( m_sendRateTimer.CheckAndReset() != true )
		return;

	// If we need a heartbeat to be sent
	if( m_heartbeatTimer.CheckAndReset() == true )
	{
		m_immediatlyRespondForAck = false;

		NetworkMessage heartbeat( "heartbeat" );
		Send( heartbeat );
	}
	
	// To return, if there's nothing to send
	bool shouldResendUnconfirmedReliables = ShouldResendUnconfirmedReliables();
	if( HasNewMessagesToSend() == false && shouldResendUnconfirmedReliables == false )
	{
		// But if we gotta inform the connection about last received packet, do it!
		if( m_immediatlyRespondForAck == true )
		{
			m_immediatlyRespondForAck = false;

			// Prepare the packet header
			NetworkPacket packetJustForAck;
			NetworkPacketHeader &theHeader	= packetJustForAck.m_header;
			theHeader.messageCount			= 0x00;
			theHeader.connectionIndex		= (uint8_t)m_indexInSession;
			theHeader.ack					= INVALID_PACKET_ACK;
			theHeader.highestReceivedAck	= m_highestReceivedAck;
			theHeader.receivedAcksHistory	= m_receivedAcksBitfield;

			// Send it
			m_parentSession.SendPacket( packetJustForAck );

			// Update Analytics
			m_lastSendTimeHPC = Clock::GetCurrentHPC();
		}

		return;
	}

	// Populate messages into thisPacket & its header
	NetworkPacket		 thisPacket;
	NetworkPacketHeader &thisHeader	= thisPacket.m_header;
	thisHeader.messageCount			= 0x00;
	thisHeader.connectionIndex		= (uint8_t)m_indexInSession;

	// Ack
	uint16_t ackToSend = GetNextAckToSend();
	IncrementSentAck();
	thisHeader.ack					= ackToSend;
	thisHeader.highestReceivedAck	= m_highestReceivedAck;
	thisHeader.receivedAcksHistory	= m_receivedAcksBitfield;

	// If about to wrap around, calculate the loss
	if( (ackToSend % MAX_TRACKED_PACKETS) == 0 )
		m_loss = CalculateLoss();

	// Track the packet
	int reliableMessagesInThisPacker = 0;
	PacketTracker *packetTracker = AddTrackedPacket( thisPacket.m_header.ack );


	// Unconfirmed Reliable Messages
	// ...
	if( shouldResendUnconfirmedReliables )
	{
		for( int ucrID = 0; ucrID < m_unconfirmedSentReliables.size(); ucrID++ )
		{
			// Don't write more than MAX reliables in this packet
			if( reliableMessagesInThisPacker >= MAX_RELIABLES_PER_PACKET )
				break;

			bool writeSuccessfull = thisPacket.WriteMessage( *m_unconfirmedSentReliables[ucrID] );
			if( writeSuccessfull )
			{
				reliableMessagesInThisPacker++;
				bool reliableIDAdded = packetTracker->AddNewReliableID( m_unconfirmedSentReliables[ucrID]->m_header.reliableID );
				GUARANTEE_RECOVERABLE( reliableIDAdded, "Error: Couldn't add new reliable ID to PacketTracker!!" );
				// ConsolePrintf( RGBA_GRAY_COLOR, "Resending message with ReliableID %d", m_unconfirmedSentReliables[ucrID]->m_header.reliableID );
			}
			else
			{
				GUARANTEE_RECOVERABLE( false, "Error: Ancountered a message which might be too large to fit in current packet?!" );
				continue;		// This message might be too big to fit?
			}
		}
	}
	

	// Reliable Messages
	// ...
	while( m_outgoingReliables.size() > 0 && reliableMessagesInThisPacker < MAX_RELIABLES_PER_PACKET )
	{
		// Give outgoing message proper reliable ID
		uint16_t reliableIDToSend = GetNextReliableIDToSend();

		if( CanSendReliableID( reliableIDToSend ) == false )
			break;

		m_outgoingReliables.front()->m_header.reliableID = reliableIDToSend;

		bool writeSuccessfull = thisPacket.WriteMessage( *m_outgoingReliables.front() );
		if( writeSuccessfull )
		{
			reliableMessagesInThisPacker++;
			bool added = packetTracker->AddNewReliableID( reliableIDToSend );
			GUARANTEE_RECOVERABLE( added, "Error: Couldn't add new reliable ID to PacketTracker?!" );
			ConsolePrintf( RGBA_KHAKI_COLOR, "Sending message with ReliableID %d, added = %s", reliableIDToSend, added ? "YES" : "NO" );

			// Move the message to sent-reliables queue
			std::swap( m_outgoingReliables.front(), m_outgoingReliables.back() );
			m_unconfirmedSentReliables.push_back( m_outgoingReliables.back() );
			m_outgoingReliables.pop_back();

			IncrementReliableIDToSend();
		}
		else
		{
			GUARANTEE_RECOVERABLE( false, "Error: Ancountered a message which might be too large to fit in current packet?!" );
			break;
		}
	}
	

	// Unreliable Messages
	// ...
	while ( m_outgoingUnreliables.size() > 0 )
	{
		bool writeSuccessfull = thisPacket.WriteMessage( *m_outgoingUnreliables.front() );
		if( writeSuccessfull )
		{
			// Delete the message from queue
			std::swap( m_outgoingUnreliables.front(), m_outgoingUnreliables.back() );
			delete m_outgoingUnreliables.back();
			m_outgoingUnreliables.back() = nullptr;

			m_outgoingUnreliables.pop_back();
		}
		else
		{
			// That would mean that package can't store more messages
			// Unreliable messages => we don't care about leftover messages
			EmptyTheOutgoingUnreliables();
			break;
		}
	}

	// Send it
	m_parentSession.SendPacket( thisPacket );

	// Update Analytics
	m_lastSendTimeHPC = Clock::GetCurrentHPC();
}

uint16_t NetworkConnection::GetLowestReliableIDToConfirm() const
{
	return (m_highestConfirmedReliableID - RELIABLE_MESSAGES_WINDOW);
}

uint16_t NetworkConnection::GetHighestConfirmedReliableID() const
{
	return m_highestConfirmedReliableID;
}

bool NetworkConnection::HasReceivedReliableID( uint16_t reliableID ) const
{
	if( CycleLess( reliableID, GetLowestReliableIDToConfirm() ) )
		return true;
	else
	{
		// If in the received list
		for( uint i = 0; i < m_receivedReliableIDs.size(); i++ )
		{
			if( m_receivedReliableIDs[i] == reliableID )
				return true;
		}

		return false;
	}
}

uint NetworkConnection::GetUnconfirmedSendReliablesCount() const
{
	return (uint)m_unconfirmedSentReliables.size();
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

void NetworkConnection::EmptyTheOutgoingUnreliables()
{
	while ( m_outgoingUnreliables.size() > 0 )
	{
		// fast delete
		std::swap( m_outgoingUnreliables.front(), m_outgoingUnreliables.back() );
		delete m_outgoingUnreliables.back();
		m_outgoingUnreliables.back() = nullptr;

		m_outgoingUnreliables.pop_back();
	}
}

uint16_t NetworkConnection::GetNextReliableIDToSend()
{
	return m_nextReliableIDToSend;
}

void NetworkConnection::IncrementReliableIDToSend()
{
	m_nextReliableIDToSend++;
}

bool NetworkConnection::ShouldResendUnconfirmedReliables()
{
	bool timeToResendReliables	= m_confirmReliablesTimer.CheckAndReset();
	bool hasReliablesToResend	= (m_unconfirmedSentReliables.size() > 0);

	return (timeToResendReliables == true) && (hasReliablesToResend == true);
}

void NetworkConnection::UpdateHigestConfirmedReliableID( PacketTracker &tracker )
{
	for( int i = 0; i < tracker.reliablesCount; i++ )
	{
		// Look for that reliableID in the vector
		uint16_t receivedReliableID = tracker.sentReliables[i];

		// If Greater than the current highest, set the highest
		if( CycleGreater( receivedReliableID, m_highestConfirmedReliableID ) )
			m_highestConfirmedReliableID = receivedReliableID;

		// Confirm Sent Reliables
		for( uint ucI = 0; ucI < m_unconfirmedSentReliables.size(); ucI++ )
		{
			if( receivedReliableID == m_unconfirmedSentReliables[ucI]->m_header.reliableID )
			{
				// Fast delete
				std::swap( m_unconfirmedSentReliables[ucI], m_unconfirmedSentReliables.back() );
				delete m_unconfirmedSentReliables.back();
				m_unconfirmedSentReliables.back() = nullptr;

				m_unconfirmedSentReliables.pop_back();
			}
		}
	}
}

void NetworkConnection::MarkReliableReceived( uint16_t reliableID )
{
	// If Greater than the current highest, set the highest
	if( CycleGreater( reliableID, m_highestConfirmedReliableID ) )
	{
		// Put it into received list
		m_receivedReliableIDs.push_back( reliableID );

		// Update the received messages' window
		uint16_t lowestRIDToConfirm = GetLowestReliableIDToConfirm();
		for( uint r = 0; r < m_receivedReliableIDs.size(); r++ )
		{
			// Lower than the lowest ID we care about?
			if( CycleLess( m_receivedReliableIDs[r], lowestRIDToConfirm ) )
			{
				// Get rid of it
				std::swap( m_receivedReliableIDs[r], m_receivedReliableIDs.back() );
				m_receivedReliableIDs.pop_back();
			}
		}
	}
	else
	{
		// Less than the highest
		// Add it to received list
		m_receivedReliableIDs.push_back( reliableID );
	}
}

/*
	for( uint j = 0; j < m_unconfirmedSentReliables.size(); j++ )
	{
		NetworkMessage* &thisUMsg = m_unconfirmedSentReliables[j];
		bool isReceivedReliableID = (thisUMsg->m_header.reliableID == receivedReliableID);
		bool isOlderThanLowestUnconfirmed = CycleLess( thisUMsg->m_header.reliableID, GetLowestConfirmedReliableID() + 0x0001 );
	
		if( isReceivedReliableID || isOlderThanLowestUnconfirmed )
		{
			// Fast-Delete the message from unconfirmed
			std::swap( thisUMsg, m_unconfirmedSentReliables.back() );
	
			delete m_unconfirmedSentReliables.back();
			m_unconfirmedSentReliables.back() = nullptr;
	
			m_unconfirmedSentReliables.pop_back();
	
			ConsolePrintf( "Reliable ID confirmed: %d", receivedReliableID );
		}
	}
*/
