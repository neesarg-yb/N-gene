#pragma once
#include "NetworkSession.hpp"

NetworkConnection::NetworkConnection( NetworkAddress &addr, NetworkSession &parentSession )
	: connection( addr )
	, session( parentSession )
{

}

NetworkConnection::~NetworkConnection()
{

}

MessageQueueElement::MessageQueueElement( NetworkAddress &connectionAddr, NetworkMessage &msg )
	: connection( connectionAddr )
	, message( msg )
	, operationComplete( false )
{

}

void NetworkConnection::Send( NetworkMessage &msgToSend )
{
	session.QueueMessageToSend( msgToSend, connection );
}

NetworkSession::NetworkSession()
{

}

NetworkSession::~NetworkSession()
{

}

bool NetworkSession::AddBinding( uint16_t port )
{
	// Delete the existing one
	if( m_mySocket != nullptr )
		delete m_mySocket;

	m_mySocket = new UDPSocket();

	// get an address to use; 
	NetworkAddress addr = NetworkAddress::GetLocal();
	addr.port = port;

	if ( !m_mySocket->Bind( addr, 0 ) ) 
	{
		ConsolePrintf( "NetworkSession: Failed to bind." );
		return false;  
	} 
	else 
	{
		m_mySocket->EnableNonBlocking(); // if you have cached options, you could do this
									  // in the constructor; 
		ConsolePrintf( "Socket bound: %s", m_mySocket->m_address.AddressToString().c_str() );
		return true; 
	}
}

void NetworkSession::QueueMessageToSend( NetworkMessage &msgToSend, NetworkAddress &receiver )
{
	UNUSED( msgToSend );
	UNUSED( receiver );

	// We need to divide the message in chunks of size less than PACKET_MTU
	//	Such that each chunk has its index, suggesting that "Hey I'm 5th chunk, out of 12 chunks!"
	//	And remaining of the chunk will be filled with message's data bytes
	std::vector< void * > chunksOfThisMessage;
	
	// Add it to a pending out box list on Session Class, with a network address to send to..
}

void NetworkSession::RegisterMessage( char const *messageName, sessionMessage_cb cb )
{
	std::map< std::string, SessionMessageCallbacks >::iterator it;
	it = m_registeredMessages.find( messageName );

	// If this messageName is not registered, add new entry
	if( it == m_registeredMessages.end() )
		m_registeredMessages[ messageName ] = SessionMessageCallbacks();

	m_registeredMessages[ messageName ].push_back( cb );
}

NetworkConnection* NetworkSession::AddConnection( int idx, NetworkAddress &addr )
{
	// Delete the existing one
	if( idx < m_connections.size() )
	{
		delete m_connections[idx];
		m_connections[idx] = nullptr;
	}

	m_connections[idx] = new NetworkConnection( addr, *this );
	return m_connections[idx];
}

NetworkConnection* NetworkSession::GetConnection( int idx )
{
	if( idx < m_connections.size() )
		return nullptr;
	else
		return m_connections[idx];
}

void NetworkSession::ProcessIncoming()
{
	// Receive a chunk of message
		// Store it to m_incomingMessages list, along side the sender's address..
	
	// If we're missing any chunk whose index is lower than the one we received,
		// Request the sender to re-send that missing indexed chunk

	// See if all the chunks of that message are received..
		// If yes, do a callback with registered message!
			// Send acknowledgment to sender saying: "Hey I received all the chunks of this message, you can get rid of it from your end."
			// And after that delete vector entry from m_incomingMessages;

	// If received a request to resend a previously sent chunk
		// Find it from the m_outgoingMessages vector,
			// Send it again..

	// If received an acknowledgment saying that "I received all chunks of this message!"
		// Delete that message from m_outgoingMessages..
}

void NetworkSession::ProcessOutgoing()
{
	// Go through m_outgoingMessages list..
		// Send all the pending messages to its receiver's address..
		// i.e. MessageQueueElement with (operationComplete == false)
			// Mark it "sent" (operationComplete = true) 
			// So we don't send it again unless client requests us to do so..

	// If sending fails, close that socket..
		// Remove all the messages from m_outgoingMessages..
		// Consider it as connection closed!
}
