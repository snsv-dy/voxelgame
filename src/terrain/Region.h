#ifndef REGION_H
#define REGION_H

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
//#include <queue>
#include <list>
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

typedef unsigned short region_dtype; // Should probably be struct, for easier access to light and block info.
#define block_type(x) ((x) & 0xff)
#define block_light(x) ((x) >> 8)

struct propagateParam{ // change to something with light (will be also useful when propagating light emited by blocks)
	glm::ivec3 position;
	region_dtype light_value;
};

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

class Region{
private:
	std::string fileName = std::string("");
	
	std::vector<glm::ivec3> loaded_chunks;
	char terrain(int x, int y, int z);
	
	// xyz - position of a chunk inside of region used only in generate
	void genChunk(int x, int y, int z);
	region_dtype ref0 = 0;
	
	void generate();
	void load();
	void save();

public:
	inline static const std::string directory = std::string("../world3");
	static const int reg_size = 4;
	static const int chunk_size = 16;
	static const unsigned int region_size = chunk_size * reg_size;//worldProvider::regionSize;
	static const unsigned int data_size = region_size * region_size * region_size;
	
	bool modified = false;
	bool brand_new = false; // Not loaded from disk, generated using generate(), needs to calculate world light. Change name of this
	std::list<propagateParam> pending_lights;
	
	glm::ivec3 position = glm::ivec3(0);
	
	RegionType type = RegionType::NULL_REGION;
	bool removeable = false; // Used in world provider to determine if this region should be unloaded.
	
	Region()=default;
	Region(glm::ivec3 pos);
	
	std::list<propagateParam> calculateSunlight();
	void propagatePendingLight();
	
	region_dtype& valueAt(int x, int y, int z);
	const std::vector<glm::ivec3>& getLoadedChunks();
	int getChunkOffset(glm::ivec3 pos);
	region_dtype* getData();
	
	~Region();
	
private:
	region_dtype data[data_size];
	std::list<propagateParam> propagateLight(std::list<propagateParam>& queue);
	bool is_near_light(int x, int z, int mask[chunk_size][chunk_size]);
	std::list<propagateParam>  calculateSunInChunk(int gx, int gy, int gz, int mask[chunk_size][chunk_size]);
};

#endif // REGION_H
