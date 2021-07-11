#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

// Axis aligned bounding box
class AABB
{
	glm::vec3 _local_min; // base
	glm::vec3 _local_max;
public:
	glm::vec3 min; // base
	glm::vec3 max;
	AABB(const glm::vec3& min, const glm::vec3& max);
	~AABB();
	
	void moveTo(glm::vec3 newPosition);

};

#endif // AABB_H
