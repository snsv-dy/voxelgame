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
	
	controls->kamera.rotateView(dx * mspeed, dy * mspeed);
	
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
		
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		controls->kamera.walk(controls->kamera.FRONT);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		controls->kamera.walk(controls->kamera.BACK);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		controls->kamera.walk(controls->kamera.LEFT);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		controls->kamera.walk(controls->kamera.RIGHT);
		
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		controls->kamera.moveVertical(controls->kamera.UP);
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		controls->kamera.moveVertical(controls->kamera.DOWN);
		
	return (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) | (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) * 2;
}


void mouse_callback(GLFWwindow* window, int button, int action, int mods){
	ControlsStruct* controls = (ControlsStruct*)glfwGetWindowUserPointer(window);
	
	if(action == GLFW_PRESS){
//		mouse_down = button == GLFW_MOUSE_BUTTON_LEFT;
//		mouse_right_down = button == GLFW_MOUSE_BUTTON_RIGHT;
		if(controls->world_loader != nullptr){
			BlockAction action = BlockAction::PLACE;
			
			if(button == GLFW_MOUSE_BUTTON_LEFT)
				action = BlockAction::DESTROY;
			
			controls->world_loader->updateTerrain(controls->cursor_pos, action);
		}
		
	}else if(action == GLFW_RELEASE){
//		mouse_down = false;
//		mouse_right_down = false;
	}

}