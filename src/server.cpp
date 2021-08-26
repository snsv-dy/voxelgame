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
#include "terrain/Lighter.hpp"


using namespace std;
using asio::ip::tcp;

class Server {
	tcp::acceptor acceptor;
	asio::io_context& context;

	vector<shared_ptr<ClientHandler>> players;
	TsQueue<OwnedMessage> receivedMessages;
	//  
	// Thread safe world provider should be there!!!
	shared_ptr<LocalWorldProvider> provider;
	// shared_ptr<WorldProvider> provider_ptr;
	WorldLoader loader; // World loader needed for lightning, but could be avoided if lighter would have getblock and getchunk function.
	Lighter light;
	std::list<std::pair<std::shared_ptr<ClientHandler>, glm::ivec3>> pendingChunkRequests;
	std::map<glm::ivec3, int, compareVec3> chunkPlayersNumber;
	const unsigned int maxRegions = 10; // Must be like at least same as max players?? idk maybye more.
	// But for now this will suffice. 
	// 
	bool running = true;
public:
	Server(asio::io_context& context): context{context}, acceptor{context, tcp::endpoint(tcp::v4(), 25013)}, provider{make_shared<LocalWorldProvider>()}, loader{provider}, light{loader} {
		// someText = vector<char>(4);
		// provider.update(glm::ivec3(0));
		acceptClient();
	}

	void handleMessages() {
		while(running) {
			receivedMessages.wait();
			while (!receivedMessages.empty()) {
				OwnedMessage& omsg = receivedMessages.front();
				switch (omsg.msg.header.type) {
					case MsgType::ChunkRequest:
					{
						glm::ivec3 chunkPosition = omsg.msg.getData<glm::ivec3>();
						requestChunk(omsg.owner, chunkPosition);
					}break;
					default:
					{
						omsg.owner->onMessage(omsg.msg);
					}
				}
				receivedMessages.pop();
			}

			// loading chunks according to player changed position.
			// for (std::shared_ptr<ClientHandler>& player : players) {
			// 	for (const glm::ivec3& chunkPosition : player->requestedChunks) {
			// 		if (!loader.getChunk(chunkPosition).second) {
			// 			// printf("loading: %2d %2d %2d\n", chunkPosition.x, chunkPosition.y, chunkPosition.z);
			// 			loader.loadChunk(chunkPosition);
			// 		}
			// 	}
			// }

			// Light update
			auto unlitColumns = loader.getUnlitColumns();
			if(unlitColumns.size() > 0) {
				light.updateLightColumns(unlitColumns);
				printf("\t\tcolumns updated\n");
			}
			
			std::list<ChangedBlock> blocks_changed = loader.getChangedBlocks();

			if (!blocks_changed.empty()) {
				light.updateLightForBlock(blocks_changed);
			}
			
			light.propagateLights();

			// Sending chunks after hypothetical terrain generation
			// (Changed chunks that were modified are not resent)
			// for (auto req : pendingChunkRequests) {
			// 	// chunksWithPlayers.insert
			// 	sendChunk(req.first, req.second);
			// }
			// pendingChunkRequests.clear();
			
			for (std::shared_ptr<ClientHandler>& player : players) {
				for (const glm::ivec3& chunkPosition : player->chunksToUnload) {
					int& nplayers = chunkPlayersNumber[chunkPosition];
					nplayers -= 1;
					if(nplayers == 0) {
						chunkPlayersNumber.erase(chunkPosition);
						printf("chunk unloaded: %2d %2d %2d\n", chunkPosition.x, chunkPosition.y, chunkPosition.z);
						// and somehow hint provider/loader to unload this chunk.
					}
				}
				player->chunksToUnload.clear();
			}

			// loader.addUpdatedChunks(light.getUpdatedChunks());

			for (const glm::ivec3 chunkPosition : light.getUpdatedChunks()) {
				for (std::shared_ptr<ClientHandler>& player : players) {
					if (player->chunkInRange(chunkPosition)) {
						player->requestedChunks.insert(chunkPosition);
						// sendChunk(player, chunkPosition);
					}
				}
			}

			for (std::shared_ptr<ClientHandler>& player : players) {
				for (const glm::ivec3 chunkPosition : player->requestedChunks) {
					if (player->chunkInRange(chunkPosition)) {
						sendChunk(player, chunkPosition);
					}
				}
				player->requestedChunks.clear();
			}

			// for (std::shared_ptr<ClientHandler>& player : players) {
			// 	for (const glm::ivec3& chunkPosition : player->loadedChunks) {
			// 		auto [c, found] = loader.getChunk(chunkPosition);
			// 		if (c.modified) {
			// 			sendChunk(player, chunkPosition);
			// 		}
			// 	}
			// 	for (const glm::ivec3& chunkPosition : player->requestedChunks) {
			// 		player->loadedChunks.insert(chunkPosition);
			// 		sendChunk(player, chunkPosition);
			// 	}
			// 	player->requestedChunks.clear();
			// }

			// unloadRegions();
		}
	}

	void requestChunk(shared_ptr<ClientHandler> client, glm::ivec3 chunkPosition) {
		// bool inVicinity = chunkPosition.y >= 0;//!glm::any(glm::bvec3(glm::greaterThanEqual(glm::abs(chunkPosition - playerChunkPosition), vecDrawDistance)));
		if (client->chunkInRange(chunkPosition)) {
			// 
			if (!loader.getChunk(chunkPosition).second) {
				// printf("loading: %2d %2d %2d\n", chunkPosition.x, chunkPosition.y, chunkPosition.z);
				loader.loadChunk(chunkPosition);
			}
			// mutex here?
			client->requestedChunks.insert(chunkPosition);

			// std::pair<std::shared_ptr<ClientHandler>, glm::ivec3> req = {client, chunkPosition};
			// pendingChunkRequests.push_back(req);
		}
	}

	void sendChunk(shared_ptr<ClientHandler> client, glm::ivec3 chunkPosition) {
		auto [data, offset, generated] = provider->getChunkData(chunkPosition);
		if (data != nullptr) {
			client->loadedChunks.insert(chunkPosition);

			if (auto it = chunkPlayersNumber.find(chunkPosition); it == chunkPlayersNumber.end()) {
				chunkPlayersNumber[chunkPosition] = 1;
			} else {
				it->second++;
			}

			Message msg;
			size_t chunkDataSize = TerrainConfig::ChunkCubed * sizeof(unsigned short);
			size_t dataSize = chunkDataSize + sizeof(glm::ivec3);
			msg.data.resize(dataSize);
			// printf("	Sending chunk size: %d %d\n", t_chunk.size(), msg.data.size());
			memcpy(msg.data.data(), &chunkPosition, sizeof(glm::ivec3));
			memcpy(msg.data.data() + sizeof(glm::ivec3), data + offset, chunkDataSize);

			msg.header.type = MsgType::Chunk;
			msg.header.size = dataSize; // should be region_dtype
			// printf("header, data: %d %d\n", msg.header.size, msg.data.size());
			client->sendMessage(msg);
		} else {
			// printf("data at: %2d %2d %2d is nullptr\n", chunkPosition.x, chunkPosition.y, chunkPosition.z);
		}
	}

	// This could probably be improved but i don't have better idea now.
	void unloadRegions() {
		// std::set<glm::ivec3, compareVec3> playerPositions;
		// for (auto player : clients) {
		// 	playerPositions.insert(player->playerChunkPosition);
		// }

		// provider->unloadRegions(playerPositions);
	}

	void acceptClient() {
		acceptor.async_accept([this] (error_code err, tcp::socket socket) {
			if(!err) {
				// Make sure this doesn't get removed after this function finishes.
				shared_ptr<ClientHandler> client = make_shared<ClientHandler>(context, std::move(socket), 12, &receivedMessages, *provider);
				players.push_back(client);

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