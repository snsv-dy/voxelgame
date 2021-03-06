#ifndef _FONTS_H_
#define _FONTS_H_

#include "stb_image.h"

#include <glad/glad.h>
#include "basic_util.h"
#include <string>
#include <list>

// https://evanw.github.io/font-texture-generator/
typedef struct{
	unsigned int VAO, VBO;
	unsigned int vertex_shader, fragment_shader;
	unsigned int texture;
	float textureWidth;
	float textureHeight;
	unsigned int shaderProgram;
	
	glm::mat4 projection;
	unsigned int projectionLocation;
	unsigned int textureWidthLocation;
	unsigned int textureHeightLocation;
} FontMesh;

//FontMesh fontParams;

// Maybye change to throw instead of returning != 0 when handling errors.
// fontParams 			- FontMesh object which will contain everything needed for drawing
// fontTexturePath 		- Path to texture atlas image.
// vertexShaderPath 	- Path to vertex shader source.
// fragmentShaderPath 	- Path to fragment shader source.
// projection 			- Matrix for orthorgaphic projection.
int initText(FontMesh& fontParams, std::string fontTexturePath, std::string vertexShaderPath, std::string fragmentShaderPath, glm::mat4 projection);

typedef struct Character {
  int codePoint;
  float x, y, width, height, originX, originY;
} Character;

typedef struct Font {
  const char *name;
  int size, bold, italic, width, height, characterCount;
  Character *characters;
} Font;

//
//static Font font_Times_New_Roman = {"Times New Roman", 50, 0, 0, 1024, 128, 95, characters_Times_New_Roman};


// basically https://learnopengl.com/In-Practice/Text-Rendering
void renderText(FontMesh &font, std::string text, float x, float y, float scale);

class textDisplayer{
	std::list<std::string> dynamicTexts;
	float screen_width;
	float screen_height;
	
	float font_size;
	float scale;
	
	glm::vec2 origin;
	
public:
	textDisplayer(float screenWidth, float screenHeight, float fontSize, glm::vec2 origin){
		this->screen_width = screenWidth;
		this->screen_height = screenHeight;
		this->font_size = fontSize;
		this->origin = origin;
	}
	
	void addText(std::string str){
		dynamicTexts.push_back(str);
	}
	
	void renderTexts(FontMesh& font){
		while(dynamicTexts.size() > 0);
	}
};

#endif