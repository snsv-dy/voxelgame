#include "RemoteWorldProvider.hpp"

int regions_size();
RemoteWorldProvider::RemoteWorldProvider(asio::ip::tcp::socket socket, asio::ip::tcp::resolver::results_type endpoints): Connection(std::move(socket)) {
	dummy_data = std::vector<region_dtype>(chunkSize * chunkSize * chunkSize);
	initData();
	cachedChunks[glm::ivec3(0, 0, 0)] = std::move(dummy_data);
	this->connectToServer(endpoints);
	// sendChunkRequest(glm::ivec3(1, 0, 0));
}

void RemoteWorldProvider::initData() {
	for (int z = 0; z < chunkSize; z++) {
		for (int y = 0; y < chunkSize; y++) {
			for (int x = 0; x < chunkSize; x++) {
				dummy_data[z * chunkSize * chunkSize + y * chunkSize + x] = 2 * (y < 8) | 0x0f00;
			}
		}
	}
}

void RemoteWorldProvider::update(glm::ivec3 pos) {
	// printf("provider update\n");
	// Process incoming messages
	while (!receivedQueue.empty()) {
		printf("PROCESSING MESSAGE\n");
		onMessage(receivedQueue.front());
		receivedQueue.pop();
	}

	// printf("PROVIDER UPDATED\n");
}

// Loading regions could be done here
// 	CHANGE NAME
// position *= chunkSize;
// auto [reg_pos, chunk_pos] = toChunkCoords(position, regionSize);

// if(auto reg_it = regions.find(reg_pos); reg_it != regions.end()){
// 	Region& region = reg_it->second;
// 	auto [offset, generated] = region.getChunkOffset(position);
// 	return {region.getData(), offset, generated};
// }

// return {nullptr, 0, false};

std::tuple<region_dtype*, int, bool> RemoteWorldProvider::getChunkData(glm::ivec3 position) {
	std::scoped_lock lock(cachedChunksMutex);

	// printf("getting: %2d %2d %2d \n", position.x, position.y, position.z);

	if(auto ch_it = cachedChunks.find(position); ch_it != cachedChunks.end()) {
		return {ch_it->second.data(), 0, false};
	}

	if (
		position.x >= -2 && position.x <= 2 &&
		position.y >= -2 && position.y <= 2 &&
		position.z >= -2 && position.z <= 2 
	) {
		sendChunkRequest(position);
	}

	return {nullptr, 0, false};
}

// bool RemoteWorldProvider::newChunks() {
bool RemoteWorldProvider::getNewChunks() {
	if (newChunks) {
		newChunks = false;
		return true;
	}

	return false;
}

void RemoteWorldProvider::onMessage(Message& msg) {
	if (msg.header.type == MsgType::Chunk) {
		printf("chunk just arrived ");
		std::scoped_lock lock(cachedChunksMutex);
		glm::ivec3 position;
		std::vector<region_dtype> data(msg.header.size - sizeof(glm::ivec3));
		memcpy(data.data(), msg.data.data() + sizeof(glm::ivec3), msg.header.size - sizeof(glm::ivec3));
		memcpy(&position, msg.data.data(), sizeof(glm::ivec3));
		printf("position (%2d %2d %2d) \n", position.x, position.y, position.z);

		cachedChunks[position] = data;
		newChunks = true;
	} else {
		printf("message %d just arrived\n", msg.header.type);
	}
}

void RemoteWorldProvider::sendChunkRequest(glm::ivec3 chunkPosition) {
	Message msg;
	msg.setData(chunkPosition);

	msg.header.type = MsgType::ChunkRequest;
	msg.header.size = msg.data.size();

	sendMessage(msg);
}

void RemoteWorldProvider::notifyChange(glm::ivec3 chunkPos) {

}

#define IN(x, range) ((x) >= 0 && (x) <= range)

region_dtype RemoteWorldProvider::valueAt(int x, int y, int z) {

	if (IN(x, 16) && IN(y, 16) && IN(z, 16)) {
		return dummy_data[z * chunkSize * chunkSize + y * chunkSize + x];
	}
	return 0;
}

RemoteWorldProvider::~RemoteWorldProvider() {
	printf("huh?\n");
}


int RemoteWorldProvider::regions_size() {
	return 1;
}