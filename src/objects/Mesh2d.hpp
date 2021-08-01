#ifndef _MESH2D_H_
#define _MESH2D_H_

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

// #include "../utilities/basic_util.h"

#include <glm/glm.hpp>
#include <vector>
#include <cstdio>


class Mesh2d {
private:
	unsigned int vertices_count = 0; // above array size
	
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	
	bool buffers_set = false;
public:
	void initVao(std::vector<float>& data);
	void updateVao(std::vector<float>& data);
	void draw();
	
	Mesh2d() =default;
	Mesh2d& operator=(Mesh2d&& mesh);
	Mesh2d(Mesh2d&& mesh) =delete;
	Mesh2d& operator=(Mesh2d& mesh) = delete;
	~Mesh2d();
};

#endif // _MESH2D_H_
