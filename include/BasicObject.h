#pragma once

#include "Mesh.h"
#include "gtx/matrix_decompose.hpp"
#include <vector>
#include "src/triangles/vector2.h"
#include "src/triangles/triangle.h"
#include "src/triangles/delaunay.h"
#include "src/Plane.h"

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
	glm::vec2 last_point = glm::vec2(0.0f);
	
public:
	BoundingBox(){}
	
	BoundingBox(glm::vec3 points[8], glm::vec3 center){
		this->set_points(points, center);
	}
	
	BoundingBox(glm::vec3 center, float width){
//		printf("setting center: (%.2f %.2f %.2f)\n", center.x, center.y, center.z);
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
	
	glm::vec2 get_last_plane_point(){
		return this->last_point;
	}
	
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
	
	bool plane_intersect(glm::vec3 ray, glm::vec3 ray_point, glm::vec3 plane_norm, glm::vec3 plane_point, int planeid, glm::mat4 model = glm::mat4(1.0f)){
		plane_norm *= -1;
		float denom = glm::dot(plane_norm, ray);
		if(denom > 1e-6){
			glm::vec3 prvec = plane_point - ray_point;
			float t = (glm::dot(prvec, plane_norm) / denom);
			prvec = ray_point + ray * t - plane_point;
			glm::vec2 point_relative;
			float width = this->width;
			float height = this->width;
			glm::vec3 model_scale, gb;
			glm::quat qb;
			glm::vec4 gb4;
			glm::decompose(model, model_scale, qb, gb, gb, gb4);
			
			switch(planeid){
				case 0: 
					point_relative.x = prvec.x;
					point_relative.y = prvec.y;
					width *= model_scale.x;
					height*= model_scale.y;
				break;
				case 1: 
					point_relative.x = -prvec.z;
					point_relative.y = prvec.y;
					width *= model_scale.z;
					height*= model_scale.y;
				break;
				case 2: 
					point_relative.x = -prvec.x;
					point_relative.y = prvec.y;
					width *= model_scale.x;
					height*= model_scale.y;
				break;
				case 3: 
					point_relative.x = prvec.z;
					point_relative.y = prvec.y;
					width *= model_scale.z;
					height*= model_scale.y;
				break;
				case 4:
					point_relative.x = prvec.x;
					point_relative.y = -prvec.z;
					width *= model_scale.x;
					height*= model_scale.z;
				break;
				case 5:
					point_relative.x = prvec.x;
					point_relative.y = prvec.z;
					width *= model_scale.x;
					height*= model_scale.z;
				break;
			}
			
			last_point = point_relative;
			
			if( t >= 0 && (point_relative.x >= -width && point_relative.x <= width) && (point_relative.y >= -height && point_relative.y <= height)){
				return true;
			}
		}
		return false;
	}
	
	char get_plane(glm::vec3 origin, glm::vec3 dir, glm::mat4 model){
		int closest_id;
		this->get_closest(origin, model, &closest_id);
		
//		printf("planes for %d: ", closest_id);
		for(int i = 0; i < 3; i++){
//			printf("%d ", this->vertex_sides[closest_id][i]);
			glm::vec3 plane_normal = this->sides_normals[this->vertex_sides[closest_id][i]];
			glm::vec3 plane_point = model * (glm::vec4(plane_normal, 1/this->width) * this->width);
//			printf("plane_point: (%.2f %.2f %.2f)\n", plane_point.x, plane_point.y, plane_point.z);
			if(plane_intersect(dir, origin, plane_normal, plane_point, this->vertex_sides[closest_id][i], model)){
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
		
		if( (t0x > t1y) || (t0y > t1x)){
			return false;
		}
		
		if(t0y > t0x){
			t0x = t0y;
		}
			
		if(t1y < t1x){
			t1x = t1y;
		}
		
		float t0z = (bounds[0].z - origin.z) / dir.z;
		float t1z = (bounds[1].z - origin.z) / dir.z;
		
		if(t0z > t1z){
			float t = t0z;
			t0z = t1z;
			t1z = t;
		}
		
		if(t0x > t1z || t0z > t1x ){
			return false;
		}
		
		return true; 
	}
	
};



class BasicObject2{
public:
	Mesh *mesh;
//	std::vector <glm::vec3> vertices = {
	glm::vec3 vertices[VERTICES_SIZE] {
		glm::vec3(-0.5f, 0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, -0.5f, 0.5f),
		glm::vec3(0.5f, -0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f)
	};
	
	struct plane_struct{
		std::vector<Plane> front;// = {Plane()};
		std::vector<Plane> right;
		std::vector<Plane> back;
		std::vector<Plane> left;
		std::vector<Plane> top;
		std::vector<Plane> bottom;
	}planes;
	
	bool selected = true;
	int side = -1;
	int side_pos = 0;
	
	BasicObject2(){
		glm::vec3 *temp_face[] = {vertices, vertices + 1, vertices + 2, vertices + 3};
		planes.front.push_back(Plane(temp_face, 4));
		planes.front[0].center = glm::vec3(0.0f, 0.0f, 0.5f);
		planes.front[0].face_normal = glm::vec3(0.0f, 0.0f, 1.0f);
//		
		temp_face[0] = vertices + 1; temp_face[1] = vertices + 5; temp_face[2] = vertices + 3; temp_face[3] = vertices + 7; 
		planes.right.push_back(Plane(temp_face, 4));
		planes.right[0].center = glm::vec3(0.5f, 0.0f, 0.0f);
		planes.right[0].face_normal = glm::vec3(1.0f, 0.0f, 0.0f);
		
		temp_face[0] = vertices + 5; temp_face[1] = vertices + 4; temp_face[2] = vertices + 7; temp_face[3] = vertices + 6; 
		planes.back.push_back(Plane(temp_face, 4));
		planes.back[0].center = glm::vec3(0.0f, 0.0f, -0.5f);
		planes.back[0].face_normal = glm::vec3(0.0f, 0.0f, -1.0f);
		
		temp_face[0] = vertices + 4; temp_face[1] = vertices; temp_face[2] = vertices + 6; temp_face[3] = vertices + 2; 
		planes.left.push_back(Plane(temp_face, 4));
		planes.left[0].center = glm::vec3(-0.5f, 0.0f, 0.0f);
		planes.left[0].face_normal = glm::vec3(-1.0f, 0.0f, 0.0f);
//		
		temp_face[0] = vertices + 4; temp_face[1] = vertices + 5; temp_face[2] = vertices + 1; temp_face[3] = vertices;
		planes.top.push_back(Plane(temp_face, 4));
		planes.top[0].center = glm::vec3(0.0f, 0.5f, 0.0f);
		planes.top[0].face_normal = glm::vec3(0.0f, 1.0f, 0.0f);
		
		temp_face[0] = vertices + 7; temp_face[1] = vertices + 6; temp_face[2] = vertices + 2; temp_face[3] = vertices + 3;
		planes.bottom.push_back(Plane(temp_face, 4));
		planes.bottom[0].center = glm::vec3(0.0f, -0.5f, 0.0f);
		planes.bottom[0].face_normal = glm::vec3(0.0f, -1.0f, 0.0f);
		
		this->mesh = new Mesh();
		this->mesh->init_shaders("vertexShader.vs", "fragmentShader.fs");
		
		unsigned int data_size;
		float* data = this->get_data(&data_size);
//		this->mesh->init_vabo(data, data_size);
		
		delete[] data;
	}
	
	void click_at(glm::vec3 ray_pos, glm::vec3 ray_dir){
		std::vector<Plane>* cur_plane = &(planes.front);
		
		side = -1;
		selected = false;
		bool found = false;
		for(int k = 0; k < 6; k++){
			for(int i = 0; i < (*(cur_plane + k)).size(); i++){
				Plane temp = (*(cur_plane + k))[i];
				if(temp.ray_intersects(ray_pos, ray_dir)){
					this->side = k;
					this->side_pos = i;
					this->selected = true;
//					printf("side: %d, ", k);
//					printf("pos: %d\n", i);
					found = true;
					break;
				}
			}
			if(found)
				break;
		}
	}
	
	void isfuckingtwo(){
		std::vector<Plane>* cur_plane = &(planes.front);
//		for(int k = 0; k < 6; k++){
		if(side != -1){
			glm::vec3 min = (*(cur_plane + side))[0].min_corner;
			glm::vec3 max = (*(cur_plane + side))[0].max_corner;
			printf("min (%.2f %.2f), max (%.2f %.2f)\n", min.x, min.y, max.x, max.y);
		}
		
	}
	
	void set_view_projection(glm::mat4 *projection, glm::mat4 *view){
		this->mesh->set_mats_pointers(projection, view);
	}
	
	void update_data(){
		unsigned int tsize;
		float *tdata = this->get_data(&tsize);
//		this->mesh->update_data(tdata, tsize);
		delete[] tdata;
	}
	
	std::vector<Plane>* get_current_plane(int id){
		if(this->side == -1)
			return NULL;
	}
	
	void extrude(float val){
		if(this->side != -1){
			(*(&planes.front + side))[side_pos].extrude(val);
			
			for(int i = 0; i < 6; i++){
				std::vector<Plane> *t = (&(this->planes.front) + i);
				for(int j = 0; j < t->size(); j++){
					(*t)[j].update_bounds();
				}
			}
			
			this->update_data();
		}
	}
	
	std::vector<Plane> *getPlanePtr(){
		return (&(this->planes.front) + this->side);
	}
	
	void split(){
		if(this->side != -1){
			Plane tplane = (*getPlanePtr())[this->side_pos]; // plane to split
			glm::vec2 last_mouse = tplane.get_last_mouse();
			
			glm::mat4 fromMouseRotation = glm::transpose(get_rotation_to_xy(tplane.face_normal));
			
			glm::vec3 top(last_mouse.x, tplane.max_corner.y, 0.0f);
			glm::vec3 bottom(last_mouse.x, tplane.min_corner.y, 0.0f);
			
			top = fromMouseRotation * glm::vec4(top, 1.0f);
			bottom = fromMouseRotation * glm::vec4(bottom, 1.0f);
			
			
		}
	}
	
	float *get_data(unsigned int *size){
//		planes.front
		std::vector<float> ret;
		
//		printf("\n");
		std::vector<Plane>* cur_plane = &(planes.front);
		for(int k = 0; k < 6; k++){
			for(unsigned int i = 0; i < (*(cur_plane + k)).size(); i++){
				Plane t = (*(cur_plane + k))[i];
				unsigned int size;
				float *tdata = t.get_buffer_data(&size);
				for(int j = 0; j < size; j++){
//					if(j > 0 && j % 6 == 0) printf("\n");
//					printf("%.2f ", tdata[j]);
					ret.push_back(tdata[j]);
				}
				delete[] tdata;
//				printf("\n");
			}
		}
		float *pret = new float[ret.size()];
		memcpy(pret, ret.data(), ret.size() * 4);
		
		if(size != NULL)
			*size = ret.size();
		
		return pret;
	}
	
	void draw(glm::vec3 light_position){
//		this->mesh->draw(glm::mat4(1.0f), this->selected, this->side, light_position);
	}
};

class BasicObject{
private:
	BoundingBox bounding_box;
	bool bounding_box_set = false;
	
	glm::vec3 near;
	glm::vec3 far;
	glm::vec3 bounds[2];
	
	glm::mat4 base_model = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	
	// extrude variables
	//
	glm::vec3 extruded = glm::vec3(1.0f);
	glm::vec3 center = glm::vec3(0.0f);
	
	
	// function source
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
	// pos - ray begin
public:
	Mesh *mesh;
	bool model_set = false;
	BasicObject(){
		
	}
	
	bool selected = false;
	int side = -1;
	
	void extrude(int side, float value){
		switch(side){
			case 0:
				this->extruded.z += value;
				this->center.z += value / 2;
			break;
			case 1:
				this->extruded.x += value;
				this->center.x += value / 2;
			break;
			case 2:
				this->extruded.z += value;
				this->center.z -= value / 2;
			break;
			case 3:
				this->extruded.x += value;
				this->center.x -= value / 2;
			break;
			case 4:
				this->extruded.y += value;
				this->center.y += value / 2;
			break;
			case 5:
				this->extruded.y += value;
				this->center.y -= value / 2;
			break;
		}
		
		this->model = glm::mat4(1.0f);
		this->model = glm::translate(this->base_model, this->center);
		this->model = glm::scale(this->model, this->extruded);
		
		printf("center: (%.2f %.2f %.2f)\nextruded: (%.2f %.2f %.2f)\n", this->center.x, this->center.y, this->center.z, this->extruded.x, this->extruded.y, this->extruded.z);
	}
	
	
	// position - counting from the center, cant be more or less than extruded / 2
	BasicObject split(enum split_direction dir){
		if(side == 0 || side == 2){
			if(dir == VERTICAL){
				glm::vec2 point = this->bounding_box.get_last_plane_point();
				if(side == 2)
					point.x *= -1;
				
				glm::vec3 new_extruded = this->extruded;
				new_extruded.x /= 2;
				new_extruded.x -= point.x;
				glm::vec3 new_center = this->center;
				new_center.x += point.x + new_extruded.x / 2;
				
				glm::vec3 ret_extruded = this->extruded;
				ret_extruded.x -= new_extruded.x;
				if(ret_extruded.x < 0) ret_extruded.x *= -1;
				
				this->extruded = new_extruded;
				
				glm::vec3 ret_center = this->center;
				ret_center.x += point.x;
				ret_center.x -= ret_extruded.x / 2;
				
				this->center = new_center;
				
				this->extrude(0, 0.0f);
				
				BasicObject ret;
				ret.center = ret_center;
				ret.extruded = ret_extruded;
				ret.setMesh(this->mesh);
				ret.setBoundingBox(this->bounding_box);
				ret.setModel(glm::mat4(1.0f));
				ret.extrude(0, 0.0f);
				
				return ret;
			}
		}
	}
	
	void setMesh(Mesh *mesh){
		this->mesh = mesh;
	}
	
	void setModel(glm::mat4 model){
		this->base_model = model;
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
	
	void draw(glm::vec3 light_position = glm::vec3(1.0f, 2.0f, 1.0f)){
		if(!this->model_set){
//			this->mesh->draw();
		}else{
//			this->mesh->draw(this->model, this->selected, this->side, light_position);
		}
	}
	
	void draw(glm::mat4 model){
//		this->mesh->draw(model);
	}
	
};