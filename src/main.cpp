#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include <thread>
#include <sstream>
#include <string>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "terrain/worldProvider.hpp"

#include "utilities/basic_util.h"

#include "objects/Mesh.h"
#include "objects/Cursor.hpp"
#include "objects/Player.hpp"
#include "objects/Hud.hpp"

#include "terrain/Terrain.h"
#include "terrain/Lighter.hpp"
#include "terrain/WorldLoader.h"
#include "terrain/Chunk.h"

#include "utilities/Camera.h"
#include "utilities/fonts.h"
#include "utilities/controls.h"
#include "utilities/Shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef __EMSCRIPTEN__
const char* TEXT_TEXTURE_PATH = "font.png";
const char* TEXT_VS_PATH = "src/shaders/text.vs";
const char* TEXT_FS_PATH = "src/shaders/text.fs";
const char* CHUNK_VS_PATH = "src/shaders/chunkvs.vs";
const char* CHUNK_FS_PATH = "src/shaders/chunkfs.fs";
const char* TEXTURES_PATH = "texture.png";
#else
const char* TEXT_TEXTURE_PATH = "../font.png";
const char* TEXT_VS_PATH = "../src/shaders/text.vs";
const char* TEXT_FS_PATH = "../src/shaders/text.fs";
const char* CHUNK_VS_PATH = "../src/shaders/chunkvs.vs";
const char* CHUNK_FS_PATH = "../src/shaders/chunkfs.fs";
const char* TEXTURES_PATH = "../texture.png";
const char* HUD_VS_PATH = "../src/shaders/hud.vs";
const char* HUD_FS_PATH = "../src/shaders/hud.fs";
// const char* HUD_TEXTURE_PATH = TEXTURES_PATH;
const char* HUD_TEXTURE_PATH = "../crosshair.png";
#endif


static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int opengl_context_scope(GLFWwindow *window);

const int screen_width = 1270;
const int screen_height = 720;

struct LoopParams {
	WorldLoader* wl;
	Lighter* light;
	Player* player;
	GLFWwindow* window;
	char* textBuffer;
	FontMesh* fontMesh;
	ControlsStruct *controls;
	Cursor* cursor;
	glm::mat4 projection;
	glm::mat4& playersView;
	Hud* hud;
	RemoteWorldProvider* provider;
};

void main_loop(void* params);
void terrain_thread(WorldLoader& wl, Lighter& light, Player& player, bool& exitHamlet);

int main(void) {
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

int opengl_context_scope(GLFWwindow *window) {
	const std::string CHUNK_VERTEX_SHADER = std::string(CHUNK_VS_PATH);
	const std::string CHUNK_FRAGMENT_SHADER = std::string(CHUNK_FS_PATH);
	
	// Setting params struct and callbacks.
	
	Camera kamera(static_cast<float>(screen_width), static_cast<float>(screen_height));
	const float player_xz = 0.8f / 2.0f;
	Player player(AABB( {-player_xz, 0.0f, -player_xz}, {player_xz, 1.8f, player_xz} ));
	
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
	
	#ifndef __EMSCRIPTEN__ 
	// #else
		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
			printf("Failed to initialize GLAD\n");
			return -1;
		}
	#endif
	
	glViewport(0, 0, screen_width, screen_height);
	
	glEnable(GL_DEPTH_TEST);
	
	FontMesh fontmesh1 = {0};
	if(initText(fontmesh1, TEXT_TEXTURE_PATH, TEXT_VS_PATH, TEXT_FS_PATH, glm::ortho(0.0f, static_cast<float>(screen_width), 0.0f, static_cast<float>(screen_height))) != 0){
		printf("Initialization of text failed.\n");
		
		glfwDestroyWindow(window);
		glfwTerminate();
		
		return 0;
	}
	
	Shader chunkShader {CHUNK_VS_PATH, CHUNK_FS_PATH};

	shaderParams chunkShaderParams = getShaderParams(chunkShader.getProgram());
	// getShaderParams(
	// 	getShaderProgram(
	// 		getShaderFromFile(CHUNK_VERTEX_SHADER, ShaderType::VERTEX),
	// 		getShaderFromFile(CHUNK_FRAGMENT_SHADER, ShaderType::FRAGMENT)
	// 		)
	// 	);
	unsigned int textures = loadTexture(std::string(TEXTURES_PATH));
	
	glm::mat4 projection = glm::perspective(glm::radians(55.0f), static_cast<float>(screen_width) / screen_height, 0.1f, 1000.0f);
	
	glm::mat4 *view;
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
	float camera_near = 0.1f;
	

	view = kamera.get_view();
	cameraPos = kamera.get_pos();
	
	// Internet initialization (needed for now in world loader)
	
	// Internet initialization
	asio::io_context ioContext;
	asio::ip::tcp::resolver resolver(ioContext);
	asio::ip::tcp::resolver::results_type endpoints;// = resolver.resolve("localhost", "25013");
	try {
		endpoints = resolver.resolve("localhost", "25013");
	} catch (std::exception e) {
		printf("Resolve error: %s\n", e.what());
	}
	asio::ip::tcp::socket socket(ioContext);

	// Provider initialization.
	std::shared_ptr<RemoteWorldProvider> provider = std::make_shared<RemoteWorldProvider>(ioContext, std::move(socket), endpoints);
	provider->sendPlayerPosition(player.getPosition());
	// end of internet initialization
	// internet initialization end

	// Might be good to provide WorldLoader with provider so it will have less things to worry about.
	// WorldLoader wl(&projection, view, textures, chunkShaderParams, std::move(socket), endpoints);
	WorldLoader wl(&projection, view, textures, chunkShaderParams, provider);
	Lighter light(wl);
	
	controls.world_loader = &wl;
	
	// Shopping list:
	//
	// Frustrum culling
	// Make handling cursor nicer.
	// 2d Hud
	// Make greedy meshing work with lightning!!!!! (This not done??? xD)
	// Change .h files to .hpp*
	// Removing block on chunk's edge doesn't update light in neighbouring chunk.
	// Sometimes light doesn't get propagated when placing torch. (after multithreading implementation)
	// Some object that makes geometry from chunks (remove this responsibility from world loader)`
	// Sometimes chunks doesn't load
	// ---------------------------------------------------------
	// Ideaz
	// --------
	// wP.hpp:16
	// Mesh/Chunk allocator? (reusing vertex buffers)
	// World provider should return chunk objects, and region should consist of chunks. (Chunk objects should be in Region object).
	// 
	
	
	Cursor cursor(controls.cursor_pos);
	glm::mat4& playersView = player.getView();
	
	char textBuffer[2000] = {};
	Shader hudShader = Shader{HUD_VS_PATH, HUD_FS_PATH};
	Hud hud{hudShader, loadTexture(HUD_TEXTURE_PATH), screen_width, screen_height};

	// std::map<glm::ivec3, Mesh, compareVec3> meshes;
	// Prepare mesh data in thread, and call opengl methods in main thread.

	LoopParams loopP {
		.wl = &wl,
		.light = &light,
		.player = &player,
		.window = window,
		.textBuffer = textBuffer,
		.fontMesh = &fontmesh1,
		.controls = &controls,
		.cursor = &cursor,
		.projection = projection,
		.playersView = playersView,
		.hud = &hud,
		.provider = provider.get()
	};

	player.noclip = true;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	bool exitHamlet = false;

	std::thread terrain_task {terrain_thread, std::ref(wl), std::ref(light), std::ref(player), std::ref(exitHamlet)};

	// hmm~~~~~~~~
	// yeah don't hog the main thread.
	std::thread netThread([&] () {
		ioContext.run();
		printf("\t\t\tioContext ended NO INTERNET!!!\n");
	});
	// To send player position after connecting to server.
	player.moved = true;

	#ifdef __EMSCRIPTEN__ 
		emscripten_set_main_loop_arg(main_loop, (void*)&loopP, 0, true);
	#else
		while(!glfwWindowShouldClose(window)){
			main_loop((void*)&loopP);
		}
		exitHamlet = true;
		{
			std::scoped_lock lock(wl.updateMutex);
			wl.notified = true;
		}
		wl.updateNotifier.notify_one();
		terrain_task.join();
	#endif
	
	glDeleteTextures(1, &textures);
	
    return 0;
}

void terrain_thread(WorldLoader& wl, Lighter& light, Player& player, bool& exitHamlet) {
	// wl.notified = false;
	while(!exitHamlet) {
		printf("waiting for update\n");
		std::unique_lock lock{wl.updateMutex};
		wl.updateNotifier.wait(lock, [&wl](){ return wl.notified;});

		if (exitHamlet) {
			break;
		}

		printf("updating\n");

		wl.update(wl.cur_camera_pos - wl.last_camera_pos);

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
		wl.prepareGeometry();

		wl.notified = false;

		lock.unlock();
		printf("updated\n");
	}
}

void main_loop(void* params) {
	// Unpacking variables
	LoopParams* loopP = (LoopParams*)params;
	WorldLoader& wl = *loopP->wl;
	Lighter& light = *loopP->light;
	Player& player = *loopP->player;
	GLFWwindow* window = loopP->window;
	char* textBuffer = loopP->textBuffer;
	FontMesh fontmesh1 = *loopP->fontMesh;
	ControlsStruct& controls = *loopP->controls;
	Cursor& cursor = *loopP->cursor;
	glm::mat4& playersView = loopP->playersView;
	Hud& hud = *loopP->hud;
	RemoteWorldProvider* provider = loopP->provider;

	const float dt = 1000.0f / 12 / 1000;
	// float deltaTime = 0.0f;
	// float lastFrame = 0.0f;
	// float currentFrame = glfwGetTime();
	// deltaTime = currentFrame - lastFrame;
	// lastFrame = currentFrame;

	wl.disposeChunks();
	wl.updateGeometry();
	provider->update(glm::ivec3(0));
	wl.checkForUpdate(player.positionFromHead);
	
	int debugkeys = processInput(window);
	glm::vec3 dx = player.updatePhysics(dt);
	if (!player.noclip) {
		bool jumpReset = dx[1] < 0;
		dx = wl.collideAABB(player.aabb, player.velocity, dx);
		if (jumpReset && abs(dx[1]) < 0.0001) {
			player.jumped = false;
		}
	}
	player.applyTranslation(dx);
	if (player.moved) {
		provider->sendPlayerPosition(player.getPosition());
		player.moved = false;
	}

	
	player.updateView();
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	wl.draw(player.positionFromHead, playersView);

	glm::vec3 kameraPos = player.positionFromHead;
	glm::vec3 playerPos = player.getPosition();
	glm::vec3 kameraFront = player.orientation;
	region_dtype block_under_cursor;
	std::tie(controls.cursor_pos, controls.prev_cursor_pos, block_under_cursor) = wl.collideRay(kameraPos, kameraFront, 7);

	// glm::ivec3 c_pos;
	// controls.cursor_pos = c_pos;

	// Cursor drawing
	cursor.draw(loopP->projection, &playersView);
	// End of cursor drawing

	// 2d drawing
	hud.draw();

	sprintf(textBuffer, "ground intersection: %.02f, y: %.02f, z: %.02f", dx.x, dx.y, dx.z);
	renderText(fontmesh1, std::string(textBuffer), 20, 130, 0.5);
	
	sprintf(textBuffer, "playerPos: x: %.02f, y: %.02f, z: %.02f", playerPos.x, playerPos.y, playerPos.z);
	renderText(fontmesh1, std::string(textBuffer), 20, 50, 0.5);
	
	glm::ivec3 playerBlockPos = {floor(playerPos.x), floor(playerPos.y), floor(playerPos.z)};
	std::tie(std::ignore, playerBlockPos) = toChunkCoordsReal(playerBlockPos, TerrainConfig::ChunkSize);
	sprintf(textBuffer, "playerOrdżin: x: %2d, y: %2d, z: %2d", playerBlockPos.x, playerBlockPos.y, playerBlockPos.z);
	renderText(fontmesh1, std::string(textBuffer), 20, 70, 0.5);
//		
	sprintf(textBuffer, "cameraFront: x: %.02f, y: %.02f, z: %.02f", kameraFront.x, kameraFront.y, kameraFront.z);
	renderText(fontmesh1, std::string(textBuffer), 20, 20, 0.5);

//		sprintf(textBuffer, "cursor  : x: %2d, y: %2d, z: %2d ", controls.cursor_pos.x, controls.cursor_pos.y, controls.cursor_pos.z);
//		renderText(fontmesh1, std::string(textBuffer), 20, 70, 0.5);
	
	sprintf(textBuffer, "block under cursor: [%d, %x]", block_under_cursor & 0xff, block_under_cursor >> 8);
	renderText(fontmesh1, std::string(textBuffer), 20, 90, 0.5);
	
	glm::ivec3 chpos;
	std::tie(chpos, std::ignore) = toChunkCoords(controls.cursor_pos, TerrainConfig::ChunkSize);
	sprintf(textBuffer, "chunk cursor: x: %2d, y: %2d, z: %2d ", chpos.x, chpos.y, chpos.z);
	renderText(fontmesh1, std::string(textBuffer), 20, 110, 0.5);
	
	glfwSwapBuffers(window);
	glfwPollEvents();
}