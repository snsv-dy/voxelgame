#include "Mesh.h"

Mesh& Mesh::operator=(Mesh&& mesh){
	this->VAO = mesh.VAO;
	this->VBO_INT = mesh.VBO_INT;
	this->VBO = mesh.VBO;
	
	this->vertices_count = mesh.vertices_count;
	this->buffers_set = mesh.buffers_set;
	
	this->model_location = mesh.model_location;
}

void Mesh::setModelLocation(unsigned int model_loc){
	model_location = model_loc;
}

void Mesh::draw(){	
	glUniformMatrix4fv(model_location, 1, GL_FALSE,  glm::value_ptr(this->model));
	
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, this->vertices_count);
}

void Mesh::initVao(std::vector<float>& data, std::vector<unsigned int>& int_data){
	glGenVertexArrays(1, &this->VAO);
	
	glBindVertexArray(this->VAO);
	
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->VBO_INT);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO_INT);
	glBufferData(GL_ARRAY_BUFFER, int_data.size() * sizeof(int), int_data.data(), GL_STATIC_DRAW);
	glVertexAttribIPointer(2, 1, GL_INT, 1 * sizeof(int), (void *)0);
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);
	
	this->vertices_count = data.size() / 6;
	this->buffers_set = true;
}

void Mesh::updateVao(std::vector<float>& data, std::vector<unsigned int>& int_data){
	if(!buffers_set){
		initVao(data, int_data);
	}else{
		glBindVertexArray(this->VAO);
		
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO_INT);
		glBufferData(GL_ARRAY_BUFFER, int_data.size() * sizeof(int), int_data.data(), GL_STATIC_DRAW);
		glVertexAttribIPointer(2, 1, GL_INT, 1 * sizeof(int), (void *)0);
		glEnableVertexAttribArray(2);
		
		glBindVertexArray(0);
		
		this->vertices_count = data.size() / 6;
	//	this->buffers_set = true;
	}
}

Mesh::~Mesh()
{
	if(buffers_set){
		if(VBO != 0)
			glDeleteBuffers(1, &this->VBO);
			
		if(VBO_INT != 0)
			glDeleteBuffers(1, &this->VBO_INT);
		
		if(VAO != 0)
			glDeleteVertexArrays(1, &this->VAO);
	}
}
