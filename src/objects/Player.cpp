#include "Player.hpp"

Player::Player(AABB aabb): aabb{aabb} {
	
}

Player::~Player() {
}

void Player::moveRequest(moveDirection direction) {
	switch (direction) {
		case moveDirection::FRONT: force += movingForce * orientation; break;
		case moveDirection::BACK: force -= movingForce * orientation; break;
		case moveDirection::LEFT: force -= movingForce * glm::normalize(glm::cross(orientation, up)); break;
		case moveDirection::RIGHT: force += movingForce * glm::normalize(glm::cross(orientation, up)); break;
		
		case moveDirection::UP: 
			{
				force.y += movingForce;
//				if (!jumped) {
//					force.y += jumpForce;
//					jumped = true;
//				}
			}break;
		case moveDirection::DOWN: force.y -= movingForce; break;
//		case moveDirection::FRONT: velocity += movementSpeed * orientation; break;
//		case moveDirection::BACK: velocity -= movementSpeed * orientation; break;
//		case moveDirection::LEFT: velocity -= movementSpeed * glm::normalize(glm::cross(orientation, up)); break;
//		case moveDirection::RIGHT: velocity += movementSpeed * glm::normalize(glm::cross(orientation, up)); break;
//		
//		case moveDirection::UP: velocity.y += movementSpeed; break;
//		case moveDirection::DOWN: velocity.y -= movementSpeed; break;
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

const glm::vec3 Player::getPosition() {
	return position;
}

void Player::setPosition(glm::vec3 newPosition) {
	position = newPosition;
	aabb.moveTo(newPosition);
}

glm::vec3 Player::updatePhysics(const float& dt) {
	glm::vec3 a = force / mass;
	const float gravity = 1.0f;
	a.y -= gravity / mass;
	
	glm::vec3 dv = a * dt;
	
	for (int i = 0; i < 3; i++) {
		if (abs(velocity[i]) < maxVelocity) {
			velocity[i] += dv[i];
		} else if(i == 1 && abs(velocity[i]) < maxFalling) { // eh
			velocity[i] += dv[i];
		}
	}
		
	// friction
	const float frictionValue = 0.6f;
	float fMax = frictionValue * dt;
	glm::vec3 friction = -velocity;
	float frictionMag = glm::length(friction);
	if (frictionMag > fMax) {
		friction *= fMax / frictionMag;
		velocity += friction;
	} else {
		velocity.x = velocity.y = velocity.z = 0.0f;
	}
	force = glm::vec3(0.0f);
	
	int oldTouching[3];
	for(int i = 0; i < 3; i++) {
		oldTouching[i] = touching[i];
		touching[i] = 0;
	}
	
	int movingDirection[3];
	for(int ax = 0; ax < 3; ax++) {
		movingDirection[ax] = 0;
		if (velocity[ax] > 0) {
			movingDirection[ax] = 1;
		} else if (velocity[ax] < 0) {
			movingDirection[ax] = -1;
		}
	}
	
	return velocity * dt;
}

void Player::applyTranslation(const glm::vec3& dx) {
	this->setPosition(position + dx);
}