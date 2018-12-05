#pragma once
#include "UDPSocket.hpp"

bool UDPSocket::Bind( NetworkAddress &address, uint16_t portRange /*= 0U */ )
{
	// Create the socket 
	SOCKET mySocket = socket( AF_INET,			// IPv4 to send...
							  SOCK_DGRAM,		// ...Datagrams... 
							  IPPROTO_UDP );	// ...using UDP.
	GUARANTEE_RECOVERABLE( mySocket != INVALID_SOCKET, "UDPSocket: Invalid socket!!" ); 
	
	for ( int remainingPorts = portRange; remainingPorts >= 0; remainingPorts-- )
	{
		// Get sockaddr
		sockaddr sock_addr;
		size_t sock_addr_len;
		
		bool success = address.ToSocketAddress( &sock_addr, &sock_addr_len );
		if( success == false )
			break;

		// try to bind - if it succeeds - great.  If not, try the next port in the range.
		int result = ::bind( mySocket, &sock_addr, (int)sock_addr_len );
		if (0 == result) 
		{
			m_handle  = mySocket; 
			m_address = address; 
			return true; 
		} 

		// Check so that the port won't overflow
		if( address.port == 0xffff )
			break;
		else
			address.port++;
	}

	::closesocket( mySocket );
	return false; 
}

size_t UDPSocket::SendTo( NetworkAddress const &address, void const *data, size_t const byteCount )
{
	sockaddr_storage sock_addr;
	int addr_len;
	NetworkAddress::GetSocketAddressForHost( (sockaddr*)&sock_addr, &addr_len, address.IPToString().c_str(), address.PortToString().c_str() );

	SOCKET sock = (SOCKET) m_handle;
	int sent = ::sendto(	sock,								// socket we're sending from
							(char const *)data,					// data we want to send
							(int)byteCount,						// bytes to send
							0,									// unused flags
							(sockaddr*)&sock_addr,addr_len );	// address we're sending to

	if( sent > 0 )
	{
		GUARANTEE_RECOVERABLE( sent == byteCount, "UDP: Couldn't send all bytes!" );
		return (size_t)sent;
	}
	else
	{
// 		if( HasFatelError() )
// 			Close();
		return 0U;
	}
}

size_t UDPSocket::ReceiveFrom( NetworkAddress *outAddress, void *buffer, size_t const maxReadSize )
{
	if( IsClosed() )
		return 0U;

	sockaddr_storage fromAddr;
	int addr_len = sizeof( sockaddr_storage );
	SOCKET sock = m_handle;

	int recvd = ::recvfrom( sock,									// what socket am I receiving on
							(char*) buffer,
							(int) maxReadSize,						// max I can read
							0,										// unused flags
							(sockaddr*) &fromAddr, &addr_len );		// who sent it

	if( recvd > 0 )
	{
		*outAddress = NetworkAddress( (sockaddr*)&fromAddr );
		return recvd;
	}
	else
	{
// 		if( HasFatelError() )
// 			Close();

		return 0;
	}
}
