#ifndef _BASIC_UTIL_
#define _BASIC_UTIL_

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <tuple>
#include <string>

#include "../terrain/TerrainConfig.hpp"
	
enum class BlockAction{
	DESTROY,
	PLACE
};

enum class ShaderType{
	VERTEX,
	FRAGMENT
};

struct shaderParams{
	unsigned int program;
	int projection;
	int view;
	int model;
	
	int light;
};

//unsigned int getShaderFromFile(char *filename, ShaderType type );
unsigned int getShaderFromFile(const std::string& filename, const ShaderType& type );
int getShaderProgram(int vs, int fs);
shaderParams getShaderParams(unsigned int program);
	
unsigned int loadTexture(const std::string& filename, unsigned int& width, unsigned int& height);
unsigned int loadTexture(const std::string& filename);
	
//typedef struct{
//	glm::ivec3 chPos;
//	glm::ivec3 inPos;
//} chunkCoords;

//chunkCoords toChunkCoords(glm::ivec3 cord, int chunkSize);
std::tuple<glm::ivec3, glm::ivec3> toChunkCoords(const glm::ivec3& cord, const int& chunkSize);
std::tuple<int, int> toNonSymetricSpace(const int& value, const int& chunkSize);

inline std::tuple<int, int> toNonSymetricSpaceReal(const float& value, const float& chunkSize);
std::tuple<glm::ivec3, glm::ivec3> toChunkCoordsReal(const glm::vec3& position, const int& chunkSize);

struct compareVec3{
	bool operator()(const glm::ivec3 &a, const glm::ivec3 &b) const;
};

struct compareVec{
	bool operator()(const glm::ivec3 &a, const glm::ivec3 &b) const;
};

struct block_position{
	glm::ivec3 chunk {0};
	glm::ivec3 block {0};
	block_position() {
		// chunk = glm::ivec3
	}

	block_position(glm::ivec3 block, glm::ivec3 chunk): block{block}, chunk{chunk}{
		this->verify_position();
	}
	
	block_position& operator+= (const glm::ivec3& block_offset){
		this->block += block_offset;
		this->verify_position();
		
		return *this;
	}
	
	block_position operator+ (const glm::ivec3& block_offset){
		return block_position(block + block_offset, chunk);
	}
	
	static block_position fromPosition(const glm::vec3 position) {
		auto [chunk_pos, in_chunk_pos] = toChunkCoords(position, TerrainConfig::ChunkSize);
		return block_position(in_chunk_pos, chunk_pos);
	}
	
private:
	void verify_position(){
		for(int i = 0; i < 3; i++){
			auto [b, c] = this->inc_pos(block[i], chunk[i]);
//			printf("%d bc: %d %d\n", i, b, c);
			block[i] = b;
			chunk[i] = c;
		}
	}
	
	std::pair<int, int> inc_pos(int block, int chunk){
		int size = 16;//Chunk::size; // Make some global chunk size reference.
	
		chunk += (block >= size) -(block < 0);
		block &= (size - 1); // ONLY IF SIZE IS POWER OF 2!!! Use expression below in other cases.
//		block = 15 * (block < 0) + block * (block > 0 && block < size);
		
		return {block, chunk};
	}
};

struct ChangedBlock {
	block_position position;
	bool placed;
	region_dtype type;
	region_dtype typeBefore;
};

#endif