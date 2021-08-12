#include <glm/glm.hpp>
#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <functional>
#include <cstdio>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <memory>
#include "server/netCommon.hpp"
#include "server/ClientHandler.hpp"

// Completely unnecessary just wanted to compile it 
#include "utilities/basic_util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
// End o

#include "terrain/worldProvider.hpp"
#include "terrain/LocalWorldProvider.hpp"
#include "terrain/WorldLoader.h"


using namespace std;
using asio::ip::tcp;




class Server {
	tcp::acceptor acceptor;
	asio::io_context& context;

	vector<char> someText;
	vector<shared_ptr<ClientHandler>> clients;
	TsQueue<OwnedMessage> receivedMessages;
	//  
	// Thread safe world provider should be there!!!
	shared_ptr<LocalWorldProvider> provider;
	// shared_ptr<WorldProvider> provider_ptr;
	WorldLoader loader;
	const unsigned int maxRegions = 10; // Must be like at least same as max players?? idk maybye more.
	// But for now this will suffice. 
	// 
	bool running = true;
public:
	Server(asio::io_context& context): context{context}, acceptor{context, tcp::endpoint(tcp::v4(), 25013)}, provider{make_shared<LocalWorldProvider>()}, loader{provider} {
		someText = vector<char>(4);
		// provider.update(glm::ivec3(0));
		acceptClient();
	}

	void handleMessages() {
		while(running) {
			receivedMessages.wait();
			while (!receivedMessages.empty()) {
				OwnedMessage& omsg = receivedMessages.front();
				omsg.owner->onMessage(omsg.msg);
				receivedMessages.pop();
			}

			unloadRegions();
		}
	}

	// This could probably be improved but i don't have better idea now.
	void unloadRegions() {
		std::set<glm::ivec3, compareVec3> playerPositions;
		for (auto player : clients) {
			playerPositions.insert(player->playerChunkPosition);
		}

		provider->unloadRegions(playerPositions);
	}

	void acceptClient() {
		acceptor.async_accept([this] (error_code err, tcp::socket socket) {
			if(!err) {
				// Make sure this doesn't get removed after this function finishes.
				shared_ptr<ClientHandler> client = make_shared<ClientHandler>(std::move(socket), 12, &receivedMessages, *provider);
				clients.push_back(client);

				acceptClient(); // This is not infinite recursion.
								// Accepting is done in context.run(); (probably)
			} else {
				printf("Accepting client error: %s\n", err.message().c_str());
			}
			// thread t(ClientHandler(move(socket)));
		});
	}
};

int main() {
	// try {
		// glm::ivec3 a(-1, 0, 0);
		// glm::ivec3 pos(1, 0, 2);
		// bool rel = ;
		// printf("eee: %2d %2d %2d \n", rel);


		// return 0;
		asio::io_context context;
		Server s(context);
		// acceptor = tcp::acceptor(context, tcp::endpoint(tcp::v4(), 16013));
		// acceptor.async_accept(accept_func);

		thread asioThread([&] {
			context.run();
		});
		
		s.handleMessages();

		printf("context stopped\n");

	return 0;
}