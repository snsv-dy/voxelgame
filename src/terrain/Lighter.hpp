#ifndef LIGHTER_HPP
#define LIGHTER_HPP

#include "WorldLoader.h"
#include <list>
#include "Region.h"
#include "Chunk.h" // for Direction

//class WorldLoader;
//struct DirAndSide;

void inline disperseLight(block_position pos, std::list<propagateParam>& lights, region_dtype light_value);

class Lighter
{
	WorldLoader& worldLoader;
	std::list<propagateParam> darks;
	std::list<propagateParam> lights;
	
	std::list<propagateParam> blockDarks;
	std::list<propagateParam> blockLights;
	std::set<glm::ivec3, compareVec3> updatedChunks;
public:
	Lighter(WorldLoader& worldLoader);
//	~Lighter();
	std::list<propagateParam> updateSunlightInColumn(int x, int z);
	std::list<propagateParam> updateLightColumns(std::set<glm::ivec3, compareVec3> columns);
	void updateSunlightForBlock(block_position pos, bool placed);
	void updateBlocklightForBlock(block_position pos, bool placed);
	void updateLightSource(region_dtype& block, block_position& pos, region_dtype intensity, bool placed);
	void updateLight(std::list<ChangedBlock> blocks);
	std::set<glm::ivec3, compareVec3> getUpdatedChunks();
	
	void propagateBlockLight();
	void propagateSunLight();
	std::set<glm::ivec3, compareVec> propagateLight(std::list<propagateParam>& lights, const LightType& type = LightType::Sun);
	std::set<glm::ivec3, compareVec> propagateDark(std::list<propagateParam> darks, const LightType& type = LightType::Sun);
	
	std::list<propagateParam> getLightsToPropagate();
	std::list<propagateParam> getDarksToPropagate();
};

#endif // LIGHTER_HPP
