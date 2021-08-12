#ifndef __CLIENT_HANDLER_HPP__
#define __CLIENT_HANDLER_HPP__

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
#include "netCommon.hpp"
#include "../terrain/LocalWorldProvider.hpp"
#include "../terrain/TerrainConfig.hpp"
#include "../utilities/basic_util.h"

class ClientHandler;

struct OwnedMessage {
	Message msg;
	std::shared_ptr<ClientHandler> owner = nullptr;
	OwnedMessage(Message msg, std::shared_ptr<ClientHandler> owner): msg{msg}, owner{owner} {}
};

class ClientHandler : public enable_shared_from_this<ClientHandler>, public Connection {
	// asio::ip::tcp::socket mclient;
	// MsgHeader header {MsgType::Id, 0};
	const unsigned int drawDistance = 2; // Chunks
	const glm::ivec3 vecDrawDistance {drawDistance};
	uint32_t id;
	vector<uint16_t> t_chunk;
	TsQueue<OwnedMessage>* serverReceivedQueue = nullptr;
	LocalWorldProvider& worldProvider;

	// glm::ivec3 prevPlayerChunkPosition;
	bool firstPosition = true;

	std::set<glm::ivec3, compareVec3> loadedChunks;
	// Connection conn;
public:
	glm::ivec3 playerChunkPosition;
	glm::vec3 playerPosition;
	ClientHandler(tcp::socket client, uint32_t id, TsQueue<OwnedMessage>* serverQueue, LocalWorldProvider& worldProvider);
	void genSampleChunk();
	void sendId();
	void sendChunk(glm::ivec3 chunkPosition);

	// Overriden from connection.
	void addToReceivedQueue();
	void onMessage(Message msg);
	~ClientHandler();
};

#endif