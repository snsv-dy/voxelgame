#ifndef __LOCAL_WORLD_PROVIDER_HPP__
#define __LOCAL_WORLD_PROVIDER_HPP__
#include <cstdio>
#include "worldProvider.hpp"

class LocalWorldProvider: public WorldProvider {
	bool firstLoop = true;
	std::map<glm::ivec3, Region, compareVec3> regions;
	Region nullRegion;
	
	inline static const std::string world_path = "../world2";
public:
	static const int chunkSize = TerrainConfig::ChunkSize;
	static constexpr int regionN = Region::reg_size;
	static constexpr int regionSize = regionN * chunkSize; // Region size in single blocks, not in chunks.
	static const int regionHeight = regionN * chunkSize;
	
	int regions_size();
	LocalWorldProvider() {
		printf("elo\n");
	}
	
	// 
	int radius = 2;
	
	int xmoved = 0, zmoved = 0;
	int move_required = Region::reg_size / 2;
	
	glm::ivec3 last_pos = glm::ivec3(0);
	
	void update(glm::ivec3 pos);
	std::tuple<region_dtype*, int, bool> getChunkData(glm::ivec3 position);
	void notifyChange(glm::ivec3 chunkPos);
	void changeBlock(ChangedBlock& block);
	region_dtype valueAt(int x, int y, int z);
	~LocalWorldProvider();

	// Not part of interface.
	//
	const unsigned int unloadDistance = 3;
	// playerPosition is actually position of a chunk in which player is.
	void unloadRegions(std::set<glm::ivec3, compareVec3> playerPositions);
};

#endif