#include "Shader.hpp"

Shader::Shader(const std::string& vsPath, const std::string& fsPath) {
	unsigned int vs = getShader(vsPath, GL_VERTEX_SHADER);
	unsigned int fs = getShader(fsPath, GL_FRAGMENT_SHADER);

	makeProgram(vs, fs);
	
	glDeleteShader(vs);
	glDeleteShader(fs);
}

const unsigned int& Shader::getProgram() {
	return program;
}

void Shader::makeProgram(const unsigned int& vs, const unsigned int& fs) {
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	char error_msg[512];
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(program, 512, NULL, error_msg);
		printf("prorgam log:\n%s\n", error_msg);
		program = 0;
		printf("succ = %d\n", success);
	}
}

unsigned int Shader::getShader(const std::string& path, int type) {
	unsigned int shader = 0;

	std::ifstream file(path);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();

		std::string vsText = buffer.str();
		const char* cbuffer = vsText.c_str();
		// printf("shader text: ");
		// for(int i = 0; i < vsText.length(); i++) {
		// 	printf("%c", vsText[i]);
		// }
		// printf("\n");

		shader = glCreateShader(type);
		glShaderSource(shader, 1, &cbuffer, NULL);
		glCompileShader(shader);
	
		int success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success) {
			printf("Some shader failed: %s\n", path.c_str());
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			printf("%s\n", infoLog);
			return -1;
		}
	}

	return shader;
}

void Shader::use() {
	glUseProgram(program);
}

Shader::~Shader()
{
}

