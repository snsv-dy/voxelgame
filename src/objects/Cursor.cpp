#include "Cursor.hpp"

Cursor::Cursor(glm::ivec3& pos) : position{pos} {
	init();
}

void Cursor::init(){
	std::string vspath = "../src/shaders/chunkvs.vs";
	std::string fspath = "../src/shaders/chunkfs.fs";
	shader = getShaderParams(
		getShaderProgram(
			getShaderFromFile(vspath, ShaderType::VERTEX),
			getShaderFromFile(fspath, ShaderType::FRAGMENT)
			)
		);
		
	// cube data
	std::vector<float> vertices;
	std::vector<unsigned int> v_data;
	
	// Vertives for a square composed of two triangles
	float square[12] {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, -1.0f,
		
		-1.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, -1.0f
	};
	
	const float cursor_size_mul = 0.505f;

	// Generating vertices for a cube from 2d squares
	for(float side = -1.0f; side < 2.0f; side += 2.0f){
		for(int norm = 0; norm < 3; norm++){
			int t = (norm + 1) % 3;
			int bt = (norm + 2) % 3;
			
			for(int i = 0; i < 12; i += 2){
				float vec[3];
				vec[norm] = side * cursor_size_mul;
				vec[t] = square[i] * cursor_size_mul;
				vec[bt] = square[i + 1] * cursor_size_mul;
				
				vertices.push_back(vec[0]);
				vertices.push_back(vec[1]);
				vertices.push_back(vec[2]);
				
				vertices.push_back(square[i]);
				vertices.push_back(square[i + 1]);
				vertices.push_back(0.0f);
				
				v_data.push_back(0x101);
				v_data.push_back(0x101);
				v_data.push_back(0x101);
			}
		}
	}
		
	mesh.initVao(vertices, v_data);
	mesh.setModelLocation(shader.model);
}

void Cursor::draw(const glm::mat4& projection, const glm::mat4* view){
	glBindTexture(GL_TEXTURE_2D, 0);	
	glUseProgram(shader.program);
	glUniformMatrix4fv(shader.projection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(shader.view, 1, GL_FALSE, glm::value_ptr(*view));
	mesh.model = glm::translate(glm::mat4(1.0f), glm::vec3(position) + glm::vec3(0.5f));
//		
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mesh.draw();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

Cursor::~Cursor()
{
	
}

