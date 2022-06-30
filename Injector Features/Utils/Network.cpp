#include "STDInclude.hpp"

namespace Utils
{
	namespace Network
	{
		u_short IPChecksum(const u_short* buf, u_short length)
		{
			u_short checkSum = 0;
			for (int i = 0; i < length; i += 2)
			{
				u_short field = ntohs(buf[i / 2]);
				u_short diff = 65535 - checkSum;
				checkSum += field;
				if (field > diff) checkSum += 1;
			}

			return htons(~checkSum);
		}

		u_short UDPChecksum(in_addr saddr, in_addr daddr, u_short sport, u_short dport, u_char protocol, std::string data)
		{
			Utils::Buffer buffer;
			buffer.write<u_char>(protocol);
			buffer.write<in_addr>(saddr);
			buffer.write<in_addr>(daddr);

			u_short length = htons(u_short(data.size()) + 8);
			buffer.write<u_short>(length);
			buffer.write<u_short>(length);

			buffer.write<u_short>(htons(sport));
			buffer.write<u_short>(htons(dport));

			buffer.append(data);

			if (buffer.size() % 2) buffer.push_back(0);

			return ntohs(Network::IPChecksum(PUSHORT(buffer.data()), u_short(buffer.size())));
		}
	}
}
