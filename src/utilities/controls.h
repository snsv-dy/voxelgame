#ifndef CONTROLS_H
#define CONTROLS_H

#include <cstdio>
#include "../terrain/WorldLoader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"

enum class BlockSide{
	FRONT,
	TOP,
	RIGHT,
	BOTTOM,
	LEFT,
	BACK,
	NONE
};

struct ControlsStruct{
	double last_mx, last_my;
	Camera &kamera;
	
	int screen_width;
	int screen_height;
	
	const double mouse_speed;
	glm::ivec3 cursor_pos;
	glm::ivec3 prev_cursor_pos;
	
	WorldLoader *world_loader;
};

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, int button, int action, int mods);
	
#endif