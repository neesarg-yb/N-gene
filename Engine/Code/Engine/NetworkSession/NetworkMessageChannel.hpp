#pragma once
#include <queue>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/NetworkSession/NetworkMessage.hpp"

struct NetworkMessageAndSender
{
public:
	NetworkMessage	*message	= nullptr;
	NetworkSender	*sender		= nullptr;

public:
	NetworkMessageAndSender() { }
	NetworkMessageAndSender( NetworkMessage *newMessage, NetworkSender *newSender )
		: message( newMessage )
		, sender( newSender ) { }
};

// Puts the lowest sequenceID having message on the top
struct CustomCompareForInorderMessagesAndSender
{
	bool operator () ( NetworkMessageAndSender const &lhs, NetworkMessageAndSender const &rhl )
	{
		return lhs.message->m_header.sequenceID > rhl.message->m_header.sequenceID;
	}
};

typedef std::priority_queue< NetworkMessageAndSender, std::vector<NetworkMessageAndSender>, CustomCompareForInorderMessagesAndSender > InOrderNetworkMessages;


class NetworkMessageChannel
{
public:
	 NetworkMessageChannel();
	~NetworkMessageChannel();

private:
	uint16_t				m_nextSentSequenceID		= 0U;
	uint16_t				m_nextExpectedSequenceID	= 0U;

public:
	InOrderNetworkMessages	m_pendingMessagesToProcess;

public:
	inline uint16_t	GetNextSequenceIDToSend() const { return m_nextSentSequenceID; }
	inline void		IncrementNextSendSequenceID()	{ m_nextSentSequenceID++; }

	inline uint16_t GetExpectedSequenceID() const	{ return m_nextExpectedSequenceID; }
	inline void		IncrementExpectedSequenceID() 	{ m_nextExpectedSequenceID++; }
};
