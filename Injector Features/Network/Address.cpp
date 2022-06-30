#include "STDInclude.hpp"

#include "Network/Address.hpp"

namespace Network
{
	Address::Address()
	{
		ZeroMemory(&this->address, sizeof(this->address));
	}

	Address::Address(std::string addr) : Address()
	{
		this->parse(addr);
	}

	Address::Address(sockaddr_in* addr)
	{
		this->address = *addr;
	}

	bool Address::operator==(const Address &obj) const
	{
		return !std::memcmp(&this->address, &obj.address, sizeof(this->address));
	}

	void Address::setIPv4(in_addr addr)
	{
		this->address.sin_family = AF_INET;
		this->address.sin_addr = addr;
	}

	void Address::setPort(unsigned short port)
	{
		this->address.sin_port = htons(port);
	}

	unsigned short Address::getPort()
	{
		return ntohs(this->address.sin_port);
	}

	std::string Address::toString()
	{
		char buffer[MAX_PATH] = { 0 };
		inet_ntop(this->address.sin_family, &this->address.sin_addr, buffer, sizeof(buffer));

		return Utils::String::VA("%s:%hu", buffer, this->getPort());
	}

	bool Address::isLocal()
	{
		// According to: https://en.wikipedia.org/wiki/Private_network

		// 10.X.X.X
		if (this->address.sin_addr.S_un.S_un_b.s_b1 == 10) return true;

		// 192.168.X.X
		if (this->address.sin_addr.S_un.S_un_b.s_b1 == 192 && this->address.sin_addr.S_un.S_un_b.s_b2 == 168) return true;

		// 172.16.X.X - 172.31.X.X
		if (this->address.sin_addr.S_un.S_un_b.s_b1 == 172 && (this->address.sin_addr.S_un.S_un_b.s_b2 >= 16) && (this->address.sin_addr.S_un.S_un_b.s_b2 < 32)) return true;

		// 127.0.0.1
		if (this->address.sin_addr.S_un.S_addr == 0x0100007F) return true;

		// TODO: Maybe check for matching localIPs and subnet mask

		return false;
	}

	sockaddr* Address::getAddr()
	{
		return reinterpret_cast<sockaddr*>(this->getInAddr());
	}

	sockaddr_in* Address::getInAddr()
	{
		return &this->address;
	}

	void Address::parse(std::string addr)
	{
		auto pos = addr.find_last_of(":");
		if (pos != std::string::npos)
		{
			std::string port = addr.substr(pos + 1);
			this->setPort(USHORT(atoi(port.data())));

			addr = addr.substr(0, pos);
		}

		this->resolve(addr);
	}

	void Address::resolve(std::string hostname)
	{
		addrinfo *result = nullptr;
		if (!getaddrinfo(hostname.data(), nullptr, nullptr, &result))
		{
			unsigned short port = this->getPort();
			std::memcpy(&this->address, result->ai_addr, sizeof(this->address));
			this->setPort(port);

			freeaddrinfo(result);
		}
	}
}
