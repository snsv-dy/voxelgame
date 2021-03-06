#pragma once

#include "Mesh.h"
class BoundnigBox{
	glm::vec3 points[8];
	glm::vec3 center;
public:
	BoundnigBox(glm::vec3 points[8], glm::vec3 center){
		
		this->points[0] = points[6];
		this->points[1] = points[3];
		this->points[2] = points[4];
		this->points[3] = points[0];
		this->points[4] = points[7];
		this->points[5] = points[2];
		this->points[6] = points[5];
		this->points[7] = points[1];
	}
	
	BoundnigBox(glm::vec3 center, float width){
		glm::vec3 temp_points[8];
		int tpi = 0;
		for(float z = 1; x >= -1; x -= 2){
			for(float y = 1; y >= -1; y -= 2){
				for(float x = -1; x <= 1; x += 2){
					temp_points[tpi++] = glm::vec3(x, y, z);
				}
			}
		}
		
		this->points[0] = temp_points[6];
		this->points[1] = temp_points[3];
		this->points[2] = temp_points[4];
		this->points[3] = temp_points[0];
		this->points[4] = temp_points[7];
		this->points[5] = temp_points[2];
		this->points[6] = temp_points[5];
		this->points[7] = temp_points[1];
	}
	
	glm::vec3 get_closest(glm::vec3 camera){
		glm::vec3 dir = glm::normalize(this->center - camera);
		
		int id = dir.z >= 0;
		id |= (dir.y >= 0)<<1;
		id |= (dir.x >= 0)<<2;
		
		return this->points[id];
	}
	
	glm::vec3 get_furthest(glm::vec3 camera){
		glm::vec3 dir = glm::normalize(this->center - camera);
		
		int id = dir.z < 0;
		id |= (dir.y < 0)<<1;
		id |= (dir.x < 0)<<2;
		
		return this->points[id];
	}
	
	bool ray_intersect(glm::vec3 pos, glm::vec3 dir){
		glm::vec3 bounds[2];
		bounds[0] = get_closest(pos);
		bounds[1] = get_furthest(pos);
		
		float t0x = (bounds[0].x - pos.x) / dir.x;
		float t0y = (bounds[0].y - pos.y) / dir.y;
		
		float t1x = (bounds[1].x - pos.x) / dir.x;
		float t1y = (bounds[1].y - pos.y) / dir.y;
		
		float minest = t0x;
		if(t0y > minest) minest = t0y;
		
		float maxest = t1x;
		if(t1y < maxest) maxest = t1y;
		
		if(minest > maxest){
			return false;
		}
		float t0z = (bounds[0].z - pos.z) / dir.z;
		float t1z = (bounds[1].z - pos.z) / dir.z;
		
		if(minest > t1z || t0z > maxest )
			return false;
			
		return true; 
	}
	
};

class BasicObject{
private:
	Mesh *mesh;
	
	glm::vec3 near;
	glm::vec3 far;
	glm::vec3 bounds[2];
	
	glm::mat4 model_above_mesh;
	bool model_set = false;
	
//	glm::mat3
	
	// function source
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	// pos - ray begin
public:
	bool selected = false;

	void setMesh(Mesh *mesh){
		this->mesh = mesh;
	}
	
	void setModel(glm::mat4 model){
		this->model_above_mesh = model;
		this->model_set = true;
	}
	
	glm::mat4 *get_model(){
		return &this->model_above_mesh;
	}
	
	Mesh *get_mesh(){
		return this->mesh;
	}
	
	void draw(){
		if(!this->model_set)
			this->mesh->draw();
		else
			this->mesh->draw(this->model_above_mesh, this->selected);
	}
	
	void draw(glm::mat4 model){
		this->mesh->draw(model);
	}
	
	
	
	bool intersect(glm::vec3 pos, glm::vec3 dir){
		float t0x = (this->bounds[0].x - pos.x) / dir.x;
		float t0y = (this->bounds[0].y - pos.y) / dir.y;
		
		float t1x = (this->bounds[1].x - pos.x) / dir.x;
		float t1y = (this->bounds[1].y - pos.y) / dir.y;
		
		float minest = t0x;
		if(t0y > minest) minest = t0y;
		
		float maxest = t1x;
		if(t1y < maxest) maxest = t1y;
		
		if(minest > maxest){
//			printf("not a chens ");
			return false;
		}
		
//		printf("(%.2f %.2f %.2f) (%.2f %.2f %.2f) ", t0x, t0y, t0z, t1x, t1y, t1z);
		
		float t0z = (this->bounds[0].z - pos.z) / dir.z;
		float t1z = (this->bounds[1].z - pos.z) / dir.z;
		
//		t0z *= -1;
//		t1z *= -1;
		
		if(minest > t1z || t0z > maxest )
			return false;
		
//		if(t0z > minest) minest = t0z;
//		
//		if(t1z < maxest) maxest = t1z;
		
//		printf("[%.2f %.2f]\n", minest, maxest);
		
		
		return true; 
	}
};