#include "STDInclude.hpp"

#include "Network/Address.hpp"
#include "Network/Sniffer.hpp"

#include "Game/MW2/Handler.hpp"
#include "Game/MW2/Client.hpp"

namespace Game
{
	namespace MW2
	{
		Handler::Handler(Network::Sniffer* sniffer)
		{
			using namespace std::placeholders;
			sniffer->onPacket(std::bind(&Handler::handler, this, _1));
			printf("Listening for MW2 connections...\n");
		}
		
		Handler::~Handler()
		{

		}

		void Handler::handler(Network::Packet* packet)
		{
			if (packet->data.find("\xFF\xFF\xFF\xFF" "getchallenge") == 0
				&& packet->source.isLocal()) // If source is not local, then someone is connecting to us or the network config is fucked up!
			{
				std::lock_guard<std::mutex> _(this->mutex);
				this->client = std::make_shared<Client>(this, packet->target);
			}

			if (this->client)
			{
				int dir = this->client->getDirection(packet->source, packet->target);
				if (dir != Client::PacketDirection::NONE)
				{
					this->client->handle(packet, dir == Client::PacketDirection::REMOTE_TO_LOCAL);
				}
			}
		}

		void Handler::sendPayload(Network::Sniffer* sniffer, int num)
		{
			std::lock_guard<std::mutex> _(this->mutex);
			if(this->client) this->client->sendPayload(sniffer, num);
		}
	}
}
