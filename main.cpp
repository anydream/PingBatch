#include <stdint.h>
#include <stdio.h>
#include "IcmpPing.h"
#include <algorithm>
#include <unordered_set>

//////////////////////////////////////////////////////////////////////////
static void PrintResult(const IcmpPing::PingResult &result)
{
	std::string replyAddr = result.ReplyAddr;
	if (replyAddr != result.RemoteAddr)
		replyAddr += '(' + result.RemoteAddr + ')';

	std::string strHead = result.Target + '|' + replyAddr;
	while (strHead.size() < 32)
		strHead.push_back(' ');

	printf("* %s Delay: %ums, TTL: %u, Size: %u, Status: %u\n",
		strHead.c_str(),
		result.Delay,
		result.Ttl,
		result.DataSize,
		result.Status);
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	std::vector<std::string> argList;
	for (int i = 1; i < argc; ++i)
	{
		argList.push_back(argv[i]);
	}

	uint32_t timeOut = 1000;
	uint32_t payloadSize = 32;
	std::vector<std::string> targetList;
	std::unordered_set<std::string> targetSet;

	for (int i = 0, sz = (int)argList.size(); i < sz; ++i)
	{
		const auto &str = argList[i];
		if (i != sz - 1)
		{
			const auto &strNext = argList[i + 1];
			if (str == "-timeout")
			{
				timeOut = strtol(strNext.c_str(), nullptr, 10);
				++i;
				continue;
			}
			else if (str == "-size")
			{
				payloadSize = strtol(strNext.c_str(), nullptr, 10);
				++i;
				continue;
			}
		}
		if (targetSet.find(str) == targetSet.end())
		{
			targetSet.insert(str);
			targetList.emplace_back(str);
		}
	}

	printf("====== Timeout: %ums, Size: %u ======\n", timeOut, payloadSize);

	IcmpPing ping;

	std::vector<IcmpPing::PingResult> pingResults;

	for (const auto &strTarget : targetList)
	{
		IcmpPing::PingResult result;
		bool status = ping.Ping(result, strTarget, timeOut, payloadSize);

		if (!status)
			printf("* TIMEOUT ");

		pingResults.emplace_back(result);
		PrintResult(result);
	}

	printf("\n====== Sorting... ======\n\n");

	std::sort(pingResults.begin(), pingResults.end(),
		[](const IcmpPing::PingResult &lhs, const IcmpPing::PingResult &rhs)
	{
		if (lhs.Status == 0 && lhs.Status == rhs.Status)
			return lhs.Delay < rhs.Delay;
		return lhs.Status < rhs.Status;
	});

	for (auto &result : pingResults)
	{
		PrintResult(result);
	}

	return 0;
}
