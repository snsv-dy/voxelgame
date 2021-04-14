#ifndef WORLDLOADER_H
#define WORLDLOADER_H

#include <map>
#include <set>
#include <unordered_map>
#include "worldProvider.h"
#include "Chunk.h"
//#include "Mesh.h"
//#include "../vec3Comp.h"
#include <glm/gtx/norm.hpp>
//#include <mutex>

void inline disperseLight(block_position pos, std::list<propagateParam>& lights, region_dtype light_value);

// used in light propagation
const glm::ivec3 neighbouring_offsets[6] { 
	glm::ivec3(-1, 0, 0),
	glm::ivec3(1, 0, 0),
	glm::ivec3(0, 0, -1),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, -1, 0),
	glm::ivec3(0, 1, 0)
};

class WorldLoader
{
	struct shaderParams shParams;
	std::map<glm::ivec3, Chunk, compareVec3> chunks;
	std::set<glm::ivec3, compareVec> chunks_to_update;
//	std::set<glm::ivec3, compareVec> lights_to_propagate;
//	std::map<glm::ivec3, Chunk*, compareVec3> chunksToDraw;
	glm::mat4 *projection;
	glm::mat4 *view;
	unsigned int textures;
	worldProvider provider;
	
	glm::ivec3 last_camera_pos;
	bool first = true;
	
	int highest_y = 0; // should be lowest int?
	region_dtype ref0 = 0;
	
public:
	WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params);
	
	const int radius = 2;
	const int unloadRadius = (radius * 2);
	const int unloadRadiusSquared = unloadRadius*unloadRadius;
	const int chunkSize = 16;
	
	void update(glm::vec3 cameraPos);
	region_dtype valueAt(int x, int y, int z);
	void draw(glm::vec3 cameraPos);
	std::tuple<glm::ivec3, glm::ivec3, region_dtype> collideRay(const glm::vec3& origin, const glm::vec3& direction, const int& range);
	void updateTerrain(const int& block_type, const glm::ivec3 &pos, BlockAction action);
//	void insertBlock(const glm::ivec3 &pos);
private:
	void litBlock(block_position& pos, int intensity);
	std::pair<region_dtype&, bool> getBlock(const block_position& pos);
	void updateSunlightForBlock(block_position pos, bool placed); // removed means if we are removing a block or placing it.
	void updateBlocklightForBlock(block_position pos, bool placed);
	
	std::list<propagateParam> updateSunlightInColumn(int x, int z);
	std::set<glm::ivec3, compareVec> propagateLight(std::list<propagateParam>& lights, const LightType& type = LightType::Sun);
	std::set<glm::ivec3, compareVec> propagateDark(std::list<propagateParam> darks, const LightType& type = LightType::Sun);
	void updateLightSource(region_dtype& block, block_position& pos, region_dtype intensity, bool placed);
};

#endif // WORLDLOADER_H
