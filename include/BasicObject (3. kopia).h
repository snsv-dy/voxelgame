#pragma once

#include "Mesh.h"
class BoundingBox{
	glm::vec3 points[8];
	glm::vec3 center;
	glm::vec3 sides_normals[6] = {
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	};
	
	int vertex_sides[8][3] = {
		{5, 3, 2},
		{3, 0, 5},
		{2, 3, 4},
		{3, 4, 0},
		{2, 5, 1},
		{0, 1, 5},
		{2, 4, 1},
		{4, 1, 0}
	};
	
	float width = 0.5f;
	
public:
	BoundingBox(){}
	
	BoundingBox(glm::vec3 points[8], glm::vec3 center){
		this->set_points(points, center);
	}
	
	BoundingBox(glm::vec3 center, float width){
		printf("setting center: (%.2f %.2f %.2f)\n", center.x, center.y, center.z);
		this->set_points(center, width);
		this->width = width;
	}
	
	void set_points(glm::vec3 center, float width){
		this->center = center;
		
		glm::vec3 temp_points[8];
		int tpi = 0;
		for(float z = 1; z >= -1; z -= 2){
			for(float y = 1; y >= -1; y -= 2){
				for(float x = -1; x <= 1; x += 2){
					temp_points[tpi++] = glm::vec3(x, y, z) * width;
//					printf("point %d: (%.2f %.2f %.2f)\n", tpi-1, temp_points[tpi-1].x, temp_points[tpi-1].y, temp_points[tpi-1].z);
				}
			}
		}
		
		this->points[0] = temp_points[6];
		this->points[1] = temp_points[2];
		this->points[2] = temp_points[4];
		this->points[3] = temp_points[0];
		this->points[4] = temp_points[7];
		this->points[5] = temp_points[3];
		this->points[6] = temp_points[5];
		this->points[7] = temp_points[1];
	}
	
	void set_points(glm::vec3 points[8], glm::vec3 center){
		this->center = center;
		
		this->points[0] = points[6];
		this->points[1] = points[2];
		this->points[2] = points[4];
		this->points[3] = points[0];
		this->points[4] = points[7];
		this->points[5] = points[3];
		this->points[6] = points[5];
		this->points[7] = points[1];
	}
	
//	void set_model(glm::mat4 *model){
//		this->model = model;
//	}
	
	glm::vec3 get_closest(glm::vec3 camera, glm::mat4 model, int *id_ret = NULL){
//		glm::vec3 dir = glm::normalize(glm::vec3(model * glm::vec4(this->center, 1.0f)) - camera);
		glm::vec3 pos = glm::vec3(model * glm::vec4(this->center, 1.0f));
		glm::vec3 dir = glm::normalize(pos - camera);
		
//		printf("dir: (%.2f %.2f %.2f), ", dir.x, dir.y, dir.z);
//		printf("camera: (%.2f %.2f %.2f), ", camera.x, camera.y, camera.z);
//		printf("pos: (%.2f %.2f %.2f), ", pos.x, pos.y, pos.z);
		
		int id = (dir.z < 0);
		id |= (dir.y < 0)<<1;
		id |= (dir.x < 0)<<2;
		
		if(id_ret != NULL)
			*id_ret = id;
			
//		printf("id: %d, ", id);
//		printf("p: (%.2f %.2f %.2f)\n", this->points[id].x, this->points[id].y, this->points[id].z);
//		printf("\n");
		return glm::vec3(model * glm::vec4(this->points[id], 1.0f));
//		return this->points[id];
	}
	
	glm::vec3 get_furthest(glm::vec3 camera, glm::mat4 model){
		glm::vec3 dir = glm::normalize(glm::vec3(model * glm::vec4(this->center, 1.0f)) - camera);
		
		int id = dir.z >= 0;
		id |= (dir.y >= 0)<<1;
//		id |= 2;
		id |= (dir.x >= 0)<<2;
		
		return glm::vec3(model * glm::vec4(this->points[id], 1.0f));
	}
	
	bool point_between(glm::vec3 p, glm::vec3 b1, glm::vec3 b2){
		return (p.x < b2.x) && (p.y < b2.y) && (p.z < b2.z) && (p.x > b1.x) && (p.y > b1.y) && (p.z > b1.z);
	}
	
	bool plane_intersect(glm::vec3 ray, glm::vec3 ray_point, glm::vec3 plane_norm, glm::vec3 plane_point, int planeid){
		plane_norm *= -1;
		float denom = glm::dot(plane_norm, ray);
		if(denom > 1e-6){
			glm::vec3 prvec = plane_point - ray_point;
			float t = (glm::dot(prvec, plane_norm) / denom);
			prvec = ray_point + ray * t - plane_point;
			glm::vec2 point_relative;
			switch(planeid){
				case 0: 
					point_relative.x = prvec.x;
					point_relative.y = prvec.y;
				break;
				case 1: 
					point_relative.x = -prvec.z;
					point_relative.y = prvec.y;
				break;
				case 2: 
					point_relative.x = -prvec.x;
					point_relative.y = prvec.y;
				break;
				case 3: 
					point_relative.x = prvec.z;
					point_relative.y = prvec.y;
				break;
				case 4:
					point_relative.x = prvec.x;
					point_relative.y = -prvec.z;
				break;
				case 5:
					point_relative.x = prvec.x;
					point_relative.y = prvec.z;
				break;
			}
			
			if( t >= 0 && (point_relative.x >= -this->width && point_relative.x <= this->width) && (point_relative.y >= -this->width && point_relative.y <= this->width)){
				return true;
			}
			
//			printf("prvec: (%.2f %.2f %.2f)\n", prvec.x, prvec.y, prvec.z);
//			return t >= 0;
		}
		return false;
	}
	
	char get_plane(glm::vec3 origin, glm::vec3 dir, glm::mat4 model){
		int closest_id;
		glm::vec3 closest = this->get_closest(origin, model, &closest_id);
		
//		printf("planes for %d: ", closest_id);
		for(int i = 0; i < 3; i++){
//			printf("%d ", this->vertex_sides[closest_id][i]);
			glm::vec3 plane_normal = this->sides_normals[this->vertex_sides[closest_id][i]];
			glm::vec3 plane_point = model * glm::vec4(plane_normal, 1.0f) * this->width;
//			printf("plane_point: (%.2f %.2f %.2f)\n", plane_point.x, plane_point.y, plane_point.z);
			if(plane_intersect(dir, origin, plane_normal, plane_point, this->vertex_sides[closest_id][i])){
//				printf("\n");
				return this->vertex_sides[closest_id][i];
			}
		}
//		printf("\n");
		
		return -1;
	}
	
	bool ray_intersect(glm::vec3 origin, glm::vec3 dir, glm::mat4 model){
		glm::vec3 bounds[2];
		bounds[0] = get_closest(origin, model);
		bounds[1] = get_furthest(origin, model);
//		bounds[0] = glm::vec3(-0.5f, -0.5f, 0.5f);
//		bounds[1] = glm::vec3(0.5f, 0.5f, -0.5f);
//		printf("closest: (%.2f %.2f %.2f), farest (%.2f %.2f %.2f)\n", bounds[0].x, bounds[0].y, bounds[0].z, bounds[1].x, bounds[1].y, bounds[1].z);
//		printf("ray: (%.2f %.2f %.2f)\n", dir.x, dir.y, dir.z);
//		printf("kamera: (%.2f %.2f %.2f)\n", pos.x, pos.y, pos.z);
		
		float t0x = (bounds[0].x - origin.x) / dir.x;
		float t1x = (bounds[1].x - origin.x) / dir.x;
		
		if(t0x > t1x){
			float t = t0x;
			t0x = t1x;
			t1x = t;
		}
		
		float t0y = (bounds[0].y - origin.y) / dir.y;
		float t1y = (bounds[1].y - origin.y) / dir.y;
		
		if(t0y > t1y){
			float t = t0y;
			t0y = t1y;
			t1y = t;
		}
		
//		printf("t0y, t1y: %.2f %.2f\n", t0y, t1y);
//		float minest = t0x;
//		if(t0y > minest) minest = t0y;
//		printf("t0x, t0y: %.2f %.2f\n", t0x, t0y);
//		printf("t1x, t1y: %.2f %.2f\n", t1x, t1y);
		
//		float maxest = t1x;
//		if(t1y < maxest) maxest = t1y;
		
		if( (t0x > t1y) || (t0y > t1x)){
			return false;
		}
		
		if(t0y > t0x){
			t0x = t0y;
			printf("miny wieksze\n");
		}
			
		if(t1y < t1x){
			printf("maxy wieksze\n");
			t1x = t1y;
		}
		
		float t0z = (bounds[0].z - origin.z) / dir.z;
		float t1z = (bounds[1].z - origin.z) / dir.z;
//		float t0z = (origin.z - bounds[0].z) / dir.z;
//		float t1z = (origin.z - bounds[1].z) / dir.z;
		
		if(t0z > t1z){
			float t = t0z;
			t0z = t1z;
			t1z = t;
		}
		
		if(t0x > t1z || t0z > t1x ){
			printf("failed: %d %d (%.2f > %.2f) (%.2f > %.2f)\n", t0x > t1z, t0z > t1x, t0x, t1z, t0z, t1x);
			return false;
		}
		
		printf("passed\n");
		return true; 
	}
	
};

class BasicObject{
private:
	Mesh *mesh;
	BoundingBox bounding_box;
	bool bounding_box_set = false;
	
	glm::vec3 near;
	glm::vec3 far;
	glm::vec3 bounds[2];
	
	glm::mat4 model;
	bool model_set = false;
	
//	glm::mat3
	
	// function source
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	// pos - ray begin
public:
	BasicObject(){
		
	}
	
	bool selected = false;

	void setMesh(Mesh *mesh){
		this->mesh = mesh;
	}
	
	void setModel(glm::mat4 model){
		this->model = model;
		this->model_set = true;
	}
	
	glm::mat4 *get_model(){
		return &this->model;
	}
	
	Mesh *get_mesh(){
		return this->mesh;
	}
	
	void setBoundingBox(BoundingBox bb){
		this->bounding_box = bb;
		bounding_box_set = true;
	}
	
	bool box_intersect(glm::vec3 pos, glm::vec3 ray){
		if(this->bounding_box_set)
			return this->bounding_box.ray_intersect(pos, ray, this->model);
		return false;
	}
	
	char get_plane(glm::vec3 pos, glm::vec3 ray){
		if(this->bounding_box_set)
			return this->bounding_box.get_plane(pos, ray, this->model);
		return -1;
	}
	
	void draw(){
		if(!this->model_set){
			this->mesh->draw();
		}else{
			this->mesh->draw(this->model, this->selected);
		}
	}
	
	void draw(glm::mat4 model){
		this->mesh->draw(model);
	}
	
};