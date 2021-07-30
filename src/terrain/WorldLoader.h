#ifndef WORLDLOADER_H
#define WORLDLOADER_H

#include <condition_variable>
#include <map>
#include <set>
#include <unordered_map>
#include <cmath> // for sweep
#include "worldProvider.hpp"
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
	std::set<glm::ivec3, compareVec3> chunks_to_update; // Chunks that need to have vertices (re)generated.
	std::set<glm::ivec3, compareVec3> prepared_chunks; // Chunks that need to have data sent to buffer by drawing thread (Thread in which opengl was initialized).
	std::set<glm::ivec3, compareVec3> disposable_chunks; // Chunks that need to have thier mesh buffers disposed.
	std::mutex disposableChunksMutex;
//	std::set<glm::ivec3, compareVec> lights_to_propagate;
//	std::map<glm::ivec3, Chunk*, compareVec3> chunksToDraw;
	glm::mat4 *projection;
	glm::mat4 *view;
	unsigned int textures;
	worldProvider provider;
//	Lighter lighter;
	
	bool first = true;
	
	int highest_y = 0; // should be lowest int?
	region_dtype ref0 = 0;
	std::set<glm::ivec3, compareVec3> light_needed;
	std::list<ChangedBlock> blocks_changed;
public:

	//
	// MOVE TO PRIVATE
	glm::ivec3 last_camera_pos {0.0f, 0.0f, 0.0f};
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//

	WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params);
	
	const int radius = 2;
	const int unloadRadius = (radius * 2);
	const int unloadRadiusSquared = unloadRadius*unloadRadius;
	const int chunkSize = TerrainConfig::ChunkSize;
	
	std::mutex updateMutex;
	std::mutex prepareSetMutex;
	std::condition_variable updateNotifier;
	bool notified = true;
	glm::ivec3 cur_camera_pos {0.0f, 0.0f, 0.0f};

	void checkForUpdate(glm::vec3 cameraPos);
	void update(glm::ivec3 change);
	void disposeChunks();
	void loadChunk(const glm::ivec3& pos, std::set<glm::ivec3, compareVec3> *light_needed=nullptr);
	std::set<glm::ivec3, compareVec3> getUnlitColumns();
	void prepareGeometry();
	void updateGeometry();
	std::list<ChangedBlock> getChangedBlocks();
	void addUpdatedChunks(std::set<glm::ivec3, compareVec3> updatedChunks);
	
	region_dtype valueAt(int x, int y, int z);
	void draw(glm::vec3 cameraPos, const glm::mat4& view);
	std::tuple<glm::ivec3, glm::ivec3, region_dtype> collideRay(const glm::vec3& origin, const glm::vec3& direction, const int& range);
	void updateTerrain(const int& block_type, const glm::ivec3 &pos, BlockAction action);
	std::pair<Chunk&, bool> getChunk(glm::ivec3 position);
	std::pair<region_dtype&, bool> getBlock(const block_position& pos);
	std::pair<region_dtype&, bool> getBlock(const glm::vec3& pos);
	
	glm::vec3 collideAABB(const AABB& aabb, glm::vec3& velocity, glm::vec3 dx);
	float fastRay(glm::vec3 origin, glm::vec3 direction, float maxt, glm::vec3& penetration, int& collision_axis, glm::ivec3& hit);
	float fastAABB(const AABB& origin, glm::vec3 direction, float maxt, int& collision_axis);
	
	// sweeep stuff
};

#endif // WORLDLOADER_H

// for sweep only
int trailEdgeToInt(int lead, int step);
int leadEdgeToInt(int lead, int step);
int stepForward(float tNext[3], float& t, int leading_i[3], float trailing[3], int trailing_i[3], int step[3], float tDelta[3], float normed[3]);