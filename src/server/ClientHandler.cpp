#include "ClientHandler.hpp"

ClientHandler::ClientHandler(tcp::socket client, uint32_t id, TsQueue<OwnedMessage>* serverQueue, LocalWorldProvider& worldProvider): id{id}, worldProvider{worldProvider}, Connection{std::move(client)}, serverReceivedQueue{serverQueue} {
	// printf("client connected: %s:%d\n", mclient.local_endpoint().address().to_string().c_str(), mclient.local_endpoint().port());
	printf("client made\n");
	genSampleChunk();
	connectToClient();
	sendId();
}

void ClientHandler::genSampleChunk() {
	const int chunkSize = 16;
	vector<uint16_t> chunk(chunkSize * chunkSize * chunkSize);
	for (int z = 0; z < chunkSize; z++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int x = 0; x < chunkSize; x++) {
				chunk[z * chunkSize * chunkSize + y * chunkSize + x] = 2 * (y < 8);
			}
		}
	}
	t_chunk = chunk;
}

void ClientHandler::sendId() {
	Message msg;
	msg.setData<uint32_t>(id);
	msg.header.type = MsgType::Id;
	msg.header.size = msg.data.size();
	sendMessage(msg);
}

void ClientHandler::sendChunk(glm::ivec3 chunkPosition) {
	Message msg;

	auto [data, offset, generated] = worldProvider.getChunkData(chunkPosition);
	if (data != nullptr) {
		size_t chunkDataSize = TerrainConfig::ChunkCubed * sizeof(unsigned short);
		size_t dataSize = chunkDataSize + sizeof(glm::ivec3);
		msg.data.resize(dataSize);
		// printf("	Sending chunk size: %d %d\n", t_chunk.size(), msg.data.size());
		memcpy(msg.data.data(), &chunkPosition, sizeof(glm::ivec3));
		memcpy(msg.data.data() + sizeof(glm::ivec3), data + offset, chunkDataSize);

		msg.header.type = MsgType::Chunk;
		msg.header.size = dataSize; // should be region_dtype
		// printf("header, data: %d %d\n", msg.header.size, msg.data.size());
		sendMessage(msg);
	} else {
		printf("data at: %2d %2d %2d is nullptr\n", chunkPosition.x, chunkPosition.y, chunkPosition.z);
	}
}

// Overriden from connection.
void ClientHandler::addToReceivedQueue() {
	serverReceivedQueue->push(OwnedMessage(temp_message, shared_from_this()));
}

void ClientHandler::onMessage(Message msg) {
	switch (msg.header.type) {
		case MsgType::ChunkRequest:
		{
			glm::ivec3 chunkPosition = msg.getData<glm::ivec3>();
			// glm::ivec3 relativeDistance = position - playerChunkPosition;
			
			bool inVicinity = chunkPosition.y >= 0;//!glm::any(glm::bvec3(glm::greaterThanEqual(glm::abs(chunkPosition - playerChunkPosition), vecDrawDistance)));
			printf("Received chunk requested position: %2d, %2d, %2d\n", chunkPosition.x, chunkPosition.y, chunkPosition.z);
			if (
				inVicinity
				// position.x >= -2 && position.x <= 2 &&
				// position.x >= -2 && position.x <= 2 &&
				// position.z >= -2 && position.z <= 2 
			) {
				sendChunk(chunkPosition);
				// loadedChunks.insert(chunkPosition);
			}
		}break;

		case MsgType::PlayerChunkPosition:
		{
			// if (!firstPosition) {
			// 	prevPlayerChunkPosition = playerChunkPosition;
			// }
			
			// playerChunkPosition = msg.getData<glm::ivec3>();

			// if (!firstPosition) {
			// 	std::set<glm::ivec3, compareVec3> unloadableChunks;
			// 	for (const glm::ivec3& chunkPosition : loadedChunks) {
			// 		bool canBeUnloaded = !glm::any(glm::bvec3(glm::greaterThanEqual(glm::abs(chunkPosition - playerChunkPosition), vecDrawDistance)));
			// 	}
			// }

			// firstPosition = false;
		}break;

		case MsgType::PlayerPosition:
		{
			playerPosition = msg.getData<glm::vec3>();
			auto [chunkPos, inChunk] = toChunkCoords(playerPosition, TerrainConfig::ChunkSize);
			playerChunkPosition = chunkPos;
			// printf("player changed position: %2.2f %2.2f %2.2f \n", playerPosition.x, playerPosition.y, playerPosition.z);
		}break;

		case MsgType::BlockChange:
		{
			ChangedBlock block {msg.getData<ChangedBlock>()};
			printf("block changed at: %2d %2d %2d, pos: %2d %2d %2d \n", 
				block.position.chunk.x,
				block.position.chunk.y,
				block.position.chunk.z,
				block.position.block.x,
				block.position.block.y,
				block.position.block.z
				);
		}break;
	}
	// printf("message type: %d\n", msg.header.type);
}

ClientHandler::~ClientHandler() {
	printf("seeya\n");
}