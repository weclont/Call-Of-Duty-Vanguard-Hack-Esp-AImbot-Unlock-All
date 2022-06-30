#include "STDInclude.hpp"

#include "Network/Address.hpp"
#include "Network/Sniffer.hpp"

namespace Network
{
	bool Sniffer::isRunning()
	{
		return !this->stopped;
	}

	void Sniffer::stop()
	{
		this->stopped = true;

		if (this->handle != INVALID_HANDLE_VALUE)
		{
			this->divert.invokePascal<BOOL>("WinDivertClose", this->handle);
			this->handle = INVALID_HANDLE_VALUE;
		}
	}

	void Sniffer::run()
	{
		Utils::Memory::Allocator allocator;

		u_int size = 0;
		const u_int maxSize = 0x10000;
		u_char* buffer = allocator.allocateArray<u_char>(maxSize);

		auto winDiverSend = this->divert.get<BOOL(__stdcall)(HANDLE, PVOID, UINT, PWINDIVERT_ADDRESS, UINT*)>("WinDivertSend");
		auto winDiverReceive = this->divert.get<BOOL(__stdcall)(HANDLE, PVOID, UINT, PWINDIVERT_ADDRESS, UINT*)>("WinDivertRecv");

		Packet packet;
		while (!this->stopped && this->handle != INVALID_HANDLE_VALUE)
		{
			if (winDiverReceive(this->handle, buffer, maxSize, &packet.address, &size) == TRUE)
			{
				PWINDIVERT_IPHDR ipHeader = PWINDIVERT_IPHDR(buffer);
				PWINDIVERT_UDPHDR udpHeader = PWINDIVERT_UDPHDR(buffer + (ipHeader->HdrLength * 4));

				packet.drop = false;
				packet.sniffer = this;
				packet.rawData = std::string_view(LPSTR(buffer), size);
				packet.data = std::string_view(LPSTR(udpHeader) + sizeof(WINDIVERT_UDPHDR), ntohs(udpHeader->Length) - sizeof(WINDIVERT_UDPHDR));

				if (udpHeader->Length <= sizeof(WINDIVERT_UDPHDR) || packet.data.size() > maxSize) packet.data = std::string_view(LPSTR(udpHeader), 0);

				IN_ADDR addr;
				addr.S_un.S_addr = ipHeader->SrcAddr;
				packet.source.setIPv4(addr);

				addr.S_un.S_addr = ipHeader->DstAddr;
				packet.target.setIPv4(addr);

				packet.source.setPort(ntohs(udpHeader->SrcPort));
				packet.target.setPort(ntohs(udpHeader->DstPort));

				if (this->callback) this->callback(&packet);
				if (!packet.drop) winDiverSend(this->handle, PVOID(packet.rawData.data()), UINT(packet.rawData.size()), &packet.address, &size);
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}

	bool Sniffer::send(Network::Packet* packet)
	{
		this->divert.invokePascal<BOOL>("WinDivertHelperCalcChecksums", PVOID(packet->rawData.data()), UINT(packet->rawData.size()), 0ui64);

		u_int size = 0;
		BOOL result = this->divert.invokePascal<BOOL>("WinDivertSend", this->handle, PVOID(packet->rawData.data()), UINT(packet->rawData.size()), &packet->address, &size) == TRUE;
		return (result && size == packet->rawData.size());
	}

	void Sniffer::onPacket(Callback _callback)
	{
		this->callback = _callback;
	}

	void Sniffer::extractRessources()
	{
#ifndef _WIN64
		BOOL is64Bit = FALSE;
		IsWow64Process(GetCurrentProcess(), &is64Bit);

		if (is64Bit)
		{
#endif
			Utils::IO::File driver("WinDivert64.sys");
			driver.write(Utils::LoadResource(WINDIVERT_DRIVER_x64));
#ifndef _WIN64
		}
		else
		{
			Utils::IO::File driver("WinDivert32.sys");
			driver.write(Utils::LoadResource(WINDIVERT_DRIVER_x86));
		}
#endif

		Utils::IO::File library("WinDivert.dll");
		library.write(Utils::LoadResource(WINDIVERT_DLL));
	}

	Sniffer::Sniffer() : stopped(false), handle(INVALID_HANDLE_VALUE)
	{
		this->extractRessources();

		this->divert = Utils::NT::Module::Load("WinDivert.dll");
		if (this->divert.isValid())
		{
			this->handle = this->divert.invokePascal<HANDLE>("WinDivertOpen", "ip and udp", WINDIVERT_LAYER_NETWORK, -1000i16, 0ui64);
		}
	}

	Sniffer::~Sniffer()
	{
		this->stop();
	}
}
