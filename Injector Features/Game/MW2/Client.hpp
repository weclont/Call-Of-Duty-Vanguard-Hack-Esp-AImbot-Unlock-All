#pragma once

#define MAX_RELIABLE_COMMANDS 128
#define MAX_STRING_CHARS 1024

namespace Game
{
	namespace MW2
	{
		class Client
		{
		public:
			enum PacketDirection
			{
				NONE = 0,
				LOCAL_TO_REMOTE,
				REMOTE_TO_LOCAL
			};

			Client(Handler* handler, Network::Address target);
			~Client();

			int getDirection(Network::Address source, Network::Address target);
			void handle(Network::Packet* packet, bool receiving);

			void sendPayload(Network::Sniffer* sniffer, int num);

		private:
			typedef Utils::Slot<void(Network::Packet* packet, std::string data, bool receiving)> Callback;

			Handler* handler;
			Network::Address target;
			Network::Packet packetTemplate;

			bool workerInstalled;

			u_int64 xuid;
			u_short qport;
			u_int challenge;

			u_int id; // Packet id
			u_int ack; // Reliable command

			u_char commands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS];

			std::map<std::string, Callback> oobHandlers;

			bool handleOOB(Network::Packet* packet, bool receiving);
			void handleNetchan(Network::Packet* packet, bool receiving);

			void onOOB(std::string command, Callback callback);
			void send(Network::Sniffer* sniffer, std::string data);

			std::string compressData(std::string data);
		};
	}
}
