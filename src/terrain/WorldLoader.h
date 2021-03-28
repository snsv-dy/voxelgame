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
#include <mutex>

class WorldLoader
{
	struct shaderParams shParams;
	std::map<glm::ivec3, Chunk, compareVec3> chunks;
//	std::map<glm::ivec3, Chunk*, compareVec3> chunksToDraw;
	glm::mat4 *projection;
	glm::mat4 *view;
	unsigned int textures;
	worldProvider provider;
	
	glm::ivec3 last_camera_pos;
	bool first = true;
	
	int highest_y = 0; // should be lowest int?
	
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
	std::list<propagateParam> updateSunlightInColumn(int x, int z);
	std::set<glm::ivec3, compareVec> propagateLight(std::list<propagateParam>& lights);
};

#endif // WORLDLOADER_H
