#pragma once
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Network/Socket.hpp"

class UDPSocket : public Socket
{
public:
	bool Bind( NetworkAddress const &address, uint16_t portRange = 0U );

	size_t SendTo( NetworkAddress const &address, void const *data, size_t const byteCount );
	size_t ReceiveFrom( NetworkAddress *outAddress, void *buffer, size_t const maxReadSize );
};

// IPv4 Header Size: 20B
// IPv6 Header Size: 40B
// TCP Header Size: 20B-60B
// UDP Header Size: 8B 
// Ethernet: 28B, but MTU is already adjusted for it
// so packet size is 1500 - 40 - 8 => 1452B (why?)


class UDPTest
{
public:

	bool Start() 
	{
		// get an address to use; 
		NetworkAddress addr = NetworkAddress::GetLocal();
		addr.port = GAME_PORT;

		if (!m_socket.Bind( addr, 10 )) {
			ConsolePrintf( "Failed to bind." );
			return false;  
		} else {
			m_socket.EnableNonBlocking(); // if you have cached options, you could do this
										  // in the constructor; 
			ConsolePrintf( "Socket bound: %s:%s", m_socket.m_address.IPToString().c_str(), m_socket.m_address.PortToString().c_str() );
			return true; 
		}
	}

	void Stop()
	{
		m_socket.Close(); 
	}

	void SendTo( NetworkAddress const &addr, void const *buffer, uint byte_count )
	{
		m_socket.SendTo( addr, buffer, byte_count ); 
	}

	void Update()
	{
		byte_t buffer[PACKET_MTU]; 

		NetworkAddress from_addr; 
		size_t read = m_socket.ReceiveFrom( &from_addr, buffer, PACKET_MTU ); 

		if (read > 0U) {
			uint max_bytes = (read < 128) ? (uint)read : 128; 
			
			uint stringSize = ( max_bytes * 2U + 3U ); // .. 0x & null terminator
			char *outBuffer = (char *)malloc( stringSize );
			sprintf_s( outBuffer, 3U, "0x" );

			byte_t *iter = (byte_t *)outBuffer;
			iter += 2U; // skip the 0x

			for (uint i = 0; i < read; ++i) {
				sprintf_s( (char*)iter, 3U, "%02X", buffer[i] ); 
				iter += 2U; 
			}
			buffer[read] = '\0';
			*iter = NULL; 
			
			ConsolePrintf( "Received from %s:%s;\n%s", from_addr.IPToString().c_str(), from_addr.PortToString().c_str(), outBuffer );

			free( outBuffer );
		}
	}

public:
	// if you have multiple address, you can use multiple sockets
	// but you have to be clear on which one you're sending from; 
	UDPSocket m_socket; 
};
