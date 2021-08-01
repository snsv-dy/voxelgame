#include "Hud.hpp"

Hud::Hud(Shader& shader, unsigned int textures, int screenWidth, int screenHeight): shader{shader}, textures{textures}, screenWidth{screenWidth}, screenHeight{screenHeight} {
	projection = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
	projectionLocation = glGetUniformLocation(shader.getProgram(), "projection");
	modelLocation = glGetUniformLocation(shader.getProgram(), "model");
	initCrosshair();
}

void Hud::initCrosshair() {
	std::vector<float> vertices {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
	};

	crossHairMesh.initVao(vertices);
	crosshairModel = glm::translate(glm::mat4(1.0f), glm::vec3((float)screenWidth / 2.0f, (float)screenHeight / 2.0f, 0.0f));
	crosshairModel = glm::scale(crosshairModel, glm::vec3(crosshairScale));
	// crosshairModel = glm::mat4(1.0f);
}

void Hud::draw() {
	shader.use();
	glBindTexture(GL_TEXTURE_2D, textures);
	// 2d mode
	glDisable(GL_DEPTH_TEST);
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(crosshairModel));
	crossHairMesh.draw();

	// 3d mode
	glEnable(GL_DEPTH_TEST);
}