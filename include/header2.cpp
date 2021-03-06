#include "header2.h"

float *vertices_get2(int *vcount){
	*vcount = 0;
	glm::vec3 square_vertices[6] = {
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f, 0.0f)
	};
	
	glm::vec3 square_normals[6] = {
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	};
	
	
	glm::vec3 side_normal[6] = {
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	};
	
	glm::vec3 rotation_vectors[6] = {
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};
	
	float rotation_angles[6] = {
		0.0f,
		-90.0f,
		-180.0f,
		-270.0f,
		90.0f,
		-90.0f
	};
	
	float *vertices = (float *)malloc(sizeof(float) * (3 + 3) * 6 * 6);
	if(vertices != NULL){
		int count = 0;
		for(int i = 0; i < 6; i++){
			glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_angles[i]), rotation_vectors[i]);
//			glm::mat4 translate = glm::translate(trans, side_normal[i]);
//			trans = trans * translate;
//			trans = translate;
			for(int j = 0; j < 6; j++){
				glm::vec4 tvec = glm::vec4(square_vertices[j], 1.0f);
				glm::vec4 tnorm = glm::vec4(side_normal[i], 1.0f);
				
				tvec = trans * tvec;
				tvec += tnorm;
//				tnorm = trans * tnorm;
				
//				printf("vec%d:\n", i);
//				printf("x = %f, y = %f, z = %f, n = %f, n = %f, n = %f\n", tvec.x, tvec.y, tvec.z, tnorm.x, tnorm.y, tnorm.z);
				
				*(vertices + count++) = tvec.x;
				*(vertices + count++) = tvec.y;
				*(vertices + count++) = tvec.z;
				*(vertices + count++) = tnorm.x;
				*(vertices + count++) = tnorm.y;
				*(vertices + count++) = tnorm.z;
//				*(vertices + (6) * 6 * i + (3 + 3) * j + 0) = tvec.x;
//				*(vertices + (6) * 6 * i + (3 + 3) * j + 0) = tvec.y;
//				*(vertices + (6) * 6 * i + (3 + 3) * j + 0) = tvec.z;
//				*(vertices + (6) * 6 * i + (3 + 3) * j + 0) = tnorm.x;
//				*(vertices + (6) * 6 * i + (3 + 3) * j + 0) = tnorm.y;
//				*(vertices + (6) * 6 * i + (3 + 3) * j + 0) = tnorm.z;
			}
		}
		*vcount = (3 + 3) * 6 * 6;
	}
	
	return vertices;
}

float *vertices_get(){
	float vertices2[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	
	glm::vec3 vertices_sauce[] = {
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f)
	};
	
	glm::vec3 normals_sauce[] = {
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	};
	
	int a = 9;
	a *= 2;
	
	
//	trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//	printf("mat: \n %3.3f %3.3f %3.3f %3.3f\n%3.3f %3.3f %3.3f %3.3f\n%3.3f %3.3f %3.3f %3.3f\n%3.3f %3.3f %3.3f %3.3f\n",
//	trans[0], trans[1], trans[2], trans[3],
//	trans[4], trans[2], trans[6], trans[7], 
//	trans[8], trans[9], trans[10], trans[11], 
//	trans[12], trans[13], trans[14], trans[15]);
	float *vertices = (float *)malloc(sizeof(float) * a);
	if(vertices != NULL){
		for(int i = 0; i < 3; i++){
			glm::vec3 vert = *(vertices_sauce + i);
			glm::vec4 t = glm::vec4(vert.x, vert.y, vert.z, 1.0f);// * trans;
//			glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians());
//			t = trans * t;
			printf("translated: x = %f, y = %f, z = %f\n", t.x, t.y, t.z);
//			*(vertices + i*6 + 0) = vertices_sauce[i].x;
//			*(vertices + i*6 + 1) = vertices_sauce[i].y;
//			*(vertices + i*6 + 2) = vertices_sauce[i].z;
			*(vertices + i*6 + 0) = t.x;
			*(vertices + i*6 + 1) = t.y;
			*(vertices + i*6 + 2) = t.z;
			*(vertices + i*6 + 3) = normals_sauce[i].x;
			*(vertices + i*6 + 4) = normals_sauce[i].y;
			*(vertices + i*6 + 5) = normals_sauce[i].z;
//			*(vertices + i) = vertices2[i];
			
		}
	}
		
	return vertices;
}