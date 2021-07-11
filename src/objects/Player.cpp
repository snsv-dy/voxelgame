#include "Player.hpp"

Player::Player(AABB aabb): aabb{aabb} {
	
}

Player::~Player() {
}

void Player::moveRequest(moveDirection direction) {
	switch (direction) {
		case moveDirection::FRONT: velocity += movementSpeed * orientation; break;
		case moveDirection::BACK: velocity -= movementSpeed * orientation; break;
		case moveDirection::LEFT: velocity -= movementSpeed * glm::normalize(glm::cross(orientation, up)); break;
		case moveDirection::RIGHT: velocity += movementSpeed * glm::normalize(glm::cross(orientation, up)); break;
		
		case moveDirection::UP: velocity.y += movementSpeed; break;
		case moveDirection::DOWN: velocity.y -= movementSpeed; break;
	}
}


void Player::turn(float x, float y) {
	yaw += x * mouseSensitivity;
	pitch += -y * mouseSensitivity;
	
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
	
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	orientation = glm::normalize(direction);
}

glm::mat4& Player::getView() {
	return view;
}
	
void Player::updateView() {
	positionFromHead = position + headPosition;
	view = glm::lookAt(positionFromHead, positionFromHead + orientation, up);
}

glm::vec3 Player::nextPosition() {
	return position + velocity;
}

void Player::applyMove(bool correct, glm::vec3 maxAllowed) {
	if( velocity.x != 0 || velocity.y != 0 || velocity.z != 0) {
//		if (correct) {
//			velocity = maxAllowed;
//		}
//		position += velocity;
		setPosition(position + velocity);
		velocity.x = velocity.y = velocity.z = 0;
	}
}

void Player::resetVelocity(int part) {
	if (part & 1) {
		velocity.x = 0.0f;
	}
	
	if (part & 2) {
		velocity.y = 0.0f;
	}
	
	if (part & 4) {
		velocity.z = 0.0f;
	}
}


const glm::vec3 Player::getPosition() {
	return position;
}

void Player::setPosition(glm::vec3 newPosition) {
	position = newPosition;
	aabb.moveTo(newPosition);
//	printf("AABB min: %2.2f, %2.2f, %2.2f, AABB max: %2.2f, %2.2f, %2.2f\n", aabb.min.x, aabb.min.y, aabb.min.z, aabb.max.x, aabb.max.y, aabb.max.z);
}