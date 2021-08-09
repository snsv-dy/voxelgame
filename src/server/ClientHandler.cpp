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
		printf("	Sending chunk size: %d %d\n", t_chunk.size(), msg.data.size());
		memcpy(msg.data.data(), &chunkPosition, sizeof(glm::ivec3));
		memcpy(msg.data.data() + sizeof(glm::ivec3), data + offset, chunkDataSize);

		msg.header.type = MsgType::Chunk;
		msg.header.size = dataSize; // should be region_dtype
		printf("header, data: %d %d\n", msg.header.size, msg.data.size());
		sendMessage(msg);
	}
}

// Overriden from connection.
void ClientHandler::addToReceivedQueue() {
	serverReceivedQueue->push(OwnedMessage(temp_message, shared_from_this()));
}

void ClientHandler::onMessage(Message msg) {
	switch (msg.header.type) {
		case MsgType::ChunkRequest:
			glm::ivec3 position = msg.getData<glm::ivec3>();
			printf("Received chunk requested position: %2d, %2d, %2d\n", position.x, position.y, position.z);
			if (
				position.x >= -2 && position.x <= 2 &&
				position.x >= -2 && position.x <= 2 &&
				position.z >= -2 && position.z <= 2 
			) {
				sendChunk(position);
			}
		break;
	}
	// printf("message type: %d\n", msg.header.type);
}

ClientHandler::~ClientHandler() {
	printf("seeya\n");
}