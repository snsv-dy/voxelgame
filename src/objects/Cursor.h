#ifndef CURSOR_H
#define CURSOR_H

#include <glm/glm.hpp>

class Cursor
{
private:
	void init();
	glm::ivec3& position;
	
public:
	int t;
	
	Cursor(glm::ivec3& pos);// : position(7);
	~Cursor();

};

#endif // CURSOR_H
