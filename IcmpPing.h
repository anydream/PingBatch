#pragma once

#include <stdint.h>
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////////
class IcmpPing
{
public:
	struct PingResult
	{
		std::string Target;
		std::string RemoteAddr;
		std::string ReplyAddr;
		uint32_t DataSize = 0;
		uint32_t Delay = -1;
		uint32_t Ttl = 0;
		uint32_t Status = 0;
	};

public:
	IcmpPing();
	~IcmpPing();

	bool Ping(PingResult &result, const std::string &strTarget, uint32_t timeOut = 1000, uint32_t payloadSize = 32);

private:
	void *IcmpFile_ = nullptr;
	std::vector<uint8_t> Payload_;
};
