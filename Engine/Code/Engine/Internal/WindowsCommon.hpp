#if !defined(WIN32_LEAN_AND_MEAN)	// Because some thirdparty headfiles defines it already
#define WIN32_LEAN_AND_MEAN			//		So if you include them alongside this header file,
#endif								//		the compiler yells at you, otherwise..

// Order of following includes matters..
#include <WinSock2.h>
#include <WS2tcpip.h>			// IPv6 (optional)
#include <Windows.h>