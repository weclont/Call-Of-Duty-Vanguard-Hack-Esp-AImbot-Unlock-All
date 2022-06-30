#pragma once

namespace Utils
{
	namespace Network
	{
		u_short IPChecksum(const u_short* buf, u_short length);
		u_short UDPChecksum(in_addr saddr, in_addr daddr, u_short sport, u_short dport, u_char protocol, std::string data);
	}
}
