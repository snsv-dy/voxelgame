#ifndef WORLDLOADER_H
#define WORLDLOADER_H

#include <condition_variable>
#include <map>
#include <set>
#include <unordered_map>
#include <cmath> // for sweep
#include "worldProvider.hpp"
#include "LocalWorldProvider.hpp"
#include "RemoteWorldProvider.hpp"
#include "Chunk.h"
#include "../objects/Player.hpp"
#include "../objects/AABB.hpp"
//#include "Lighter.hpp"
//#include "Mesh.h"
//#include "../vec3Comp.h"
#include <glm/gtx/norm.hpp>
#include <asio.hpp>
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

class WorldLoader
{
	struct shaderParams shParams;
	std::map<glm::ivec3, Chunk, compareVec3> chunks;
	std::set<glm::ivec3, compareVec3> chunks_to_update; // Chunks that need to have vertices (re)generated.
	std::set<glm::ivec3, compareVec3> prepared_chunks; // Chunks that need to have data sent to buffer by drawing thread (Thread in which opengl was initialized).
	std::set<glm::ivec3, compareVec3> disposable_chunks; // Chunks that need to have thier mesh buffers disposed.
	std::mutex disposableChunksMutex;
	
	glm::mat4 *projection;
	glm::mat4 *view;
	unsigned int textures;
	std::shared_ptr<WorldProvider> provider;
	
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

	WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params, std::shared_ptr<WorldProvider> provider);
	WorldLoader(std::shared_ptr<WorldProvider> provider);
	
	const int radius = 2;
	const int unloadRadius = (radius * 2);
	const int unloadRadiusSquared = unloadRadius*unloadRadius;
	const int chunkSize = TerrainConfig::ChunkSize;
	
	std::mutex updateMutex;
	std::mutex prepareSetMutex;
	std::condition_variable updateNotifier;
	bool notified = false;
	glm::ivec3 cur_camera_pos {0.0f, 0.0f, 0.0f};

	void checkForUpdate(glm::vec3 cameraPos);
	void update(glm::ivec3 change);
	void disposeChunks();
	bool loadChunk(const glm::ivec3& pos);
	void removeChunk(const glm::ivec3& pos);
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
	// ~WorldLoader();
	// sweeep stuff

	// modifications for server
	// std::list<std::shared_ptr<ClientHandler>> players;
	// void addPlayer(const std::shared_ptr<ClientHandler>& player);
	// void removePlayer(const std::shared_ptr<ClientHandler>& player);
};

#endif // WORLDLOADER_H