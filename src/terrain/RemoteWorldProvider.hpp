#ifndef __REMOTE_WORLD_PROVIDER__
#define __REMOTE_WORLD_PROVIDER__

// #include <map>
// #include <glm/glm.hpp>
// #include <string>
// #include <fstream>
// #include <memory>
// #include <tuple>
// #include <string>
// //#include "../vec3Comp.h"
// #include "../utilities/basic_util.h"
// //#include "include/basic_util.h"
// #include "Region.h"
// #include <thread>
#include <mutex>
#include "worldProvider.hpp"
#include "../server/netCommon.hpp"

class RemoteWorldProvider: public WorldProvider, public Connection {
	bool firstLoop = true;
	// std::map<glm::ivec3, Region, compareVec3> regions;
	std::map<glm::ivec3, std::vector<region_dtype>, compareVec3> cachedChunks;
	std::mutex cachedChunksMutex;
	Region nullRegion;

	std::set<glm::ivec3, compareVec3> newChunkPositions;
	std::mutex newChunksMutex;
	
	std::vector<region_dtype> dummy_data;

	inline static const std::string world_path = "../world2";
	std::thread netThread;
public:
	// bool newChunks = false;
	static const int chunkSize = TerrainConfig::ChunkSize;
	static constexpr int regionN = Region::reg_size;
	static constexpr int regionSize = regionN * chunkSize; // Region size in single blocks, not in chunks.
	static const int regionHeight = regionN * chunkSize;
	
	int regions_size();
	RemoteWorldProvider(asio::io_context& context, asio::ip::tcp::socket socket, asio::ip::tcp::resolver::results_type endpoints);
	void initData();

	// Overrided?
	void onMessage(Message& msg);
	void sendChunkRequest(glm::ivec3 chunkPosition);
	void sendPlayerPosition(glm::vec3 playerPosition);
	
	int radius = 2;
	
	int xmoved = 0, zmoved = 0;
	int move_required = Region::reg_size / 2;
	
	glm::ivec3 last_pos = glm::ivec3(0);
	
	std::set<glm::ivec3, compareVec3> getNewChunks();
	bool newChunksAvailable();
	void update(glm::ivec3 pos);
	std::tuple<region_dtype*, int, bool> getChunkData(glm::ivec3 position);
	void unloadChunk(const glm::ivec3& position);
	void notifyChange(glm::ivec3 chunkPos);
	void changeBlock(ChangedBlock& block);
	region_dtype valueAt(int x, int y, int z);
	~RemoteWorldProvider();
};

#endif