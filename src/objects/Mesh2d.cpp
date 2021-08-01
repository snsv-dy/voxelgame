#include "Mesh2d.hpp"

Mesh2d& Mesh2d::operator=(Mesh2d&& mesh){
	this->VAO = mesh.VAO;
	this->VBO = mesh.VBO;
	
	this->vertices_count = mesh.vertices_count;
	this->buffers_set = mesh.buffers_set;
}

void Mesh2d::draw() {	
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, this->vertices_count);
}

// (xpos ypos xtex ytex) = 1 vertex
void Mesh2d::initVao(std::vector<float>& data) {
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(this->VAO);
	
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(0);
	
	this->vertices_count = data.size() / 4;
	this->buffers_set = true;

	printf("vao vcount: %d\n", vertices_count);
}

void Mesh2d::updateVao(std::vector<float>& data) {
	if (!buffers_set) {
		initVao(data);
	} else {
		glBindVertexArray(this->VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		glBindVertexArray(0);
		
		this->vertices_count = data.size() / 4;
	}
}

Mesh2d::~Mesh2d() {
	if (buffers_set) {
		if(VBO != 0)
			glDeleteBuffers(1, &this->VBO);
		
		if(VAO != 0)
			glDeleteVertexArrays(1, &this->VAO);
	}
}
