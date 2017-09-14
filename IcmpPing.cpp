#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include "IcmpPing.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

//////////////////////////////////////////////////////////////////////////
IcmpPing::IcmpPing()
{
	WSADATA data;
	WSAStartup(MAKEWORD(1, 1), &data);

	IcmpFile_ = IcmpCreateFile();
	if (IcmpFile_ == INVALID_HANDLE_VALUE)
		IcmpFile_ = nullptr;
}

IcmpPing::~IcmpPing()
{
	if (IcmpFile_)
		IcmpCloseHandle(IcmpFile_);
}

bool IcmpPing::Ping(PingResult &result, const std::string &strTarget, uint32_t timeOut, uint32_t payloadSize)
{
	if (!IcmpFile_)
		return false;

	uint32_t targetIntAddr = inet_addr(strTarget.c_str());
	if (targetIntAddr == INADDR_NONE)
	{
		hostent *he = gethostbyname(strTarget.c_str());
		if (!he)
			return false;
		targetIntAddr = ((in_addr *)he->h_addr_list[0])->S_un.S_addr;
	}

	result.Target = strTarget;

	in_addr targetAddr;
	targetAddr.S_un.S_addr = targetIntAddr;
	result.RemoteAddr = inet_ntoa(targetAddr);

	Payload_.resize(payloadSize);

	const uint32_t replySize = sizeof(ICMP_ECHO_REPLY) + (uint32_t)Payload_.size();
	uint8_t *replyBuffer = (uint8_t*)alloca(replySize);

	DWORD dwRetVal = IcmpSendEcho(
		IcmpFile_, targetIntAddr,
		Payload_.data(), (uint32_t)Payload_.size(), NULL,
		replyBuffer, replySize, timeOut);

	if (dwRetVal != 0)
	{
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replyBuffer;

		in_addr replyAddr;
		replyAddr.S_un.S_addr = pEchoReply->Address;

		result.ReplyAddr = inet_ntoa(replyAddr);
		result.DataSize = pEchoReply->DataSize;
		result.Delay = pEchoReply->RoundTripTime;
		result.Ttl = pEchoReply->Options.Ttl;
		result.Status = pEchoReply->Status;

		return true;
	}
	return false;
}
