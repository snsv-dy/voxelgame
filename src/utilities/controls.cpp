#include "controls.h"

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos){
	ControlsStruct* controls = (ControlsStruct*)glfwGetWindowUserPointer(window);
	
	if(controls->last_mx == -1 && controls->last_my == -1){
		controls->last_mx = xpos;
		controls->last_my = ypos;
		printf("aaa\n");
//		firstMouse = false;
	}
	
	double dx = xpos - controls->last_mx;
	double dy = ypos - controls->last_my;
	
	const double &mspeed = controls->mouse_speed;
	
	controls->player.turn(dx * mspeed, dy * mspeed);
	
	controls->last_mx = xpos;
	controls->last_my = ypos;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	ControlsStruct* controls = (ControlsStruct*)glfwGetWindowUserPointer(window);
	
	controls->screen_width = width;
	controls->screen_height = height;
	glViewport(0, 0, width, height);
}

//void scroll_callback(GLFWwindow* window, double x, double y){
//	yscroll = y;
//}

int processInput(GLFWwindow* window){
	ControlsStruct* controls = (ControlsStruct*)glfwGetWindowUserPointer(window);
	
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
		
	if(!controls->block_key_pressed){
		controls->block_key_pressed = true;
		if(glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
			controls->placing_block_type = 1;
		}else if(glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
			controls->placing_block_type = 2;
		}else if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
			controls->placing_block_type = 3;
		}else if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
			controls->placing_block_type = 4;
		}
	}else{
		if(glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE){
			controls->block_key_pressed = false;
		}
	}
	
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		controls->player.moveRequest(moveDirection::FRONT);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		controls->player.moveRequest(moveDirection::BACK);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		controls->player.moveRequest(moveDirection::LEFT);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		controls->player.moveRequest(moveDirection::RIGHT);
		
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		controls->player.moveRequest(moveDirection::UP);
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		controls->player.moveRequest(moveDirection::DOWN);
		
	return (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) | (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) * 2;
}


void mouse_callback(GLFWwindow* window, int button, int action, int mods){
	ControlsStruct* controls = (ControlsStruct*)glfwGetWindowUserPointer(window);
	
	if(action == GLFW_PRESS){
//		mouse_down = button == GLFW_MOUSE_BUTTON_LEFT;
//		mouse_right_down = button == GLFW_MOUSE_BUTTON_RIGHT;
		if(controls->world_loader != nullptr){
			if(button == GLFW_MOUSE_BUTTON_LEFT){
				controls->world_loader->updateTerrain(0, controls->cursor_pos, BlockAction::DESTROY);
			}else{
				controls->world_loader->updateTerrain(controls->placing_block_type, controls->prev_cursor_pos, BlockAction::PLACE);
			}
		}
		
	}else if(action == GLFW_RELEASE){
//		mouse_down = false;
//		mouse_right_down = false;
	}

}