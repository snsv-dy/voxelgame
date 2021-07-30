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
	
	// const float movingForce = 2.0f;
public:
	glm::vec3 position = glm::vec3(0.0f, 60.0f, 0.0f);
	glm::vec3 force = glm::vec3(0.0f);
	const float maxVelocity = 12.7f;
	const float maxFalling = 55.0f;
	const float movingForce = 2.0f;
	const float jumpForce = movingForce;
	float mass = 1.0f;

	const glm::vec3 headPosition = glm::vec3(0.0f, 1.5f, 0.0f);
	int touching[3] {0}; // direction in which player is colliding with ground.
//	glm::vec3 _impulses;
public:
	bool noclip = false;
	bool jumped = false;

	AABB aabb;
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 positionFromHead = position + headPosition;
	Player(AABB aabb);
	~Player();
	
	// Camera part.
	void updateView();
	glm::mat4& getView();
	
	// Physics part.
	void moveRequest(enum moveDirection direction);
	void turn(float x, float y);
	
	const glm::vec3 getPosition();
	void setPosition(glm::vec3 newPosition);
	
	// Methods below could be part of generalized physics object if any would be needed.
	// Returns dx (translation value).
	glm::vec3 updatePhysics(const float& dt);
	void applyTranslation(const glm::vec3& dx);
};

#endif // PLAYER_H
