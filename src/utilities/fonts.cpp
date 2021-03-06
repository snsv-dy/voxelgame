#include "fonts.h"

static Character characters_Times_New_Roman[] = {
  {' ', 389, 84, 3, 3, 1, 1},
  {'!', 783, 0, 9, 37, -4, 35},
  {'"', 233, 84, 17, 17, -2, 35},
  {'#', 258, 0, 27, 38, 1, 35},
  {'$', 194, 0, 23, 42, -1, 37},
  {'%', 217, 0, 41, 38, 0, 35},
  {'&', 371, 0, 39, 37, 0, 35},
  {'\'', 260, 84, 7, 17, -1, 35},
  {'(', 62, 0, 16, 48, -1, 36},
  {')', 78, 0, 16, 48, 0, 36},
  {'*', 188, 84, 21, 22, -2, 36},
  {'+', 742, 48, 30, 28, 1, 31},
  {',', 250, 84, 10, 17, -1, 7},
  {'-', 345, 84, 15, 6, -1, 14},
  {'.', 337, 84, 8, 8, -2, 6},
  {'/', 339, 0, 16, 38, 1, 36},
  {'0', 571, 0, 25, 37, 0, 35},
  {'1', 169, 48, 16, 36, -4, 35},
  {'2', 78, 48, 24, 36, 0, 35},
  {'3', 713, 0, 21, 37, -1, 35},
  {'4', 0, 48, 26, 36, 1, 35},
  {'5', 126, 48, 22, 36, -1, 34},
  {'6', 621, 0, 24, 37, -1, 35},
  {'7', 102, 48, 24, 36, 0, 34},
  {'8', 691, 0, 22, 37, -2, 35},
  {'9', 645, 0, 23, 37, -1, 35},
  {':', 943, 48, 8, 26, -3, 24},
  {';', 716, 48, 10, 34, -2, 24},
  {'<', 772, 48, 30, 27, 1, 30},
  {'=', 267, 84, 30, 12, 1, 23},
  {'>', 802, 48, 30, 27, 1, 30},
  {'?', 734, 0, 21, 37, -1, 35},
  {'@', 0, 0, 45, 48, -1, 36},
  {'A', 880, 0, 38, 36, 1, 35},
  {'B', 457, 48, 33, 35, 1, 34},
  {'C', 483, 0, 33, 37, 0, 35},
  {'D', 384, 48, 37, 35, 1, 34},
  {'E', 522, 48, 31, 35, 0, 34},
  {'F', 644, 48, 28, 35, 1, 34},
  {'G', 410, 0, 37, 37, 0, 35},
  {'H', 270, 48, 38, 35, 1, 34},
  {'I', 699, 48, 17, 35, 0, 34},
  {'J', 148, 48, 21, 36, 0, 34},
  {'K', 231, 48, 39, 35, 1, 34},
  {'L', 553, 48, 31, 35, 0, 34},
  {'M', 185, 48, 46, 35, 1, 34},
  {'N', 841, 0, 39, 36, 2, 34},
  {'O', 447, 0, 36, 37, 0, 35},
  {'P', 615, 48, 29, 35, 1, 34},
  {'Q', 131, 0, 36, 46, 0, 35},
  {'R', 421, 48, 36, 35, 1, 34},
  {'S', 596, 0, 25, 37, -2, 35},
  {'T', 584, 48, 31, 35, 0, 34},
  {'U', 918, 0, 38, 36, 1, 34},
  {'V', 956, 0, 38, 36, 1, 34},
  {'W', 792, 0, 49, 36, 1, 34},
  {'X', 308, 48, 38, 35, 1, 34},
  {'Y', 346, 48, 38, 35, 1, 34},
  {'Z', 490, 48, 32, 35, 1, 34},
  {'[', 181, 0, 13, 46, -3, 35},
  {'\\', 355, 0, 16, 38, 1, 36},
  {']', 167, 0, 14, 46, 0, 35},
  {'^', 209, 84, 24, 20, 0, 35},
  {'_', 360, 84, 29, 5, 2, -7},
  {'`', 297, 84, 11, 11, -1, 36},
  {'a', 857, 48, 24, 26, 0, 24},
  {'b', 285, 0, 27, 38, 2, 36},
  {'c', 881, 48, 22, 26, 0, 24},
  {'d', 312, 0, 27, 38, 0, 36},
  {'e', 903, 48, 22, 26, 0, 24},
  {'f', 668, 0, 23, 37, 0, 36},
  {'g', 26, 48, 26, 36, 0, 24},
  {'h', 544, 0, 27, 37, 1, 36},
  {'i', 755, 0, 14, 37, 0, 36},
  {'j', 94, 0, 16, 48, 5, 36},
  {'k', 516, 0, 28, 37, 1, 36},
  {'l', 769, 0, 14, 37, 0, 36},
  {'m', 951, 48, 41, 25, 1, 24},
  {'n', 38, 84, 27, 25, 1, 24},
  {'o', 832, 48, 25, 26, 0, 24},
  {'p', 994, 0, 27, 36, 2, 24},
  {'q', 52, 48, 26, 36, 0, 24},
  {'r', 119, 84, 19, 25, 1, 24},
  {'s', 925, 48, 18, 26, -1, 24},
  {'t', 726, 48, 16, 33, 1, 31},
  {'u', 65, 84, 27, 25, 1, 23},
  {'v', 92, 84, 27, 25, 1, 23},
  {'w', 0, 84, 38, 25, 1, 23},
  {'x', 138, 84, 27, 24, 1, 23},
  {'y', 672, 48, 27, 35, 1, 23},
  {'z', 165, 84, 23, 24, 0, 23},
  {'{', 45, 0, 17, 48, -5, 36},
  {'|', 126, 0, 5, 48, -2, 36},
  {'}', 110, 0, 16, 48, -3, 36},
  {'~', 308, 84, 29, 9, 1, 18},
};

int initText(FontMesh& fontParams, std::string fontTexturePath, std::string vertexShaderPath, std::string fragmentShaderPath, glm::mat4 projection){
	
	//
	// Loading texture atlas
	//
	
	int tw, th, tchannels;
	unsigned char *tex_ptr = stbi_load(fontTexturePath.c_str(), &tw, &th, &tchannels, 0);
	
	if(tex_ptr == NULL){		
		return 1;
	}
	
	glGenTextures(1, &fontParams.texture);
	glBindTexture(GL_TEXTURE_2D, fontParams.texture);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_ptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(tex_ptr);
	
	fontParams.textureWidth = static_cast<float>(tw);
	fontParams.textureHeight = static_cast<float>(th);
	
	//
	// Loading shaders
	//
	
	unsigned int vertex_shader = getShaderFromFile((char *)vertexShaderPath.c_str(), ShaderType::VERTEX);
	if(vertex_shader == 0){
		printf("Error when loading vertex shader: '%s', for font: '%s'\n", vertexShaderPath, fontTexturePath);
		return 2;
	}
	
	unsigned int fragment_shader = getShaderFromFile((char *)fragmentShaderPath.c_str(), ShaderType::FRAGMENT);
	if(fragment_shader == 0){
		printf("Error when loading fragment shader: '%s', for font: '%s'\n", vertexShaderPath, fontTexturePath);
		return 2;
	}
	
	fontParams.shaderProgram = getShaderProgram( vertex_shader, fragment_shader);
	
	
	//
	// Setting up memory for drawing
	//
	
	glGenVertexArrays(1, &fontParams.VAO);
	glGenBuffers(1, &fontParams.VBO);
	
	glBindVertexArray(fontParams.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, fontParams.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, NULL, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	
	fontParams.projection = projection;
	fontParams.projectionLocation = glGetUniformLocation(fontParams.shaderProgram, "projection");
	
	fontParams.textureWidthLocation = glGetUniformLocation(fontParams.shaderProgram, "texWidth");
	fontParams.textureHeightLocation = glGetUniformLocation(fontParams.shaderProgram, "texHeight");
	return 0;
}

static Font defaultFont = {"Times New Roman", 50, 0, 0, 1024, 128, 95, characters_Times_New_Roman};
// basically https://learnopengl.com/In-Practice/Text-Rendering
void renderText(FontMesh &font, std::string text, float x, float y, float scale){
	glUseProgram(font.shaderProgram);
	glUniformMatrix4fv(font.projectionLocation, 1, false, glm::value_ptr(font.projection));
	glUniform1f(font.textureWidthLocation, font.textureWidth);
	glUniform1f(font.textureHeightLocation, font.textureHeight);
	
	glBindTexture(GL_TEXTURE_2D, font.texture);
	glBindVertexArray(font.VAO);
	
//	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	for(std::string::const_iterator c = text.begin(); c != text.end() && *c != '\0'; c++){
		// Subtracting ' ', becouse as you can see below space is first character in array, and doing so it maps characters from ascii to indexes in array.
//		printf("e char\n");
//		
		if(*c == '\n')
			y -= scale * 50;		

		Character *ch = &defaultFont.characters[*c - ' '];
//		printf("p char\n");
//		
		float xpos = x + ch->originX * scale;
		float ypos = y - (ch->height - ch->originY) * scale;
		
		float w = ch->width * scale;
		float h = ch->height * scale;
		
		float cx = ch->x / font.textureWidth;
		float cy = (ch->y / font.textureHeight);
		
		float hc = ch->height / font.textureHeight;
		float wc = ch->width / font.textureWidth;
		
//		float vertices[6][4] = {
//			{0, 0, 0, 0},
//			{0, 100, 0, 0},
//			{100, 100, 0, 0}
////			{100, 0, 0, 0}
//		};
		
		float vertices[6][4] = {
			{xpos, 		ypos + h,	cx, cy},
			{xpos, 		ypos,		cx, cy + hc},
			{xpos + w,	ypos,		cx + wc, cy + hc},
			
			{xpos + w,	ypos,		cx + wc, cy + hc},
			{xpos + w,	ypos + h,	cx + wc, cy},
			{xpos, 		ypos + h,	cx, cy}
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, font.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += w;
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
//	glEnable(GL_DEPTH_TEST);
}