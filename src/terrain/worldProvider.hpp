#ifndef __WORLD_PROVIDER_HPP__
#define __WORLD_PROVIDER_HPP__

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <memory>
#include <tuple>
#include <string>
//#include "../vec3Comp.h"
// #include "../utilities/basic_util.h"
//#include "include/basic_util.h"
#include "Region.h"

// Idea: World provider should only be noftified when loading and removing chunk occured,,
// 		 loading regions from disk shouldn't depend on player position.
// 		 (this might be slower[or faster xd, becouse only necessary chunks/regions are loaded ],
//		  but it simplifies the algorithm).
class WorldProvider {
	// bool firstLoop = true;
	// std::map<glm::ivec3, Region, compareVec3> regions;
	// Region nullRegion;
	
	// inline static const std::string world_path = "../world2";
public:
	// static const int chunkSize = TerrainConfig::ChunkSize;
	// static constexpr int regionN = Region::reg_size;
	// static constexpr int regionSize = regionN * chunkSize; // Region size in single blocks, not in chunks.
	// static const int regionHeight = regionN * chunkSize;
	
	virtual int regions_size() = 0;//{ return regions.size(); };
	WorldProvider() {};
	
	// int radius = 2;
	
	// int xmoved = 0, zmoved = 0;
	// int move_required = Region::reg_size / 2;
	
	// glm::ivec3 last_pos = glm::ivec3(0);
	
	virtual std::set<glm::ivec3, compareVec3> getNewChunks() {return {}; }; // For remote
	virtual bool newChunksAvailable() { return false; };
	virtual void update(glm::ivec3 pos) = 0;
	virtual std::tuple<region_dtype*, int, bool> getChunkData(glm::ivec3 position) = 0;
	virtual void notifyChange(glm::ivec3 chunkPos) = 0;
	virtual region_dtype valueAt(int x, int y, int z) = 0;//{ return BLOCK_NOT_FOUND; };
	virtual ~WorldProvider() {};
};
#endif