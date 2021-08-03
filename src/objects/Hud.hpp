#ifndef _HUD_H_
#define _HUD_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Mesh2d.hpp"
#include "../utilities/Shader.hpp"

class Hud {
	int screenWidth;
	int screenHeight;
	glm::mat4 projection;
	unsigned int projectionLocation;
	unsigned int modelLocation;

	unsigned int textures;
	Shader& shader;

	Mesh2d crossHairMesh;
	glm::mat4 crosshairModel;
	float crosshairScale = 10.0f;
	void initCrosshair();
public:
	Hud(Shader& shader, unsigned int textures, int screenWidth, int screenHeight);
	void draw();
};

#endif