#include "AABB.hpp"

AABB::AABB(const glm::vec3& min, const glm::vec3& max) : min {min}, max {max}, _local_min {min}, _local_max {max} {
}

AABB::~AABB()
{
}

void AABB::moveTo(glm::vec3 newPosition) {
	max = _local_max + newPosition;
	min = _local_min + newPosition;
}