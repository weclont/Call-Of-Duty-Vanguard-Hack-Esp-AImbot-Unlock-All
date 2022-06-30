#pragma once

namespace Game
{
	namespace MW2
	{
		class Client;

		class Handler
		{
		public:
			Handler(Network::Sniffer* sniffer);
			~Handler();

			void sendPayload(Network::Sniffer* sniffer, int num);

		private:
			std::mutex mutex;
			std::shared_ptr<Client> client;

			void handler(Network::Packet* packet);
		};
	}
}
