#ifndef _CHUNK_H_
#define _CHUNK_H_

#include <stdio.h>
#include <vector>
#include <memory>

#include "TerrainConfig.hpp"
#include "worldProvider.h"
#include "../objects/Mesh.h"
#include <glm/glm.hpp>

class WorldLoader;

enum class LightType {
	Sun,
	Block
};

enum class Direction {
	LEFT,
	RIGHT,
	FRONT, 	// Front is end of the array (z = 15)
	BACK	// Back is beginning of the array (z = 0)
};

class Chunk{
private:
	
	static constexpr glm::vec2 planeTexcoords[4] = {
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};
	
	glm::ivec3 gridOffset = glm::ivec3(0); // Change name if possible.
	bool fullOrEmpty = false; // indicates if the chunk is full or empty, so that we don't have to loop 6 x size^2 to find out there aren't any quads
	bool doNotDraw = false;
	
	region_dtype *data = nullptr;
	int data_offset = 0; // offset in data pointer, that indicates start of this chunk's data.
	
	std::unique_ptr<Mesh> mesh = nullptr;
	std::tuple<std::vector<float>, std::vector<unsigned int>> getShaderData();
	
	glm::ivec3 worldOffset = glm::ivec3(0);
	WorldLoader *parentLoader = nullptr;
	void init_data();
	
	
public:
	void update_data();
	
//	glm::ivec3 getOffset(){
//		return this->gridOffset;
//	}

//	static const int chunkSize = 16;
	static const int size = TerrainConfig::ChunkSize;
	static constexpr int size_cubed = size * size * size;
	static constexpr int size_squared = size * size;
	
//	char data[chunkSize][chunkSize][chunkSize];
	
	std::array<region_dtype, size_squared> getBottomLayer();
	std::list<propagateParam> sunlightPass(int mask[size][size], bool& allDark);
	
//	Mesh mesh;
	
	Chunk() =default;
//	Chunk(glm::mat4 *projection, glm::mat4 *view, unsigned int textureId, shaderParams &shParams, glm::ivec3 offset, WorldLoader *loaderPtr, std::shared_ptr<char[]> data, int data_offset);
	Chunk(const unsigned int& model_location, glm::ivec3 offset, WorldLoader *loaderPtr, region_dtype* data, int data_offset);
	Chunk& operator=(Chunk&& t);
	
	Chunk(Chunk const &) =delete;
	Chunk& operator=(Chunk const &) =delete;
	
	void draw();
	region_dtype valueAt(const glm::ivec3& pos);
	void changeBlock(const int& block_type, const glm::ivec3& data_pos, BlockAction& action);
	
	std::list<propagateParam> getEmmitedLightFromSide(Direction side); // Returns list with lights to propagate outside of the chunk in the direction passed by param.
	
	std::array<region_dtype, size * size> getSide(Direction side);
	std::array<region_dtype, size * size> getSide(int direction, int side);
	
	std::tuple<region_dtype*, int> giveData();
	
	~Chunk();
};

#endif