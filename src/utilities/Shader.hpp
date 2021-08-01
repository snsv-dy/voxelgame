#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <sstream>
#include <fstream>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

class Shader
{
	unsigned int program;
	unsigned int getShader(const std::string& vsPath, int type);
	void makeProgram(const unsigned int& vs, const unsigned int& fs);
public:
	Shader(const std::string& vsPath, const std::string& fsPath);
	void use();
	const unsigned int& getProgram();

	~Shader();
};

#endif // SHADER_HPP
