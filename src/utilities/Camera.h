#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera{
private:
	
	glm::mat4 view;
	glm::mat4 *projection;
	
	glm::vec3 position;
	glm::vec3 up;
	glm::vec3 front;
	float yaw = -90.0f, pitch = 0.0f;
	float sensitivity = 0.05f;
	float movementSpeed = 0.4f;
	
public:
	enum direction{
		FRONT,
		BACK,
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
	
	Camera(float scr_width, float scr_height){
//		this->screen_width = scr_width;
//		this->screen_height = scr_height;
		this->projection = projection;
		
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		position = glm::vec3(0.0f, 25.0f, 0.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);
		
		view = glm::lookAt(position, front, up);
	}
	
	void walk(enum direction dir){
		switch(dir){
			case FRONT: position += movementSpeed * front; break;
			case BACK: position -= movementSpeed * front; break;
			case LEFT: position -= movementSpeed * glm::normalize(glm::cross(front, up)); break;
			case RIGHT: position += movementSpeed * glm::normalize(glm::cross(front, up)); break;
		}
	}
	
	void rotateView(float x, float y){
		yaw += x * sensitivity;
		pitch += -y * sensitivity;
		
		if(pitch > 89.0f)
			pitch = 89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;
			
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);
	}
	
	void updateView(){
		view = glm::lookAt(position, position + front, up);
	}
	
	void moveVertical(enum direction dir){
		if(dir == UP){
			position.y += movementSpeed;
		}else if(dir == DOWN){
			position.y -= movementSpeed;
		}
	}
	
	void zoom(float val){
//		if(base_pos.z + val > 0)
//			base_pos.z += val;
//			
//		this->rotate_xy(0, 0);
	}
	
	
	
	glm::vec3& get_pos(){
		return this->position;
	}
	
	glm::vec3& get_front(){
		return this->front;
	}
	
	glm::mat4 *get_view(){
		return &this->view;
	}
	
};