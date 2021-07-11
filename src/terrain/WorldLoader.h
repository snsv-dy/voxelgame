#ifndef WORLDLOADER_H
#define WORLDLOADER_H

#include <map>
#include <set>
#include <unordered_map>
#include <cmath> // for sweep
#include "worldProvider.h"
#include "Chunk.h"
#include "../objects/Player.hpp"
#include "../objects/AABB.hpp"
//#include "Lighter.hpp"
//#include "Mesh.h"
//#include "../vec3Comp.h"
#include <glm/gtx/norm.hpp>
//#include <mutex>

// used in light propagation
const glm::ivec3 neighbouring_offsets[6] { 
	glm::ivec3(-1, 0, 0),
	glm::ivec3(1, 0, 0),
	glm::ivec3(0, 0, -1),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, -1, 0),
	glm::ivec3(0, 1, 0)
};
struct ChangedBlock{
	block_position position;
	bool placed;
	region_dtype type;
	region_dtype typeBefore;
};

class WorldLoader
{
	struct shaderParams shParams;
	std::map<glm::ivec3, Chunk, compareVec3> chunks;
	std::set<glm::ivec3, compareVec3> chunks_to_update;
//	std::set<glm::ivec3, compareVec> lights_to_propagate;
//	std::map<glm::ivec3, Chunk*, compareVec3> chunksToDraw;
	glm::mat4 *projection;
	glm::mat4 *view;
	unsigned int textures;
	worldProvider provider;
//	Lighter lighter;
	
	glm::ivec3 last_camera_pos;
	bool first = true;
	
	int highest_y = 0; // should be lowest int?
	region_dtype ref0 = 0;
	std::set<glm::ivec3, compareVec3> light_needed;
	std::list<ChangedBlock> blocks_changed;
public:
	WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params);
	
	const int radius = 2;
	const int unloadRadius = (radius * 2);
	const int unloadRadiusSquared = unloadRadius*unloadRadius;
	const int chunkSize = TerrainConfig::ChunkSize;
	
	void update(glm::vec3 cameraPos);
	void loadChunk(const glm::ivec3& pos, std::set<glm::ivec3, compareVec3> *light_needed=nullptr);
	std::set<glm::ivec3, compareVec3> getUnlitColumns();
	void updateGeometry();
	std::list<ChangedBlock> getChangedBlocks();
	void addUpdatedChunks(std::set<glm::ivec3, compareVec3> updatedChunks);
	
	region_dtype valueAt(int x, int y, int z);
	void draw(glm::vec3 cameraPos, const glm::mat4& view);
	std::tuple<glm::ivec3, glm::ivec3, region_dtype> collideRay(const glm::vec3& origin, const glm::vec3& direction, const int& range);
	void updateTerrain(const int& block_type, const glm::ivec3 &pos, BlockAction action);
	std::pair<Chunk&, bool> getChunk(glm::ivec3 position);
	std::pair<region_dtype&, bool> getBlock(const block_position& pos);
	
	std::pair<bool, glm::vec3> playerIntersects(Player& player);
	float fastRay(glm::vec3 origin, glm::vec3 direction, glm::vec3& penetration);
	
	// sweeep stuff
	float sweep(AABB box, glm::vec3 dir);
	bool checkCollision(int i_axis, int leading_i[3], int trailing_i[3], int step[3]);
};

#endif // WORLDLOADER_H

// for sweep only
int trailEdgeToInt(int lead, int step);
int leadEdgeToInt(int lead, int step);
int stepForward(float tNext[3], float& t, int leading_i[3], float trailing[3], int trailing_i[3], int step[3], float tDelta[3], float normed[3]);