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
#include "objects/Player.hpp"

#include "terrain/Terrain.h"
#include "terrain/Lighter.hpp"
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

//void arrChan(int a[3]) {
//	a[0] = 5;
//	a[1] = 2;
//	a[2] = 0;
//}

int main(void) {
//	int a[3] {1, 2, 3};
//	for(int i = 0; i < 3; i++){
//		printf("%d, ", a[i]);
//	}
//	
//	arrChan(a);
//	printf("\n");
//	for(int i = 0; i < 3; i++){
//		printf("%d, ", a[i]);
//	}
//	
//	getchar();
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
	const std::string CHUNK_VERTEX_SHADER = std::string("../src/shaders/chunkvs.vs");
	const std::string CHUNK_FRAGMENT_SHADER = std::string("../src/shaders/chunkfs.fs");
	
	// Setting params struct and callbacks.
	
	Camera kamera(static_cast<float>(screen_width), static_cast<float>(screen_height));
	Player player(AABB( {-0.4f, 0.0f, -0.2f}, {0.4f, 1.9f, 0.2f} ));
	
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
		.placing_block_type = 3,
		
		.player = player
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
	Lighter light(wl);
	
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
	// Reverse light values. ( 0 should be no light)
	// Removing block on chunk's edge doesn't update light in neighbouring chunk.
	// 
	
	
	Cursor cursor(controls.cursor_pos);
	glm::mat4& playersView = player.getView();
	
	char textBuffer[2000] = {};
    while (!glfwWindowShouldClose(window))
    {
		wl.update(player.positionFromHead);
		auto unlitColumns = wl.getUnlitColumns();
		if(unlitColumns.size() > 0) {
			light.updateLightColumns(unlitColumns);
		}
		
		std::list<ChangedBlock> blocks_changed = wl.getChangedBlocks();
		if (!blocks_changed.empty()) {
			light.updateLightForBlock(blocks_changed);
		}
		
		light.propagateLights();
		
		wl.addUpdatedChunks(light.getUpdatedChunks());
		wl.updateGeometry();
		
		int debugkeys = processInput(window);
		
		auto [groundIntersected, intersectionAmount] = wl.playerIntersects(player);
//		if (groundIntersected) {
////			player.position += intersectionAmount;
//			player.applyMove(true, intersectionAmount);
//		}else{
//		}
//		player.applyMove(false);

		
		player.updateView();
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		wl.draw(player.positionFromHead, playersView);
//		
		sprintf(textBuffer, "ground intersection: %.02f, y: %.02f, z: %.02f", intersectionAmount.x, intersectionAmount.y, intersectionAmount.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 130, 0.5);
		
		glm::vec3 kameraPos = player.positionFromHead;
		glm::vec3 playerPos = player.getPosition();
		sprintf(textBuffer, "playerPos: x: %.02f, y: %.02f, z: %.02f", playerPos.x, playerPos.y, playerPos.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 50, 0.5);
		
		glm::ivec3 playerBlockPos = {floor(playerPos.x), floor(playerPos.y), floor(playerPos.z)};
		std::tie(std::ignore, playerBlockPos) = toChunkCoordsReal(playerBlockPos, TerrainConfig::ChunkSize);
		sprintf(textBuffer, "playerOrdżin: x: %2d, y: %2d, z: %2d", playerBlockPos.x, playerBlockPos.y, playerBlockPos.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 70, 0.5);
//		
		glm::vec3 kameraFront = player.orientation;
		sprintf(textBuffer, "cameraFront: x: %.02f, y: %.02f, z: %.02f", kameraFront.x, kameraFront.y, kameraFront.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 20, 0.5);
		
//		glm::ivec3 c_pos;
		region_dtype block_under_cursor;
		std::tie(controls.cursor_pos, controls.prev_cursor_pos, block_under_cursor) = wl.collideRay(kameraPos, kameraFront, 7);
//		controls.cursor_pos = c_pos;
		
//		sprintf(textBuffer, "cursor  : x: %2d, y: %2d, z: %2d ", controls.cursor_pos.x, controls.cursor_pos.y, controls.cursor_pos.z);
//		renderText(fontmesh1, std::string(textBuffer), 20, 70, 0.5);
		
		sprintf(textBuffer, "block under cursor: [%d, %x]", block_under_cursor & 0xff, block_under_cursor >> 8);
		renderText(fontmesh1, std::string(textBuffer), 20, 90, 0.5);
		
		glm::ivec3 chpos;
		std::tie(chpos, std::ignore) = toChunkCoords(controls.cursor_pos, TerrainConfig::ChunkSize);
		sprintf(textBuffer, "chunk cursor: x: %2d, y: %2d, z: %2d ", chpos.x, chpos.y, chpos.z);
		renderText(fontmesh1, std::string(textBuffer), 20, 110, 0.5);
		
		// Cursor drawing
		cursor.draw(projection, &playersView);
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