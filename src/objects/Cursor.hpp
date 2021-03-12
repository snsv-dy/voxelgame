#ifndef CURSOR_H
#define CURSOR_H

#include <vector>
#include <glm/glm.hpp>
#include "../utilities/basic_util.h"
#include "Mesh.h"

class Cursor
{
private:
	void init();
	shaderParams shader;
	unsigned int textures;
	
	Mesh mesh;
	
public:
	int t;
	glm::ivec3& position;
	
	Cursor(glm::ivec3& pos);// : position(7);
	
	void draw(const glm::mat4& projection, const glm::mat4* view);
	
	~Cursor();
};

#endif // CURSOR_H
