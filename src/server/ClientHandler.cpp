#include "ClientHandler.hpp"

ClientHandler::ClientHandler(asio::io_context& context, tcp::socket client, uint32_t id, TsQueue<OwnedMessage>* serverQueue, LocalWorldProvider& worldProvider): id{id}, worldProvider{worldProvider}, Connection{context, std::move(client)}, serverReceivedQueue{serverQueue}{
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

bool ClientHandler::chunkInRange(glm::ivec3 chunkPosition) {
	glm::ivec3 absdiff = glm::abs(this->chunkPosition - chunkPosition);
	// int distance = absdiff[0] + absdiff[1] + absdiff[2];
	// int distance = // branchless max
		// absdiff[0] * (absdiff[0] >= absdiff[1] && absdiff[0] >= absdiff[2]) + 
		// absdiff[1] * (absdiff[1] >= absdiff[0] && absdiff[1] >= absdiff[2]) + 
		// absdiff[2] * (absdiff[2] >= absdiff[1] && absdiff[2] >= absdiff[0]);
	int distance = absdiff[0] >= absdiff[1] ? 
					absdiff[0] >= absdiff[2] ? absdiff[0] : absdiff[2] :
					absdiff[1] >= absdiff[2] ? absdiff[1] : absdiff[2];
	return distance <= renderDistance;
	// return chunkPosition.y >= 0;
}

void ClientHandler::unloadChunks() {
	if (chunkPosition != lastChunkPosition) {
		int unloaded = 0;
		bool flag = false;
		unsigned long preLoadedSize = loadedChunks.size();
		for (std::set<glm::ivec3>::iterator it = loadedChunks.begin(); it != loadedChunks.end(); ) {
			if (!chunkInRange(*it)) {
				flag = true;
				unloaded++;
				chunksToUnload.insert(*it);
				requestedChunks.erase(*it);
				it = loadedChunks.erase(it);
			} else {
				it++;
			}
		}

		// printf("loaded diff: %2lu, unloaded: %2lu\n", preLoadedSize - loadedChunks.size(), unloaded);

		// if (unloaded != chunksToUnload.size()) {
		// 	printf("chunkPosition: %2d %2d %2d, lastChunkPosition: %2d %2d %2d\n", chunkPosition.x, chunkPosition.y, chunkPosition.z, lastChunkPosition.x, lastChunkPosition.y, lastChunkPosition.z);
		// 	printf("unloaded int (%2d), set (%2d) size diff\n", unloaded, chunksToUnload.size());
		// }

		printf("Loaded chunks (clien): %d\n", loadedChunks.size());
		// printf("")
		lastChunkPosition = chunkPosition;
	}
}

void ClientHandler::onMessage(Message msg) {
	switch (msg.header.type) {
		case MsgType::PlayerPosition:
		{
			playerPosition = msg.getData<glm::vec3>();
			playerPosition.y = 5;
			// IPosition = glm::floor(playerPosition);

			auto [chunkPos, inChunk] = toChunkCoords(playerPosition, TerrainConfig::ChunkSize);
			chunkPosition = chunkPos;
			
			// glm::ivec3 amount_moved = IPosition - lastIPosition;
			// lastIPosition = IPosition;
			// moved += amount_moved;

			// bool loadChunks = false;
			// if (first) {
			// 	printf("first\n");
			// 	amount_moved.x = renderDistance * 2 * TerrainConfig::ChunkSize;
			// 	moved.x = amount_moved.x;
			// 	loadChunks = true;
			// 	first = false;
			// }

			// for (int axis = 0; axis < 3; axis++) {
			// 	if (abs(moved[axis]) >= moveRequired) {
			// 		loadChunks = true;
			// 	}
			// }

			// if (loadChunks) {
			// 	auto [chunkPos, inChunk] = toChunkCoords(playerPosition, TerrainConfig::ChunkSize);
			// 	for (int axis = 0; axis < 3; axis++) {
			// 		if (abs(moved[axis]) >= moveRequired) {
			// 			int back = amount_moved[axis] > 0 ? 1 : -1;
						
			// 			int norm = axis;
			// 			int tan = (norm + 2) % 3;
			// 			int biTan = (norm + 1) % 3;
						
			// 			glm::ivec3 cursor(0);
						
			// 			int updateRange = renderDistance - abs(moved[axis]) / TerrainConfig::ChunkSize;
			// 			printf("update range: %d/%d, axis: %d\n", updateRange, renderDistance, axis);

			// 			for (cursor[norm] = updateRange; cursor[norm] < renderDistance; cursor[norm]++) {
			// 			// for (cursor[norm] = -renderDistance; cursor[norm] < renderDistance; cursor[norm]++) {
			// 						// printf("moshi moshi?\n");
			// 				for (cursor[biTan] = -renderDistance; cursor[biTan] <= renderDistance; cursor[biTan]++) {
			// 					for (cursor[tan] = -renderDistance; cursor[tan] <= renderDistance; cursor[tan]++) {
			// 						glm::ivec3 pos = chunkPos + cursor * back;
									
			// 						if (pos.y >= 0 && loadedChunks.find(pos) == loadedChunks.end()) {
			// 							requestedChunks.insert(pos);
			// 							// printf("loading chunk: %2d %2d %2d\n", pos.x, pos.y, pos.z);
			// 							// Request chunk here
			// 						}
			// 					}
			// 				}
			// 			}
			// 			moved[axis] = 0;
			// 		}
			// 	}
			// 	printf("koniec\n");
			// }

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