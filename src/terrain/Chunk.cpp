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

Chunk::Chunk(const unsigned int& model_location, glm::ivec3 offset, WorldLoader *loaderPtr, region_dtype* data, int data_offset){
	parentLoader = loaderPtr;
//	float fchunkSize = (float)chunkSize;
	this->worldOffset = offset * this->size;//glm::vec3(offset.x * fchunkSize, offset.y * fchunkSize, offset.z * fchunkSize);
	
	this->gridOffset = offset;
	this->mesh = std::make_unique<Mesh>();
//	printf("Chunk %2d %2d %2d\n", worldOffset.x, worldOffset.y, worldOffset.z);
	
//	asociatedRegion = parentLoader->getRegion(worldOffset.x, worldOffset.y, worldOffset.z);
	
	this->data = data;
	this->data_offset = data_offset;
	
//	this->init_data();
	
	mesh->setModelLocation(model_location);
	mesh->model = glm::translate(glm::mat4(1.0f), glm::vec3(this->worldOffset));//glm::vec3(offset.x * fchunkSize, offset.y * fchunkSize, offset.z * fchunkSize));
	
}
	
void Chunk::draw(){
	if(!(fullOrEmpty || doNotDraw))
		this->mesh->draw();
}

void Chunk::changeBlock(const int& block_type, const glm::ivec3& data_pos, BlockAction& action){
	int arr_index = data_pos.z * (size * size) + data_pos.y * size + data_pos.x;
	
	if(action == BlockAction::DESTROY){
		data[data_offset + arr_index] &= 0xff00;
		printf("Chunk.cpp 123 destroy block, data: %x\n", data[data_offset + arr_index]);
	}else if(getBlockType(data[data_offset + arr_index]) == 0){
		data[data_offset + arr_index] |= block_type;
	}
	
	update_data();
}

// std::tuple<unsigned int, unsigned int, unsigned int> Chunk::getBuffers () {
// 	return mesh->getBuffers();
// }

// Called from separate thread.
void Chunk::prepareUpdateData() {
	std::tie(floatMeshData, intMeshData) = getShaderData();
	
	if (floatMeshData.size() > 0) {
		pendingUpdate = true;
	}
}

// Called from drawing thread.
void Chunk::update_data() {
	if (pendingUpdate) {
		this->mesh->updateVao(floatMeshData, intMeshData);
		pendingUpdate = false;
		floatMeshData.clear();
		intMeshData.clear();
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

std::array<region_dtype, Chunk::size_squared> Chunk::getBottomLayer(){
	std::array<region_dtype, size_squared> bottom;
	
	const int y = size - 1;
	for(int z = 0, arr_z = 0; z < size; z++, arr_z += size){
		for(int x = 0; x < size; x++){
			
			int arr_index = z * (size * size) + y * size + x;
			bottom[arr_z + x] = data[data_offset + arr_index];
		}
	}
	
	return bottom;
}

// Teraz propagation pomiędzy chunkami.
	// std::list<propagateParam> sunlightPass(int mask[size][size], bool& allDark);
std::list<propagateParam> Chunk::sunlightPass(int mask[size][size], bool& allDark) {
	std::list<propagateParam> spread_queue;
	
	for(int y = size - 1; y >= 0; y--){
		allDark = true;
		for(int z = 0; z < size; z++){
			for(int x = 0; x < size; x++){
				
				int index = z * size * size + y * size + x;
				region_dtype& block = data[data_offset + index];
				
				if(getBlockType(block) != 0){
					mask[z][x] = 0;
				}
				
				block &= 0xf0ff;
				block |= mask[z][x];
				
				if(getSunLight(block) > 0){
					allDark = false;
				}
			}
		}
		
		if(allDark){
//			printf("ALLDARK %d %d %d\n", worldOffset.x, worldOffset.y, worldOffset.z);
			break;
		}
	
		// jeżeli y > 0 to można ustawić te wartości dla warstwy poprzedniej w pętli wyżej.
		for(int z = 0; z < size; z++){
			for(int x = 0; x < size; x++){
				int index = z * size * size + y * size + x;
				region_dtype& block = data[data_offset + index];
				if(mask[z][x] != 0 && getBlockType(block) == 0){
//					glm::ivec3 base = gridOffset * size + glm::ivec3(x, y, z);
					const glm::ivec3 new_poses[6] = {
						glm::ivec3(-1, 0, 0),
						glm::ivec3(1, 0, 0),
						glm::ivec3(0, -1, 0),
						glm::ivec3(0, 1, 0),
						glm::ivec3(0, 0, -1),
						glm::ivec3(0, 0, 1)
					};
					
					for(int i = 0; i < 6; i++){
						spread_queue.push_back(propagateParam(block_position(glm::ivec3(x, y, z) + new_poses[i], gridOffset), 0xe00));
					}
				}
			}
		}
	}
	
	return spread_queue;
}

region_dtype Chunk::valueAt(const glm::ivec3& pos){
	if(data == nullptr)
		return 0;
	
	
	if((pos.x < 0 || pos.x >= size) || (pos.y < 0 || pos.y >= size) || (pos.z < 0 || pos.z >= size)){
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

inline int calculate_ao(int side1, int side2, int corner) {
	// side1 = block_type(side1) != 0;
	// side2 = block_type(side2) != 0;
	// corner = block_type(corner) != 0;
	side1 = block_type(side1) != 0 && block_type(side1) != BLOCK_NOT_FOUND;
	side2 = block_type(side2) != 0 && block_type(side2) != BLOCK_NOT_FOUND;
	corner = block_type(corner) != 0 && block_type(corner) != BLOCK_NOT_FOUND;
	
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

// Sun intensity can be attribute in shader

// https://0fps.net/2012/06/30/meshing-in-a-minecraft-game/
// https://github.com/AThilenius/greedy_meshing/blob/master/greedy_meshing.ts
std::pair<std::vector<float>, std::vector<unsigned int>> Chunk::getShaderData(){
	std::vector<float> ret; // Vertices and thier texture coordinates.
	std::vector<unsigned int> int_ret; // Parameters for each vertex ( texture id, ao) not to be interpolated along the triangle.

	if(!data){
		// return std::make_tuple(ret, int_ret);
		return {ret, int_ret};
	}
	
	region_dtype prev_layer[size][size];
	region_dtype mask[size][size];
	region_dtype mask_with_light[size][size];
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
				
				// Creating mask of previous layer to take into account light.
//				if(slice > 0){ // Lookup from other blocks isn't too noticable (in fps when using single thread), 
								// but could be done more smarter if it would become a problem with performance.
								// There could be a function in world provider or Region to return a slice of a chunk.
					glm::ivec3 prev_cursor = cursor - normal;
					for(prev_cursor[biTan] = 0; prev_cursor[biTan] < size; prev_cursor[biTan]++){
						for(prev_cursor[tan] = 0; prev_cursor[tan] < size; prev_cursor[tan]++){
							prev_layer[prev_cursor[biTan]][prev_cursor[tan]] = valueAt(prev_cursor);
						}
					}
//				}else{
//					for(int i = 0; i < size; i++){
//						for(int j = 0; j < size; j++){
//							prev_layer[i][j] = 0x0f00;
//						}
//					}
//				}
				
				// Creating mask of current layer
				for(cursor[biTan] = 0; cursor[biTan] < size; cursor[biTan]++){
					for(cursor[tan] = 0; cursor[tan] < size; cursor[tan]++){
	//					
						region_dtype valueHereWithLight = valueAt(cursor);
						region_dtype valueHere = block_type(valueHereWithLight);
						
//						char valueHere = valueAt(cursor) * randd;
						
						if(valueHere != 0 && !block_type(valueAt(cursor - normal))){
//						if(valueHere != 0 && !valueAt(cursor - normal)){
							mask[cursor[biTan]][cursor[tan]] = valueHere;
							mask_with_light[cursor[biTan]][cursor[tan]] = valueHereWithLight;
						}else{
							mask[cursor[biTan]][cursor[tan]] = 0;
							mask_with_light[cursor[biTan]][cursor[tan]] = valueHereWithLight;
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
				for(cursor[biTan] = 0, y = 0; cursor[biTan] < size; cursor[biTan]++, y++) {
					int x;
					for(cursor[tan] = 0, x = 0; cursor[tan] < size; cursor[tan]++, x++) {
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
							
							ao_mask[y][x] = calculate_ao(valueAt(left), valueAt(up), valueAt(corner));
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
							
							ao_mask[y][x + 1] = calculate_ao(valueAt(left), valueAt(up), valueAt(corner));
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
							
							ao_mask[y + 1][x] = calculate_ao(valueAt(left), valueAt(up), valueAt(corner));
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
							
							ao_mask[y + 1][x + 1] = calculate_ao(valueAt(left), valueAt(up), valueAt(corner));
						}
					}
				}
				
				for(int y = 0; y < size; y++){
					for(int x = 0; x < size; ){
						if(mask[y][x] == 0){
							x++;
							continue;
						}
						
						region_dtype blockType = mask[y][x]; // używane do sprawdzania, czy następny blok jest tego samego typu
						short aoVal[4] = {
							ao_mask[y][x],
							ao_mask[y][x + 1],
							ao_mask[y + 1][x],
							ao_mask[y + 1][x + 1]
						};

						int width = 1;
//						for(int tx = x + 1; tx < size && mask[y][tx] == blockType; ){
						for(int tx = x + 1; tx < size && mask[y][tx] == blockType; ){
							break; // The greedy meshing is inactive due to me working on light for past month and i forgot about it.
							if(ao_mask[y][tx] != aoVal[0] || ao_mask[y][tx + 1] != aoVal[1] || ao_mask[y + 1][tx] != aoVal[2] || ao_mask[y + 1][tx + 1] != aoVal[3])
								break;
							tx++, width++;
						}
						
						int height = 1;
						bool valid = true;
						for(int ty = y + 1; ty < size && valid; ty++){
							break; // Yeah
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
						// Light   |fordebug|??????aat|bbbbbbbb
						// b - block type
						// t - does this vertex belong to a top side (should be changed to distinguish all sides)
						// a - ambient occlusion value
						int texid = static_cast<int>(blockType);
						if(top){
							texid |= 0x100;
						}
						
//						if(gridOffset.x == 0 && gridOffset.y == 0 && gridOffset.z == 0 && (prev_layer[y][x] & 0xf000) != 0){
//							printf("torch != 0: %x\n", prev_layer[y][x] & 0xf000);
//						}
						
						texid |= ((prev_layer[y][x] & 0xff00)) << 16; // this is the light.
						
						texid |= (y << 4 | x) << 16; // debugging info
						
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
	
	// return std::make_tuple(ret, int_ret);
	return {ret, int_ret};
}

std::tuple<region_dtype*, int> Chunk::giveData(){
	return std::tuple<region_dtype*, int>(data, data_offset);
}

std::list<propagateParam> Chunk::getEmmitedLightFromSide(Direction side){
	std::list<propagateParam> out;
	
	int norm = 0;
	int slice = 0;
	if(side == Direction::FRONT || side == Direction::BACK){
		norm = 2;
	}
	
	if(side == Direction::BACK || side == Direction::RIGHT){
		slice = 15;
	}
	
	int tan = (norm + 1) % 3;
	int biTan = (norm + 2) % 3;
	
	glm::ivec3 normal(0); // back -1 ,front +1 left -1, right +1
	normal[norm] = 1;
	if(side == Direction::FRONT || side == Direction::LEFT){
		normal[norm] = -1;
	}
	
	glm::ivec3 cursor(0);
	cursor[norm] = slice;
	int y = 0;
	for(cursor[biTan] = 0; cursor[biTan] < size; cursor[biTan]++, y++){
		int x = 0;
		for(cursor[tan] = 0; cursor[tan] < size; cursor[tan]++, x++){
			int index = cursor.x + cursor.y * size + cursor.z * size * size;
			const region_dtype& block = data[data_offset + index];
			region_dtype light = block & 0xff00;
			
			if((block & 0xff) == 0 && ((light & 0xf000) > 0x1000 || (light & 0xf00) > 0x100)){
				light -= 0x100 * ((light & 0xf00) != 0) + 0x1000 * ((light & 0xf000) != 0);
				out.push_back(propagateParam(block_position(cursor + normal, gridOffset), light));
			}
		}
	}
	
	return out;
}

std::array<region_dtype, Chunk::size * Chunk::size> Chunk::getSide(Direction side){
	switch(side){
		case Direction::FRONT:
			return getSide(2, 0);
		case Direction::BACK:
			return getSide(2, 1);
		case Direction::LEFT:
			return getSide(0, 0);
		case Direction::RIGHT:
			return getSide(0, 1);
	}
	
	return getSide(0, 0);
}

std::array<region_dtype, Chunk::size * Chunk::size> Chunk::getSide(int direction, int side){
	std::array<region_dtype, Chunk::size * Chunk::size> output;
	
//	direction &= 3;
	int norm = direction;
	int tan = (norm + 1) % 3;
	int biTan = (norm + 2) % 3;
	
	int cursor[3];
	cursor[norm] = 0;
	
	if(side == 1)
		cursor[norm] = size - 1;
	int y = 0;
	for(cursor[biTan] = 0; cursor[biTan] < size; cursor[biTan]++, y++){
		int x = 0;
		for(cursor[tan] = 0; cursor[tan] < size; cursor[tan]++, x++){
			int index = cursor[0] + cursor[1] * size + cursor[2] * size_squared;
			output[y * size + x] = data[data_offset + index];
		}
	}
	
	return output;
}

Chunk::~Chunk(){
	if(doNotDraw){
//		printf("Not drawable destroyed\n");
	}
}