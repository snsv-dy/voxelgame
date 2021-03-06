#ifndef MESH_H
#define MESH_H



#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "../utilities/basic_util.h"

#include <glm/glm.hpp>
#include <vector>

//struct shaderParams{
//	unsigned int program;
//	int projection;
//	int view;
//	int model;
//	
//	int light;
//};

class Mesh
{
private:
	float *vertices_and_normals = NULL;
	unsigned int vertices_count; // above array size
	
	unsigned int VAO, VBO, VBO_INT;
	unsigned int vertex_shader, fragment_shader;
	bool buffers_set = false;
//	unsigned int program;
	
//	struct{
//		int projection;
//		int view;
//		int model;
//		
//		int select;
//		int side;
//		
//		int light;
//	} locations;
	shaderParams shParams;
	
	glm::mat4 *mat_projection;
	glm::mat4 *mat_view;
	
	
	unsigned int texturesId;
public:
	glm::mat4 model;
	void setTexturesId(unsigned int id);
//	void init_shaders(char *vsSource, char *fsSource);
	shaderParams& getShaderParams();
	void setShaderParams(shaderParams &params);
	
	void initVao(float *positions, unsigned int count);
	void initVao(std::vector<float>& data, std::vector<unsigned int>& int_data);
	void updateVao(std::vector<float>& data, std::vector<unsigned int>& int_data);
	
	void updateData(float *data, unsigned int size);
	
	void set_mats_pointers(glm::mat4 *proj_ptr, glm::mat4 *view_ptr);
	void draw();
	
	glm::mat4 *get_model();
	
	
	Mesh();
	~Mesh();
	
};

#endif // MESH_H
//#ifndef _MESH_H_
//#define _MESH_H_
//
//#include <glad/glad.h>
//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include "basic_util.h"
//
//
//struct shaderParams{
//	unsigned int program;
//	int projection;
//	int view;
//	int model;
//	
//	int light;
//};
//
//class Mesh{
//	glm::mat4 model;
//	unsigned int VAO;
//	unsigned int VBO;
//	
//	//init checklist
//	bool shaders_added = false;
//	bool data_added = false;
//	bool idk = false;
//	
//	unsigned int vertex_shader;
//	unsigned int fragment_shader;
//	unsigned int texturesId;
//	glm::mat4 *mat_projection;
//	glm::mat4 *mat_view;
//
//	shaderParams shParams;	
//	unsigned int vertices_count;
//	
//public: 
//	Mesh();
//	void add_shaders(const char *vs_source, const char *fs_source);
//	void addVertices(float *data, int size);
//	void init();
//	
//	void init_shaders(char *vsSource, char *fsSource);
//	void setShaderParams(shaderParams &params);
//	void setTexturesId(unsigned int id);
//	void set_mats_pointers(glm::mat4 *proj_ptr, glm::mat4 *view_ptr);
//	void draw();
//	shaderParams& getShaderParams();
//	void initVao(float *positions, unsigned int count);
//	void updateData(float *data, unsigned int size);
//	glm::mat4 *get_model();
//	~Mesh();
//};
//
//#endif
//
////public void add_shaders(const char *vs_source, const char *fs_source){
////		
////	}
////	
////	public void addVertices(float *data, int size){
////		
////	}
////	
////	public void init(){
////		
////	}