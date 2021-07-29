#ifndef MESH_H
#define MESH_H

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include "../utilities/basic_util.h"

#include <glm/glm.hpp>
#include <vector>


class Mesh
{
private:
	unsigned int vertices_count = 0; // above array size
	
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int VBO_INT = 0;
	
	bool buffers_set = false;
	
	unsigned int model_location = 0;
public:

	glm::mat4 model = glm::mat4(1.0f);
	
	void setModelLocation(unsigned int model_loc);
	void initVao(std::vector<float>& data, std::vector<unsigned int>& int_data);
	void updateVao(std::vector<float>& data, std::vector<unsigned int>& int_data);
	void draw();
	std::tuple<unsigned int, unsigned int, unsigned int> getBuffers();
	
	Mesh() =default;
	Mesh& operator=(Mesh&& mesh);
	Mesh(Mesh&& mesh) =delete;
	Mesh& operator=(Mesh& mesh) = delete;
	~Mesh();
	
};

#endif // MESH_H
