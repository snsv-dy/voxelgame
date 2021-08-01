#ifndef _WORLD_PROVIDER_H_
#define _WORLD_PROVIDER_H_

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <memory>
#include <tuple>
#include <string>
//#include "../vec3Comp.h"
#include "../utilities/basic_util.h"
//#include "include/basic_util.h"
#include "Region.h"


class WorldProvider {
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
	WorldProvider() {}
	
	int radius = 2;
	
	int xmoved = 0, zmoved = 0;
	int move_required = Region::reg_size / 2;
	
	glm::ivec3 last_pos = glm::ivec3(0);
	
	void update(glm::ivec3 pos);
	std::pair<region_dtype*, int> getChunkData(glm::ivec3 position);
	region_dtype valueAt(int x, int y, int z);
};

#endif