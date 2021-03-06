#include "Chunk.h"
#include "WorldLoader.h"

char terrainballs(int x, int y, int z){
//	if(y == 0 && z == 0)
//		printf("x: %d\n", x);
	
//	int ir = 100;
//	int margin = -29;
	int ir = 100;
	int margin = -30;
	
	if(x < 0)
		x *= -1;
//	if(y < 0)
//		y *= -1;
	if(z < 0)
		z *= -1;
	
	x %= (ir + margin) * 2;
//	y %= (ir + margin) * 2;
	z %= (ir + margin) * 2;
	
//	float fx = (float)x;
//	float fy = (float)y;
//	float fz = (float)z;
////	float xyval = (rand() % 100) / 25.0f + 4; sin((fz + fx) / 5.0)* 4 + 4;
//	float xyval = sin((fz) / 5.0)* 3 * cos((fx) / 5.0)* 3 + 7;
//	float xyval = ((x) % 16) * 1.0f + 8;
	x -= ir + margin;
//	y -= ir + margin;
	int ysph = y + 50;
	z -= ir + margin;
//	return 1;
//	if((x*x + y*y + z*z) <= (ir*ir) || y <= 0){
	if((x*x + ysph*ysph + z*z) <= (ir*ir) || y <= 0){
		float ts = sinf(x / 10.f) * cosf(z / 10.0f) * sinf(y / 10.0f);
		char type = (ts > 0) + rand() % 2 + 1;
		
		return type;
	}

	return 0;
}

char terrain(int x, int y, int z){
//	float divident = 5.0f;
//	float xy = sin(x / divident) * cos(z / divident);
//	xy *= divident;
//	
//	float div2 = 50.0f;
//	float xpos = 10;
//	float ypos = -15;
//	float mod2 = sin((x + xpos) / div2) * cos((z + ypos) / div2) * 5;
	
//	float xp = (sin(x / 5.0f))* 1 + (cos(x/100.0f) + 1) * 2 ;
//	float yp = (sin(z / 5.0f)) * 1 + (cos(z/100.0f) + 1) * 2 ;
//	
////	xy = mod2 + xy;
//	float xy = xp*yp;

	if(x < 0) x = -x;
	if(z < 0) z = -z;
//	char xy = ((y) <= abs( ( (x+8) % 16) - 8)) + 16 && ((y) <= abs((z+8) % 16 - 8)) + 16 && ((y) >= -abs( ( (x+8) % 16) - 8)) + 16 && ((y) >= -abs((z+8) % 16 - 8)) + 16;
	
	char xy = x * z + 12;
	
	if(y < xy){
		if(y < 3)
			return 2;
		return 1;
	}
		
	return 0;
	
}

Chunk& Chunk::operator=(Chunk&& t){
	if(this != &t){
		mesh = std::move(t.mesh);
		this->parentLoader = t.parentLoader;
		this->doNotDraw = t.doNotDraw;
		this->gridOffset = t.gridOffset;
		this->fullOrEmpty = t.fullOrEmpty;
		this->data = t.data;
		this->data_offset = t.data_offset;
		this->worldOffset = t.worldOffset;
	}
	
	return *this;
}

Chunk::Chunk(glm::mat4 *projection, glm::mat4 *view, unsigned int& textureId, shaderParams &shParams, glm::ivec3 offset, WorldLoader *loaderPtr, char* data, int data_offset){
	parentLoader = loaderPtr;
	float fchunkSize = (float)chunkSize;
	this->worldOffset = offset * this->size;//glm::vec3(offset.x * fchunkSize, offset.y * fchunkSize, offset.z * fchunkSize);
	
	this->gridOffset = offset;
	this->mesh = std::make_unique<Mesh>();
//	printf("Chunk %2d %2d %2d\n", worldOffset.x, worldOffset.y, worldOffset.z);
	
//	asociatedRegion = parentLoader->getRegion(worldOffset.x, worldOffset.y, worldOffset.z);
	
	this->data = data;
	this->data_offset = data_offset;
	
	// AO DEBUG
//	if(offset != glm::ivec3(0, 0, 0)){
//		this->data = nullptr;
//	}else{
//		for(int z = 0; z < size; z++){
//			for(int y = 0; y < size; y++){
//				for(int x = 0; x < size; x++){
//					this->data[z * size_cubed + y * size * size + x] = terrain(x, y, z);
//				}
//			}
//		}
//	}
	// AO DEBUG
	
//	Region& containingRegion = parentLoader->getRegion(worldOffset.x, worldOffset.y, worldOffset.z);
//	if(containingRegion.type != NULL_REGION)
//		this->data = containingRegion.getChunk(worldOffset.x, worldOffset.y, worldOffset.z);
//	else
//		this->data = NULL;
//	this->data = 
	
//	if(asociatedRegion.get() != NULL){
//		// this should be changed to coordinates within the chunk instead of global.
//		this->data = asociatedRegion->getChunk(worldOffset.x, worldOffset.y, worldOffset.z);
////		printf("data get [%d %d %d] %p\n", offset.x, offset.y, offset.z, this->data);
//	}else{
////		printf("data nul [%d %d %d]\n", offset.x, offset.y, offset.z);
//	}
	
	this->init_data();
	
	this->mesh->setShaderParams(shParams);
	mesh->set_mats_pointers(projection, view);
	mesh->setTexturesId(textureId);
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(this->worldOffset));//glm::vec3(offset.x * fchunkSize, offset.y * fchunkSize, offset.z * fchunkSize));
	
}
	
void Chunk::draw(){
	if(!(fullOrEmpty || doNotDraw))
		this->mesh->draw();
}

void Chunk::changeBlock(const glm::ivec3& data_pos, BlockAction& action){
	int arr_index = data_pos.z * (size * size) + data_pos.y * size + data_pos.x;
	
	if(action == BlockAction::DESTROY)
		data[data_offset + arr_index] = 0;
	else
		data[data_offset + arr_index] = 2;
	
	update_data();
}

void Chunk::update_data(){
	std::vector<unsigned int> intdata;
	std::vector<float> floatdata;
	
	std::tie(floatdata, intdata) = this->getShaderData();
	
	if(floatdata.size() > 0){
		this->mesh->updateVao(floatdata, intdata);
	}
}

void Chunk::init_data(){
	std::vector<unsigned int> intdata;
	std::vector<float> floatdata;
	
	std::tie(floatdata, intdata) = this->getShaderData();
	
	if(floatdata.size() > 0){
		this->mesh->initVao(floatdata, intdata);
	}
}

char Chunk::valueAt(const glm::ivec3& pos){
	if(data == nullptr)
		return 0;
	
	
	if((pos.x < 0 || pos.x >= chunkSize) || (pos.y < 0 || pos.y >= chunkSize) || (pos.z < 0 || pos.z >= chunkSize)){
//		return 0; // AO DEBUG
		
		if(parentLoader != nullptr)
			return parentLoader->valueAt(pos.x + worldOffset.x, pos.y + worldOffset.y, pos.z + worldOffset.z);
		else 
			return 0;
	}
	
	int arr_index = pos.z * (size * size) + pos.y * size + pos.x;
	
//	return valueAt(pos.x, pos.y, pos.z);
	return data[data_offset + arr_index];
}

inline int calculate_ao(int side1, int side2, int corner){
	if(side1 && side2)
		return 0;
		
	return 3 - (side1 + side2 + corner);
}

inline void add_vertex(std::vector<float>& vertices, std::vector<unsigned int>& properties, const glm::vec3& vec, const glm::vec2& tex, const unsigned int& prop){
	vertices.push_back(vec.x); 
	vertices.push_back(vec.y); 
	vertices.push_back(vec.z); 
	
	vertices.push_back(tex.x); 
	vertices.push_back(tex.y); 
	vertices.push_back(0.0f);
	properties.push_back(prop); 
}

// https://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
// https://github.com/AThilenius/greedy_meshing/blob/master/greedy_meshing.ts
std::tuple<std::vector<float>, std::vector<unsigned int>> Chunk::getShaderData(){
	std::vector<float> ret; // Vertices and thier texture coordinates.
	std::vector<unsigned int> int_ret; // Parameters for each vertex ( texture id, ao) not to be interpolated along the triangle.

	if(!data){
		return std::make_tuple(ret, int_ret);
	}
	
	char mask[size][size];
	char ao_mask[size + 1][size + 1];
	
	int randd = rand() & 3; 
	
	for(int side = 1; side > -2; side -= 2){
//	for(int side = 1; side > -1; side -= 2){
//	for(int side = -1; side > -2; side -= 2){
		for(int norm = 0; norm < 3; norm++){
//		for(int norm = 0; norm < 1; norm++){
			int tan = (norm + 1) % 3;
			int biTan = (norm + 2) % 3;
			
//			int normal[3] = {0, 0, 0};
			glm::ivec3 normal(0);
			normal[norm] = side;
			
			for(int slice = 0; slice < size; slice++){
//				int cursor[3] = {0, 0, 0};
				glm::ivec3 cursor(0);
				cursor[norm] = slice;
				
				// Creating mask of current layer
				for(cursor[biTan] = 0; cursor[biTan] < size; cursor[biTan]++){
					for(cursor[tan] = 0; cursor[tan] < size; cursor[tan]++){
	//					
						char valueHere = valueAt(cursor);
//						char valueHere = valueAt(cursor) * randd;
						
						if(valueHere && !valueAt(cursor - normal)){
							mask[cursor[biTan]][cursor[tan]] = valueHere;
						}else{
							mask[cursor[biTan]][cursor[tan]] = 0;
						}
					}
				}
				
				for(int i = 0; i < size + 1; i++){
					for(int j = 0; j < size + 1; j++){
						ao_mask[i][j] = -1;
					}
				}
				
				// Creating ambient occlusion mask
				int y;
				for(cursor[biTan] = 0, y = 0; cursor[biTan] < size; cursor[biTan]++, y++){
					int x;
					for(cursor[tan] = 0, x = 0; cursor[tan] < size; cursor[tan]++, x++){
						if(mask[y][x] == 0)
							continue;
							
						glm::ivec3 up_cursor = cursor - normal;
						
						if(ao_mask[y][x] == -1){ // v0
							glm::ivec3 left, up, corner;
							
							left = up_cursor;
							left[tan] -= 1;
							
							up = up_cursor;
							up[biTan] -= 1;
							
							corner = up_cursor;
							corner[tan] -= 1;
							corner[biTan] -= 1;
							
							ao_mask[y][x] = calculate_ao(valueAt(left) != 0, valueAt(up) != 0, valueAt(corner) != 0);
						}
						
						if(ao_mask[y][x + 1] == -1){ // v1
							glm::ivec3 left, up, corner;
							
							left = up_cursor;
							left[tan] += 1;
							
							up = up_cursor;
							up[biTan] -= 1;
							
							corner = up_cursor;
							corner[tan] += 1;
							corner[biTan] -= 1;
							
							ao_mask[y][x + 1] = calculate_ao(valueAt(left) != 0, valueAt(up) != 0, valueAt(corner) != 0);
						}
						
						if(ao_mask[y + 1][x] == -1){ // v2
							glm::ivec3 left, up, corner;
							
							left = up_cursor;
							left[tan] -= 1;
							
							up = up_cursor;
							up[biTan] += 1;
							
							corner = up_cursor;
							corner[tan] -= 1;
							corner[biTan] += 1;
							
							ao_mask[y + 1][x] = calculate_ao(valueAt(left) != 0, valueAt(up) != 0, valueAt(corner) != 0);
						}
						
						if(ao_mask[y + 1][x + 1] == -1){ // v3
							glm::ivec3 left, up, corner;
							
							left = up_cursor;
							left[tan] += 1;
							
							up = up_cursor;
							up[biTan] += 1;
							
							corner = up_cursor;
							corner[tan] += 1;
							corner[biTan] += 1;
							
							ao_mask[y + 1][x + 1] = calculate_ao(valueAt(left) != 0, valueAt(up) != 0, valueAt(corner) != 0);
						}
					}
				}
				
				for(int y = 0; y < size; y++){
					for(int x = 0; x < size; ){
						if(mask[y][x] == 0){
							x++;
							continue;
						}
						
						char blockType = mask[y][x]; // używane do sprawdzania, czy następny blok jest tego samego typu
						char aoVal[4] = {
							ao_mask[y][x],
							ao_mask[y][x + 1],
							ao_mask[y + 1][x],
							ao_mask[y + 1][x + 1]
						};

						int width = 1;
//						for(int tx = x + 1; tx < size && mask[y][tx] == blockType; ){
						for(int tx = x + 1; tx < size && mask[y][tx] == blockType; ){
//							break;
							if(ao_mask[y][tx] != aoVal[0] || ao_mask[y][tx + 1] != aoVal[1] || ao_mask[y + 1][tx] != aoVal[2] || ao_mask[y + 1][tx + 1] != aoVal[3])
								break;
							tx++, width++;
						}
						
						int height = 1;
						bool valid = true;
						for(int ty = y + 1; ty < size && valid; ty++){
//							break;
							for(int tx = x; tx < x + width; tx++){
								if((mask[ty][tx] != blockType)){
									valid = false;
									break;
								}
								
								if(ao_mask[ty][tx] != aoVal[0] || ao_mask[ty][tx + 1] != aoVal[1] || ao_mask[ty + 1][tx] != aoVal[2] || ao_mask[ty + 1][tx + 1] != aoVal[3]){
									valid = false;
									break;
								}
							}
							height += valid;
						}
						
						for(int l = y; l < y + height; l++){
							for(int k = x; k < x + width; k++){
								mask[l][k] = 0;
							}
						}
						
						//float b[3]; // base of the quad
						glm::vec3 b(0.0f);// base of the quad
						b[norm] = slice + (side == -1);
						b[tan] = x;
						b[biTan] = y;
						
						// width of the quad
//						float du[3] = {0};
						glm::vec3 du(0.0f);
						du[tan] = width;
						
						//height of the quad
//						float dv[3] = {0};
						glm::vec3 dv(0.0f);
						dv[biTan] = height;
						
						bool top = normal[1] == -1;
						
						// ###
						// 		INFORMATIVE COMMENT, DON'T DELETE
						// ###
						//
						// texId structure 
						// ????????|????????|??????aat|bbbbbbbb
						// ????????|fordebug|??????aat|bbbbbbbb
						// b - block type
						// t - does this vertex belong to a top side (should be changed to distinguish all sides)
						// a - ambient occlusion value
						int texid = static_cast<int>(blockType);
						if(top){
							texid |= 0x100;
						}
						
						texid |= (y << 4 | x) << 16;
						
						int ao[4] = {
							ao_mask[y][x] << 9,
							ao_mask[y][x + 1] << 9,
							ao_mask[y + 1][x] << 9,
							ao_mask[y + 1][x + 1] << 9
						};
						
//						if(side == 1 || norm != 1){
//							for(int ef = 0; ef < 4; ef++)
//								ao[ef] = 3 << 9;
//						}
						
						glm::vec2 size_2d = glm::vec2(width, height);
//						glm::vec2 size_2d = glm::vec2(1, 1);
						
						if(ao[0] + ao[3] < ao[1] + ao[2]){ //flipped
							add_vertex(ret, int_ret, b + dv, 			planeTexcoords[3] * size_2d, texid | ao[2]);
							add_vertex(ret, int_ret, b, 				planeTexcoords[0] * size_2d, texid | ao[0]);
							add_vertex(ret, int_ret, b + du, 			planeTexcoords[1] * size_2d, texid | ao[1]);
							add_vertex(ret, int_ret, b + du, 			planeTexcoords[1] * size_2d, texid | ao[1]);
							add_vertex(ret, int_ret, b + du + dv, 		planeTexcoords[2] * size_2d, texid | ao[3]);
							add_vertex(ret, int_ret, b + dv, 			planeTexcoords[3] * size_2d, texid | ao[2]);
						}else{ // normal
							add_vertex(ret, int_ret, b, 			planeTexcoords[0] * size_2d, texid | ao[0]);
							add_vertex(ret, int_ret, b + du, 		planeTexcoords[1] * size_2d, texid | ao[1]);
							add_vertex(ret, int_ret, b + du + dv, 	planeTexcoords[2] * size_2d, texid | ao[3]);
							add_vertex(ret, int_ret, b + du + dv, 	planeTexcoords[2] * size_2d, texid | ao[3]);
							add_vertex(ret, int_ret, b + dv, 		planeTexcoords[3] * size_2d, texid | ao[2]);
							add_vertex(ret, int_ret, b,			 	planeTexcoords[0] * size_2d, texid | ao[0]);
						}
						
						x += width;
						randd = (randd + 51) & 3; 
					}
				}
			}
		}
	}
	
	fullOrEmpty = ret.size() == 0;
	
	return std::make_tuple(ret, int_ret);
}

Chunk::~Chunk(){
	if(doNotDraw){
//		printf("Not drawable destroyed\n");
	}
}