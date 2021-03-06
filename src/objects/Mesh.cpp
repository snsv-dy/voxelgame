#include "Mesh.h"

Mesh::Mesh() 
{
	model = glm::mat4(1.0f);
}

shaderParams& Mesh::getShaderParams(){
	return shParams;
}

//void Mesh::init_shaders(char *vsSource, char *fsSource){
//	this->vertex_shader = getShaderFromFile(vsSource, ShaderType::VERTEX);
//	if(this->vertex_shader == 0){
//		printf("No vs\n");
//	}
//	
//	this->fragment_shader = getShaderFromFile(fsSource, ShaderType::FRAGMENT);
//	if(this->fragment_shader == 0){
//		printf("No fs\n");
//	}
//	
//	shParams.program = getShaderProgram( this->vertex_shader, this->fragment_shader);
//	
//	shParams.projection = glGetUniformLocation(shParams.program, "projection");
//	shParams.view = glGetUniformLocation(shParams.program, "view");
//	shParams.model = glGetUniformLocation(shParams.program, "model");
////	this->locations.select = glGetUniformLocation(program, "selected");
////	this->locations.side = glGetUniformLocation(program, "selected_plane");
//	
//	shParams.light = glGetUniformLocation(shParams.program, "light_position");	
////	int light_pos_loc = glGetUniformLocation(program, "light_position");
//}

void Mesh::setShaderParams(shaderParams &params){
	this->shParams = params;
//	this->shParams.program = params.program;
//	this->shParams.light = params.light;
//	this->shParams.model = params.model;
//	this->shParams.projection = params.projection;
//	this->shParams.view = params.view;
}

void Mesh::setTexturesId(unsigned int id){
	this->texturesId = id;
}

void Mesh::set_mats_pointers(glm::mat4 *proj_ptr, glm::mat4 *view_ptr){
	this->mat_projection = proj_ptr;
	this->mat_view = view_ptr;
}

void Mesh::draw(){
	glUseProgram(shParams.program);
	
	glUniformMatrix4fv(shParams.projection, 1, GL_FALSE, glm::value_ptr(*this->mat_projection));
	glUniformMatrix4fv(shParams.view, 1, GL_FALSE, glm::value_ptr(*this->mat_view));
	glUniformMatrix4fv(shParams.model, 1, GL_FALSE,  glm::value_ptr(this->model));
//	glm::value_ptr(model));
//	glUniform3fv(this->locations.light, 1,
	glBindTexture(GL_TEXTURE_2D, this->texturesId);
	
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, this->vertices_count);
}


void Mesh::initVao(float *positions, unsigned int count){
	glGenVertexArrays(1, &this->VAO);
	
	glBindVertexArray(this->VAO);
	
	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), positions, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);
	
	this->vertices_count = count / 6;
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
	this->buffers_set = true;
}

void Mesh::updateData(float *data, unsigned int size){
//	if(data != NULL && size > 0){
	if(data != NULL){
//		if(isInit)
		
//		glBindVertexArray(this->VAO);
		
//		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, GL_STATIC_DRAW);
//		
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
//		glEnableVertexAttribArray(0);
//		
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
//		glEnableVertexAttribArray(1);
//		
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
////		
//		glBindVertexArray(0);
	
//		printf("data updated %d %d\n", size, size / 6);
//		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
//		
//		if(size >= this->vertices_count){
//			glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), data, GL_STATIC_DRAW);
//		}else{
//			glBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(float), data);
//		}
//		
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		this->vertices_and_normals = data;
		this->vertices_count = size / 6;
	}
}

glm::mat4 *Mesh::get_model(){
	return &this->model;
}

Mesh::~Mesh()
{
	if(buffers_set){
		glDeleteBuffers(1, &this->VBO);
		glDeleteBuffers(1, &this->VBO_INT);
		glDeleteVertexArrays(1, &this->VAO);
	}
}
