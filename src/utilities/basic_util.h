#ifndef _BASIC_UTIL_
#define _BASIC_UTIL_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <tuple>

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

struct compareVec3{
	bool operator()(const glm::ivec3 &a, const glm::ivec3 &b) const;
};

struct compareVec{
	bool operator()(const glm::ivec3 &a, const glm::ivec3 &b) const;
};

#endif