#include "Region.h"

//Region::directory = "../world2";
	
char terrainballs2(int x, int y, int z){
//	if(y == 0 && z == 0)
//		printf("x: %d\n", x);
	
//	int ir = 100;
//	int margin = -29;
	int ir = 30;
	int margin = 0;
	
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
	int ysph = y;
	z -= ir + margin;
//	return 1;
//	if((x*x + y*y + z*z) <= (ir*ir) || y <= 0){
	if((x*x + ysph*ysph + z*z) <= (ir*ir) || y <= 0){
//		float ts = sinf(x / 10.f) * cosf(z / 10.0f) * sinf(y / 10.0f);
//		char type = (ts > 0) + rand() % 2 + 1;
		
//		return type;
		return 2;
	}

	return 0;
}

char Region::terrain(int x, int y, int z){
//	char terrain(int x, int y, int z){
////		float xp = (sin(x / 5.0f))* 1 + (cos(x/100.0f) + 0) * 2 ;
////		float yp = (sin(z / 5.0f)) * 1 + (cos(z/100.0f) + 0) * 2 ;
////		
////		float xy = xp*yp;
//		float xy = 		(sin(x / 70.0f)) * (cos(z / 70.0f)) * 20 + 20;
//		xy +=  (sin(x / 5.0f)) * 2 * (cos(z / 5.0f)) + 2;
//		
//		//float yval = sin(y / 100.0f) * 50.0f;
//		
//		if(y < xy){
//			if(y < 7)
//				return 2;
//			return 1;
//		}
//			
//		return 0;
//	if(y < 5 && x & 2 && z & 2){
	if(y < 5){
		return 2;
	}
	
	return 0;
}

//Region::Region(){
////	printf("auto made (%d %d %d)\n", position.x, position.y, position.z);
//	printf("DEFAULT REGION CONSTRUCTOR\n");
//	this->type = RegionType::NULL_REGION;
////	this->default_ = true;
////	this->data = nullptr;
//	this->position = glm::ivec3(0);
////	canBeDestroyed = false;
//}

Region::Region(glm::ivec3 pos){
	this->position = pos;
	this->type = RegionType::NORMAL_REGION;
	
	fileName = std::to_string(position.x) + "." + std::to_string(position.y) + "." + std::to_string(position.z) + ".reg";
//	fileName = "-1.0.-1.reg";
	std::ifstream file_exists(directory + "/" + fileName);
	
	generate();
//		if(!file_exists.good()){
//			// generate region
//			generate();
////			save();
////			fileName = "-1.0.-1.reg";
////			load();
//		}else{
//			// load region
//			load();
//		}
	
}

struct FrequencyParams{
	glm::vec3 position;
	double frequency, frequency_inc;
};

// xyz - position of a chunk inside of region used only in generate
void Region::genChunk(int x, int y, int z){
	int chunk_cubed = chunk_size * chunk_size * chunk_size;
	int chunk_squared = chunk_size * chunk_size;
//		glm::ivec3 offset = glm::ivec3(x, y, z);
	int dataoffset = x * chunk_cubed + y * (chunk_cubed * reg_size) + z * (chunk_cubed * reg_size * reg_size);
	
	glm::ivec3 worldOffset = glm::ivec3(x * chunk_size, y * chunk_size, z * chunk_size);
	worldOffset += position * reg_size * chunk_size;
	
	// Old method for generating available in previous commits.
	
	glm::vec3 world_pos_f = glm::vec3(worldOffset);
	glm::vec3 local_pos = glm::vec3(0.0f);
	
	bool zero = (x % 2 == 0) && (z % 2 == 0);
	
	siv::PerlinNoise perlin(1234);
	for(int z = 0; z < chunk_size; z++, local_pos.z++){
		int zoff = z * chunk_squared;
		local_pos.y = 0.0f;
		
		for(int y = 0; y < chunk_size; y++, local_pos.y++){
			int yoff = y * chunk_size;
			local_pos.x = 0.0f;
//			
			for(int x = 0; x < chunk_size; x++, local_pos.x++){
//				data[dataoffset + zoff + yoff + x] = terrainAt(x + worldOffset.x, y + worldOffset.y, z + worldOffset.z);
				double dvalue = 1.0;
				bool first = true;
				
//				char value = abs(x % 8) <= 8;
				int Y = worldOffset.y + y;
				int X = worldOffset.x + x;
				if(X < 0) X = -X;
				int Z = worldOffset.z + z;
				if(Z < 0) Z = -Z;
//				char value = ((Y) <= abs( ( (X+8) % 16) - 8)) + 16 && ((Y) <= abs((Z+8) % 16 - 8)) + 16 && ((Y) >= -abs( ( (X+8) % 16) - 8)) + 16 && ((Y) >= -abs((Z+8) % 16 - 8)) + 16;
				
//				region_dtype light_value = (rand() & 0xf) << 8;//(rand() & 0xf) << 8;
				region_dtype light_value = 0xf00;
//				if(X < 16 && Z < 16 && (X + Z < 15))
//					light_value = ((X + Z) & 0xf) << 8;//(rand() & 0xf) << 8;
				
//				data[dataoffset + zoff + yoff + x] = terrainballs2(X, Y, Z) | light_value;
//				data[dataoffset + zoff + yoff + x] = light_value;// | light_value;
//				continue;
				
				
				glm::vec3 fpos = (world_pos_f + local_pos) / glm::vec3(10.0f);
				
//				#define MOUNTAIN_SCALE 40.0f
//				#define MOUNTAIN_DISTANCE 2.0f
//				float mountain = perlin.noise2D_0_1(fpos.x / MOUNTAIN_DISTANCE, fpos.z / MOUNTAIN_DISTANCE) * MOUNTAIN_SCALE;
				#define THRESHOLD 0.5f
				#define GROUND_LEVEL 40
//				#define GROUND_LEVEL 11
				char ival = 0;
				int ypos = worldOffset.y + y;
//				if(ypos <= GROUND_LEVEL && !((worldOffset.x + x) == 0 && (worldOffset.z + z) == 0)){
				if(ypos <= GROUND_LEVEL){
					// light debug
//					ival = 1;
//					if((ypos <= 10 && ypos > 6) || (ypos <= 5 && ypos > 0) || (ypos == 11 && x == 8 && z == 8 && zero))
//						ival = 0;
					// light debug end
					float cave = perlin.noise3D_0_1(fpos.x, fpos.y, fpos.z);
	//				if((ypos) < (GROUND_LEVEL + mountain) && !(cave > THRESHOLD && (ypos < GROUND_LEVEL + 10))){
					if(ypos == 0 || cave < THRESHOLD){
						ival = 1;
						if(ypos < GROUND_LEVEL / 2){
							ival = 3;
						}
					}
				}
//				char ival = 1;
//				if ( (cave > THRESHOLD || (worldOffset.y > GROUND_LEVEL && mountain <= THRESHOLD) )){
//					ival = 0;
//				}
				
				data[dataoffset + zoff + yoff + x] = ival | light_value;
			}
		}
	}
}

void Region::generate(){
	for(int z = 0; z < reg_size; z++){
		for(int y = 0; y < reg_size; y++){
			for(int x = 0; x < reg_size; x++){
				genChunk(x, y, z);
			}
		}
	}
	
//	for(int s = 0; s < 6; s++){
//		for(int z = 0; z < chunk_size
//	}
	
	brand_new = true;
}

//bool Region::is_near_light(int x, int z, int mask[chunk_size][chunk_size]){
//	const int csm1 = chunk_size - 1;
//	
//	if(x < csm1 && mask[z][x+1] == 0){
//		return true;
//	}
//	
//	if(z < csm1 && mask[z+1][x] == 0){
//		return true;
//	}
//	
//	if(x > 0 && mask[z][x-1] == 0){
//		return true;
//	}
//	
//	if(z > 0 && mask[z-1][x] == 0){
//		return true;
//	}
//	
//	return false;
//}

//std::list<propagateParam> Region::calculateSunInChunk(int gx, int gy, int gz, int mask[chunk_size][chunk_size]){
//	int chunk_cubed = chunk_size * chunk_size * chunk_size;
//	int chunk_squared = chunk_size * chunk_size;
//	
//	int dataoffset = gx * chunk_cubed + (reg_size - gy - 1) * (chunk_cubed * reg_size) + gz * (chunk_cubed * reg_size * reg_size);
//	if(gy == 0){
//		for(int z = 0; z < chunk_size; z++){
//			for(int x = 0; x < chunk_size; x++){
//				mask[z][x] = 0;
//			}
//		}
//	}
//	
//	std::list<propagateParam> spread_queue;
//		
//	for(int y = chunk_size - 1; y >= 0; y--){
////	for(int y = 0; y < chunk_size; y++){
//		for(int z = 0; z < chunk_size; z++){
//			for(int x = 0; x < chunk_size; x++){
//				int index = z * chunk_size * chunk_size + y * chunk_size + x;
//				region_dtype& block = data[dataoffset + index];
//				if((block & 0xff) != 0){
//					mask[z][x] = 0xf00;
//				}
//				block &= 0xff | mask[z][x];
////				data[dataoffset + index] &= (0xff | (y << 8));
////				continue;
////				if( gy == 0 && y == 0){ // this is prolly the top of the world
////				if(y == 15 && gy == 0){ // this is prolly the top of the world
////					data[dataoffset + index] &= 0x00ff;
////					if((data[dataoffset + index] & 0xff) == 0){
////						int index_below = z * chunk_size * chunk_size + (y - 1) * chunk_size + x;
////						data[dataoffset + index_below] &= 0xff; // Memory segmentation might be here (block below).
////					}
////						
////				}//block_type(data[dataoffset + index]) == 0 && 
////				else if(data[dataoffset + index] == 0 && !(y == 0 && gy == reg_size - 1)){
////					if(y == 0){
////						int tdataoffset = gx * chunk_cubed + (reg_size - gy - 2) * (chunk_cubed * reg_size) + gz * (chunk_cubed * reg_size * reg_size);
////						data[tdataoffset + z * chunk_size * chunk_size + (chunk_size - 1) * chunk_size + x] &= 0x00ff; // Memory segmentation might be here (block below).
////					}else{
////						data[dataoffset + index - chunk_size ] &= 0x00ff; // Memory segmentation might be here (block below).
////					}
//////					data[dataoffset + index - chunk_size ] &= 0x00ff; // Memory segmentation might be here (block below).
//////					printf("miau\n");
////				}else if(data[dataoffset + index] == 0){}
//			}
//		}
//		
//		// jeżeli y > 0 to można ustawić te wartości dla warstwy poprzedniej w pętli wyżej.
//		for(int z = 0; z < chunk_size; z++){
//			for(int x = 0; x < chunk_size; x++){
//				int index = z * chunk_size * chunk_size + y * chunk_size + x;
//				region_dtype& block = data[dataoffset + index];
//				if(mask[z][x] == 0xf00 && (block & 0xff) == 0 &&
//					is_near_light(x, z, mask) // TU TRZEBA DODAĆ DO OUTSIDE QUEUE JESZCZE, BO ZA MAŁO ŚWIATEŁ PRZECHODZI POMIĘDZY REGIONAMI.
//				){
////					printf("miał\n");
////					block &= 0xff;// | mask[z][x];
////					block |= 0xf00;
////					spread_queue.push_back(
////						(struct propagateParam){
////							.position = glm::ivec3(x + gx * chunk_size, y + (reg_size - gy - 1) * chunk_size, z + gz * chunk_size),
////							.light_value = 0x100
////						}
////					);
//				}
//			}
//		}
//	}
//	
//	return spread_queue;
//}

//inline region_dtype& mutable_valueAt(const glm::ivec3& pos, region_dtype data[], int data_size){
//	const int chunk_cubed = chunk_size * chunk_size * chunk_size;
//	//		
//		int cx = (pos.x / chunk_size) * (chunk_cubed) + pos.x % chunk_size;
//		int cy = (pos.y / chunk_size) * (chunk_cubed * reg_size) + ((pos.y % (chunk_size))) * chunk_size;
//		int cz = (pos.z / chunk_size) * (chunk_cubed * (reg_size * reg_size)) + ((pos.z % chunk_size)) * (chunk_size * chunk_size);
//		
//		unsigned int pos_v = cx + cy + cz;
//		
//	//		unsigned int pos = z * (region_size * region_size) + y * region_size + x;
//		if(pos_v > data_size || pos_v < 0){
////			continue;
//		}
//		
//		// VALUE AT
//		
//		region_dtype block = data[pos_v];//valueAt(pos.x, pos.y, pos.z);
//}

// Order: 0 - left, 1 - front, 2 - right, 3 - back, 4 - top, 5 - bottom.
//void Region::addToOutputList(glm::ivec3 pos, const region_dtype& light){
//	const int size_in_blocks = reg_size * chunk_size;
//	
//	if(pos.x < 0){
//		pos.x = size_in_blocks;
//		
//		light_output[0].push_back(
//			(struct propagateParam){
////				.region_pos = pos - glm::ivec3(),
//				.position = pos,
//				.light_value = light
//			}
//		);
//	}else if(pos.x >= size_in_blocks){
//		pos.x = 0;
//		
//		light_output[2].push_back(
//			(struct propagateParam){
//				.position = pos,
//				.light_value = light
//			}
//		);
//	}
//	
//	if(pos.y < 0){
//		pos.y = size_in_blocks;
//		
//		light_output[5].push_back(
//			(struct propagateParam){
//				.position = pos,
//				.light_value = light
//			}
//		);
//	}else if(pos.y >= size_in_blocks){
//		pos.y = 0;
//		
//		light_output[4].push_back(
//			(struct propagateParam){
//				.position = pos,
//				.light_value = light
//			}
//		);
//	}
//	
//	if(pos.z < 0){
//		pos.z = size_in_blocks;
//		
//		light_output[3].push_back(
//			(struct propagateParam){
//				.position = pos,
//				.light_value = light
//			}
//		);
//	}else if(pos.z >= size_in_blocks){
//		pos.z = 0;
//		
//		light_output[1].push_back(
//			(struct propagateParam){
//				.position = pos,
//				.light_value = light
//			}
//		);
//	}
//	
	
//}

//std::list<propagateParam> Region::propagateLight(std::list<propagateParam>& queue){
//	int size_in_blocks = reg_size * chunk_size;
//	
////	std::vector<propagateParam> old_queue = queue;
//	std::list<propagateParam> outside_updates;
//	
//	while(!queue.empty()){
//			const propagateParam param = queue.front();//back();
//			queue.pop_front();
//			
//			if(param.light_value == 0xf){
//				continue;
//			}
//			
////			const glm::ivec3& pos = param.position;
//	//		old_queue.pop_back();
//			
////			bool not_here = false;
////			glm::ivec3 other_region_pos = position;
////			if( pos.x < 0){
////				not_here = true;
////				other_region_pos.x -= 1;
////			} else if( pos.x >= size_in_blocks){
////				not_here = true;
////				other_region_pos.x += 1;
////			}
////			
//////			if( pos.y < 0){
//////				not_here = true;
//////				other_region_pos.y -= 1;
//////			} else if( pos.y >= size_in_blocks){
//////				not_here = true;
//////				other_region_pos.y += 1;
//////			}
////			
////			if( pos.z < 0){
////				not_here = true;
////				other_region_pos.z -= 1;
////			} else if( pos.z >= size_in_blocks){
////				not_here = true;
////				other_region_pos.z += 1;
////			}
//			
////			if(not_here){
////				
////				
////				continue;
////			}
//			
////			if( (pos.x < 0 || pos.x >= size_in_blocks) ||
////				(pos.y < 0 || pos.y >= size_in_blocks) ||
////				(pos.z < 0 || pos.z >= size_in_blocks)){
////					// Append global position for ease of thinking.
////				addToOutputList(pos, param.light_value);
//////					outside_updates.push_back((struct propagateParam){
//////						.position = glm::ivec3(
//////											pos.x + position.x * reg_size, 
//////											pos.y + position.y * reg_size, 
//////											pos.z + position.z * reg_size
//////										),
//////						.light_value = param.light_value
//////					});
////					continue;
////				}
//				
//			region_dtype& block = valueAt(pos.x, pos.y, pos.z);
//			
//			if( (block & 0xff) != 0 || (block & 0xf00) <= param.light_value){
////				printf("e %d %x %x\n", (block & 0xff) != 0, (block & 0xf00), param.light_value);
//				continue;
//			}
//			block &= 0xff;
//			block |= param.light_value;
//	//		
//			propagateParam next_poses[6];
//			for(int i = 0; i < 6; i++){ 
//				next_poses[i].position = pos;
//				next_poses[i].light_value = param.light_value + 0x100;
//			}
//	//		
//			next_poses[2].position.x -= 1;
//			next_poses[3].position.x += 1;
//			
//			next_poses[0].position.y -= 1;
//			next_poses[1].position.y += 1;
//			
//			next_poses[4].position.z -= 1;
//			next_poses[5].position.z += 1;
//	//		
//			for(int i = 0; i < 6; i++)
//				queue.push_back(next_poses[i]);
//				
////			new_queue.insert(new_queue.end(), next_poses, next_poses + 6);
//	}
//	
//	return outside_updates;
//}

// side 0 - 1 (begin, end)
// norm 0 - 2 (x, y, z)
std::array<region_dtype, Region::chunk_size * Region::chunk_size> Region::getChunkSide(glm::ivec3 pos, int norm, int side){
	int chunk_cubed = chunk_size * chunk_size * chunk_size;
//	int chunk_squared = chunk_size * chunk_size;
	
	int data_offset = pos.x * chunk_cubed + pos.y * (chunk_cubed * reg_size) + pos.z * (chunk_cubed * reg_size * reg_size);
	
	std::array<region_dtype, chunk_size * chunk_size> ret;
	
	int tan = (norm + 1) % 3;
	int biTan = (norm + 2) % 3;
	
	glm::ivec3 normal(0);
	normal[norm] = side;
	
	glm::ivec3 cursor(0);
	cursor[norm] = (chunk_size - 1) * side; // or chunk_size - 1
	
	int x = 0, y = 0;
	for(cursor[biTan] = 0; cursor[biTan] < chunk_size; cursor[biTan]++, y++){
		for(cursor[tan] = 0; cursor[tan] < chunk_size; cursor[tan]++, x++){
			int index = cursor.x + cursor.y * chunk_size + cursor.z * chunk_size * chunk_size;
			ret[y * chunk_size + x] = data[data_offset + index];
		}
	}
	
	return ret;
}

//std::list<propagateParam> Region::calculateSunlight(){
//	int mask[chunk_size][chunk_size] = {0};
//	
//	std::list<propagateParam> spread_queue;
//	
//	for(int z = 0; z < reg_size; z++){
//		for(int x = 0; x < reg_size; x++){
//			// init mask here 
//			for(int y = 0; y < reg_size; y++){
//				std::list<propagateParam> tqueue = calculateSunInChunk(x, y, z, mask);
//				spread_queue.insert(spread_queue.end(), tqueue.begin(), tqueue.end());
//			}
//		}
//	}
//	
////	for(int side = 0; side < 2; side++){
////		for(int norm = 0; norm < 3; norm++){
////			int tan = (norm + 1) % 3;
////			int biTan = (norm + 2) % 3;
////			
////			glm::ivec3 cursor(0);
////			cursor[norm] = (chunk_size - 1) * side; // or chunk_size - 1
////			
////			int x = 0, y = 0;
////			for(cursor[biTan] = 0; cursor[biTan] < reg_size; cursor[biTan]++, y++){
////				for(cursor[tan] = 0; cursor[tan] < reg_size; cursor[tan]++, x++){
//////					int chunk_cubed = chunk_size * chunk_size * chunk_size;
////					int chunk_squared = chunk_size * chunk_size;
////
////					int side_offset = x * chunk_squared + y * (chunk_squared * reg_size);
//////					int index = cursor.x + cursor.y * chunk_size + cursor.z * chunk_size * chunk_size;
//////					ret[y * chunk_size + x] = data[data_offset + index];
////					std::array<region_dtype, chunk_size * chunk_size> chunk_side = getChunkSide(cursor, norm, side);
////					for(int i = 0; i < chunk_size; i++){
////						for(int j = 0; j < chunk_size; j++){
////							light_output[side * 3 + norm][side_offset + i * chunk_size + j] = chunk_side[i * chunk_size + j];
////						}
////					}
////				}
////			}
////		}
////	}
//	
//	printf("propagate queue len: %d\n", spread_queue.size());
//	
//	std::list<propagateParam> outside_lights;// = propagateLight(spread_queue);
//	
//	brand_new = false;
//	
//	return outside_lights;
//}

//void Region::propagatePendingLight(){ // merge with above function ?
//	propagateLight(pending_lights);
//}

void Region::load(){
	std::ifstream f;
	f.open(directory + "/" + fileName, std::ios::in);
	if(f.good()){
		f.read((char *)data, data_size * sizeof(region_dtype));
//		for(int i = 0; i < data_size; i += rand() & 7)
//			if(data[i])
//				data[i] = rand() & 3;
			
		f.close();
		printf("F loaded\n");
	}else{
		printf("F not good loaded\n");
	}
}

void Region::save(){
	std::ofstream f;
	f.open(directory + "/" + fileName, std::ios::out);
	if(f.good()){
		f.write((char *)data, data_size * sizeof(region_dtype));
		printf("F saved\n");
		f.close();
	}else{
		printf("F not good\n");
	}
}

region_dtype& Region::valueAt(int x, int y, int z){
	const int chunk_cubed = chunk_size * chunk_size * chunk_size;
//		
	int cx = (x / chunk_size) * (chunk_cubed) + x % chunk_size;
	int cy = (y / chunk_size) * (chunk_cubed * reg_size) + ((y % (chunk_size))) * chunk_size;
	int cz = (z / chunk_size) * (chunk_cubed * (reg_size * reg_size)) + ((z % chunk_size)) * (chunk_size * chunk_size);
	
	unsigned int pos = cx + cy + cz;
	
//		unsigned int pos = z * (region_size * region_size) + y * region_size + x;
	if(pos > data_size || pos < 0){
		return ref0;
	}
	
	return data[pos];
}

int Region::getChunkOffset(glm::ivec3 pos){
	
	// odejmij od współrzędnych position * chsize * regsize
//	bool print = pos.z < 0;
	
	pos -= position * static_cast<int>(region_size);
	
	int chunk_cubed = chunk_size * chunk_size * chunk_size;
	
	int cx = (pos.x / chunk_size) * (chunk_cubed);
	int cy = (pos.y / chunk_size) * (chunk_cubed * reg_size);
	int cz = (pos.z / chunk_size) * (chunk_cubed * (reg_size * reg_size));
	
	int index = cx + cy + cz;
	if(index < 0 || index >= data_size){
//		if(print)
//			printf("nochunk %02d %02d %02d, pos: %d\n", x, y, z, pos);
		return -1;
	}
	
	return index;
}

region_dtype* Region::getData(){
	return this->data;
}

const std::vector<glm::ivec3>& Region::getLoadedChunks(){
	return this->loaded_chunks;
}

Region::~Region(){
	if(type == RegionType::NORMAL_REGION && modified){
		save();
	}
}