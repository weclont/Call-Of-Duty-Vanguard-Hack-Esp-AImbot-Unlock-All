#pragma once

namespace Network
{
	class Address
	{
	public:
		Address();
		Address(std::string addr);
		Address(sockaddr_in* addr);

		void setIPv4(in_addr addr);
		void setPort(unsigned short port);
		unsigned short getPort();

		sockaddr* getAddr();
		sockaddr_in* getInAddr();

		bool isLocal();

		std::string toString();

		bool operator!=(const Address &obj) const { return !(*this == obj); };
		bool operator==(const Address &obj) const;

	private:
		sockaddr_in address;

		void parse(std::string addr);
		void resolve(std::string hostname);
	};
}
