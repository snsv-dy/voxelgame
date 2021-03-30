#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <thread>
#include <sstream>
#include <string>
#include <list>

#include "utilities/basic_util.h"

#include "objects/Mesh.h"
#include "objects/Cursor.hpp"

#include "terrain/Terrain.h"
#include "src/terrain/WorldLoader.h"
#include "src/terrain/Chunk.h"

#include "utilities/Camera.h"
#include "utilities/fonts.h"
#include "utilities/controls.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}


//void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos);
//static void mouse_callback(GLFWwindow* window, int button, int action, int mods);
//void scroll_callback(GLFWwindow* window, double x, double y);
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
//int processInput(GLFWwindow* window);

//double mx = -1, my = -1;

//bool mouse_down = false;
//bool mouse_right_down = false;
	
//double yscroll = 0;
//bool keys[256] = {false};
//
//bool spressed = false;

int opengl_context_scope(GLFWwindow *window);

const int screen_width = 1270;
const int screen_height = 720;
	
int main(void){
	
//	std::list<int> t {1, 2, 3, 4, 5, 6, 7};
//	std::list<int>::iterator it = t.begin();
//	
//	while(it != t.end()){
//		
//	}

//	glm::all(glm::lessThan(a, b));
//	glm::ivec3 a(1.0f, 2.0f, 3.0f);
//	glm::ivec3 b(1.0f, 2.0f, 4.0f);
//	glm::ivec3 res = glm::lessThan(a, b);
//	
//	printf("%d %d %d\n", res.x, res.y, res.z);
//	printf("%d\n", glm::any(glm::lessThan(a, b)));
////	glm::less
//	
//	return 0;
	
//	srand(time(NULL));
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(screen_width, screen_height, "Simple example", NULL, NULL);
		
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
	
	opengl_context_scope(window);
	
    glfwTerminate();
	
	return 0;
}

int opengl_context_scope(GLFWwindow *window)
{
//	glm::ivec3 a(1, 2, 3);
//	glm::ivec3 b(1, 3, 5);
//	glm::ivec3 t = glm::abs(a - b) == glm::ivec3(1);
//	printf("%d %d %d \n", t.x, t.y, t.z);
//	
//	char gb;
//	scanf("%c", &gb);
//
//	return 0;
	
	
	const std::string CHUNK_VERTEX_SHADER = std::string("../src/shaders/chunkvs.vs");
	const std::string CHUNK_FRAGMENT_SHADER = std::string("../src/shaders/chunkfs.fs");

	
	
	// Setting params struct and callbacks.
	
	Camera kamera(static_cast<float>(screen_width), static_cast<float>(screen_height));
	
	ControlsStruct controls {
		.last_mx = -1,
		.last_my = -1,
		.kamera = kamera,
		
		.screen_width = screen_width,
		.screen_height = screen_height,
		
		.mouse_speed = 1.5,
		.cursor_pos = glm::ivec3(0),
		.prev_cursor_pos = glm::ivec3(0),
		
		.world_loader = nullptr,
		.block_key_pressed = false,
		.placing_block_type = 3
	};
	
	glfwSetWindowUserPointer(window, &controls);
	
    glfwMakeContextCurrent(window);
//    glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		printf("Failed to initialize GLAD\n");
		return -1;
	}
	
	glViewport(0, 0, screen_width, screen_height);
	
	glEnable(GL_DEPTH_TEST);
	
	FontMesh fontmesh1 = {0};
	if(initText(fontmesh1, "../font.png", "../src/shaders/text.vs", "../src/shaders/text.fs", glm::ortho(0.0f, static_cast<float>(screen_width), 0.0f, static_cast<float>(screen_height))) != 0){
		printf("Initialization of text failed.\n");
		
		glfwDestroyWindow(window);
		glfwTerminate();
		
		return 0;
	}
	
	shaderParams chunkShaderParams = getShaderParams(
		getShaderProgram(
			getShaderFromFile(CHUNK_VERTEX_SHADER, ShaderType::VERTEX),
			getShaderFromFile(CHUNK_FRAGMENT_SHADER, ShaderType::FRAGMENT)
			)
		);
	unsigned int textures = loadTexture(std::string("../texture.png"));
	
	glm::mat4 projection = glm::perspective(glm::radians(55.0f), static_cast<float>(screen_width) / screen_height, 0.1f, 1000.0f);
	
	glm::mat4 *view;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
	float camera_near = 0.1f;
	

	view = kamera.get_view();
	cameraPos = kamera.get_pos();
	
	WorldLoader wl(&projection, view, textures, chunkShaderParams);
	
	controls.world_loader = &wl;
	
	// Shopping list:
	//
	// Terrain generation in separate thread
	// Displaying text on screen for debug (1/2) (2 - Writing text from functions in other files, than this.)
	// Terrain genereation using perlin noise
	// Terrain compression when saving?
	// Frustrum culling
	// Make handling cursor nicer.
	// 2d Hud
	// Lighting
	// Physics
	// Mesh/Chunk allocator? (reusing vertex buffers)
	// Make greedy meshing work with lightning!
	// Change .h files to .hpp*
	// Split worldProvider to 2 files.
	// 
	
	
	Cursor cursor(controls.cursor_pos);
	
	char textBuffer[2000] = {};
    while (!glfwWindowShouldClose(window))
    {
		wl.update(kamera.get_pos());
		
		int debugkeys = processInput(window);
		
		kamera.updateView();
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		wl.draw(kamera.get_pos());
//		
		glm::vec3 kameraPos = kamera.get_pos();
		sprintf(textBuffer, "cameraPos: x: %.02f, y: %.02f, z: %.02f", kameraPos.x, kameraPos.y, kameraPos.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 50, 0.5);
//		
		glm::vec3 kameraFront = kamera.get_front();
		sprintf(textBuffer, "cameraFront: x: %.02f, y: %.02f, z: %.02f", kameraFront.x, kameraFront.y, kameraFront.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 20, 0.5);
		
//		glm::ivec3 c_pos;
		region_dtype block_under_cursor;
		std::tie(controls.cursor_pos, controls.prev_cursor_pos, block_under_cursor) = wl.collideRay(kameraPos, kameraFront, 7);
//		controls.cursor_pos = c_pos;
		
		sprintf(textBuffer, "cursor  : x: %2d, y: %2d, z: %2d ", controls.cursor_pos.x, controls.cursor_pos.y, controls.cursor_pos.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 70, 0.5);
		
		sprintf(textBuffer, "block under cursor: [%d, %d]", block_under_cursor & 0xff, block_under_cursor >> 8);
		renderText(fontmesh1, std::string(textBuffer), 20, 90, 0.5);
		
		glm::ivec3 chpos;
		std::tie(chpos, std::ignore) = toChunkCoords(controls.cursor_pos, 16);
		sprintf(textBuffer, "chunk cursor: x: %2d, y: %2d, z: %2d ", chpos.x, chpos.y, chpos.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 110, 0.5);
		
		// Cursor drawing
		cursor.draw(projection, view);
		// End of cursor drawing
		
		
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
	
	glDeleteTextures(1, &textures);
	
    return 0;
}

//void updateThread(WorldLoader *loader){
//	while(1)
//		loader.update(kamera.get_pos());
//}



//bool firstMouse = true;


//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
////	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
////        glfwSetWindowShouldClose(window, GL_TRUE);
////	
////	
////	if(action == GLFW_PRESS){
////		keys[key] = true;
////		spressed = key == GLFW_KEY_S;
////	}else if(action == GLFW_RELEASE){
////		keys[key] = false;
////	}
//}
/* https://asciiart.club/ */