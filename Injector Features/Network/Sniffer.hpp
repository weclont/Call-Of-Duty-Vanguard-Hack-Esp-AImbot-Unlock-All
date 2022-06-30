#pragma once

#include <windivert.h>

namespace Network
{
	class Sniffer;

	class Packet
	{
	public:
		Sniffer* sniffer;
		WINDIVERT_ADDRESS address;

		Network::Address source;
		Network::Address target;

		std::string_view data;
		std::string_view rawData;
		std::string staticData;

		bool drop;
	};

	class Sniffer
	{
	public:
		typedef Utils::Slot<void(Network::Packet* packet)> Callback;

		Sniffer();
		~Sniffer();

		bool send(Network::Packet* packet);
		void onPacket(Callback callback);

		void run();
		void stop();
		bool isRunning();

	private:
		HANDLE handle;
		Callback callback;
		Utils::NT::Module divert;

		bool stopped;

		void extractRessources();
	};
}