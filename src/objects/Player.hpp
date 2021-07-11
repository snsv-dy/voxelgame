#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "AABB.hpp"

enum class moveDirection {
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Player {
private:
	const float movementSpeed = 0.1f;
	const float mouseSensitivity = 0.05f;
	const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	float yaw = -90.0f;
	float pitch = 0.0f;
	glm::mat4 view = glm::lookAt(position, position + orientation, up);
	glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
public:

	const glm::vec3 headPosition = glm::vec3(0.0f, 1.8f, 0.0f);
	int touching[3] {0}; // direction in which player is colliding with ground.
//	glm::vec3 _impulses;

	AABB aabb;
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 positionFromHead = position + headPosition;
	Player(AABB aabb);
	~Player();
	
	void moveRequest(enum moveDirection direction);
	void turn(float x, float y);
	glm::vec3 nextPosition();
	void applyMove(bool correct, glm::vec3 maxAllowed=glm::vec3(0.0f));
	void resetVelocity(int part);
	
	void updateView();
	glm::mat4& getView();
	
	const glm::vec3 getPosition();
	void setPosition(glm::vec3 newPosition);
};

#endif // PLAYER_H
