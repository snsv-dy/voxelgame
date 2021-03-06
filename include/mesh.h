#ifndef _MESH_H_
#define _MESH_H_

#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "basic_util.h"

class Mesh{
	
	int VAO, VBO;
	int shader_program;
	
	glm::mat4 model;
	
	//init checklist
	bool shaders_added = false;
	bool data_added = false;
	bool idk = false;
	
public: 
	Mesh();
	void add_shaders(const char *vs_source, const char *fs_source);
	void addVertices(float *data, int size);
	void init();
};

#endif

//public void add_shaders(const char *vs_source, const char *fs_source){
//		
//	}
//	
//	public void addVertices(float *data, int size){
//		
//	}
//	
//	public void init(){
//		
//	}