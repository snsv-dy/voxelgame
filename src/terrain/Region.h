#ifndef REGION_H
#define REGION_H

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <set>
#include <list>
#include "TerrainConfig.hpp"
//#include "vec3Comp.h"
//#incldue "basic_util.h"
#include "../utilities/basic_util.h"
//#include "terrain/Terrain.h"
#include "PerlinNoise.hpp"

// https://stackoverflow.com/questions/13061979/shared-ptr-to-an-array-should-it-be-used
template< typename T >
struct array_deleter{
	void operator ()(T const * p){
		delete[] p;
	}
};

//typedef unsigned short region_dtype; // Should probably be struct, for easier access to light and block info.
#define block_type(x) ((x) & 0xff)
#define block_light(x) ((x) & 0xff00)

#define getSunLight(x) ((x) & 0xf00)
#define getBlockLight(x) ((x) & 0xf000)
#define getBlockType(x) ((x) & 0xff)


struct propagateParam{
	block_position position;
	region_dtype light_value;
	
	propagateParam(block_position position, region_dtype light_value): position{position}, light_value{light_value} {}
};
//struct propagateParam{ // change to something with light (will be also useful when propagating light emited by blocks)
//	glm::ivec3 region_pos;
//	glm::ivec3 position; // In region
//	region_dtype light_value;
//};

enum class RegionType{
	NULL_REGION,
	NORMAL_REGION
};

//Chunk -> Region : get pointer to data array 
//(In region, add chunk coordinates to list of loaded chunks)
//
//if region gets destroyed
//(Pass list of chunks to remove to worldLoader )
//if chunk gets destroyed
//(Remove coordiantes of that chunk from list of loaded chunks)

class Region {
private:
	std::string fileName = std::string("");
	
	// std::vector<glm::ivec3> generatedChunks;
	std::set<glm::ivec3, compareVec3> generatedChunks;
	bool loadedChunks[TerrainConfig::RegionSize][TerrainConfig::RegionSize][TerrainConfig::RegionSize] {};
	int loadedChunksN = 0;
	char terrain(int x, int y, int z);
	
	// xyz - position of a chunk inside of region used only in generate
	void genChunk(int x, int y, int z);
	
	void generate();
	void load();
	void save();
	std::vector<region_dtype> compressData(size_t length, region_dtype mask);
	void expandData(std::vector<region_dtype> compressed, bool overwriteData);

	region_dtype ref0 = 0;
	bool readonly = true; // For server testing. (disables saving regions to disk)
public:
	inline static const std::string directory = std::string("../world3_3");
	static const int reg_size = TerrainConfig::RegionSize;
	static const int chunk_size = TerrainConfig::ChunkSize;
	static const unsigned int region_size = chunk_size * reg_size;//worldProvider::regionSize;
	static const unsigned int data_size = region_size * region_size * region_size;
	
	bool modified = false;
	bool brand_new = false; // Not loaded from disk, generated using generate(), needs to calculate world light. Change name of this
	
	glm::ivec3 position = glm::ivec3(0);
	
	RegionType type = RegionType::NULL_REGION;
	bool removeable = false; // Used in world provider to determine if this region should be unloaded.
	
	Region();
	Region(Region&& temp)=delete;
	Region(glm::ivec3 pos);
	
	region_dtype& valueAt(int x, int y, int z);
//	const std::vector<glm::ivec3>& getLoadedChunks();
	std::pair<int, bool> getChunkOffset(glm::ivec3 pos);
	void setLoadedChunk(const glm::ivec3& position);
	region_dtype* getData();
	int unloadChunk(const glm::ivec3& chunkPos); // chunkPos is local in region position (0 ... RegionSize-1)
												 // returns number of loaded chunks.
	
	~Region();
	
private:
	region_dtype data[data_size];
	std::array<region_dtype, chunk_size * chunk_size> getChunkSide(glm::ivec3 pos, int norm, int side);
};

#endif // REGION_H
