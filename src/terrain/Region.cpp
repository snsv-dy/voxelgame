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
				#define GROUND_LEVEL 30
				char ival = 0;
				int ypos = worldOffset.y + y;
				if(ypos <= GROUND_LEVEL){
					float cave = perlin.noise3D_0_1(fpos.x, fpos.y, fpos.z);
	//				if((ypos) < (GROUND_LEVEL + mountain) && !(cave > THRESHOLD && (ypos < GROUND_LEVEL + 10))){
					if(cave < THRESHOLD){
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
	
	brand_new = true;
}

bool Region::is_near_light(int x, int z, int mask[chunk_size][chunk_size]){
	const int csm1 = chunk_size - 1;
	
	if(x < csm1 && mask[z][x+1] == 0){
		return true;
	}
	
	if(z < csm1 && mask[z+1][x] == 0){
		return true;
	}
	
	if(x > 0 && mask[z][x-1] == 0){
		return true;
	}
	
	if(z > 0 && mask[z-1][x] == 0){
		return true;
	}
	
	return false;
}

std::vector<glm::ivec3> Region::calculateSunInChunk(int gx, int gy, int gz, int mask[chunk_size][chunk_size]){
	int chunk_cubed = chunk_size * chunk_size * chunk_size;
	int chunk_squared = chunk_size * chunk_size;
	
	int dataoffset = gx * chunk_cubed + (reg_size - gy - 1) * (chunk_cubed * reg_size) + gz * (chunk_cubed * reg_size * reg_size);
	if(gy == 0){
		for(int z = 0; z < chunk_size; z++){
			for(int x = 0; x < chunk_size; x++){
				mask[z][x] = 0;
			}
		}
	}
	
	std::vector<glm::ivec3> spread_queue;
		
	for(int y = chunk_size - 1; y >= 0; y--){
//	for(int y = 0; y < chunk_size; y++){
		for(int z = 0; z < chunk_size; z++){
			for(int x = 0; x < chunk_size; x++){
				int index = z * chunk_size * chunk_size + y * chunk_size + x;
				region_dtype& block = data[dataoffset + index];
				block &= 0xff | mask[z][x];
				if((block & 0xff) != 0){
					mask[z][x] = 0xf00;
				}
//				data[dataoffset + index] &= (0xff | (y << 8));
//				continue;
//				if( gy == 0 && y == 0){ // this is prolly the top of the world
//				if(y == 15 && gy == 0){ // this is prolly the top of the world
//					data[dataoffset + index] &= 0x00ff;
//					if((data[dataoffset + index] & 0xff) == 0){
//						int index_below = z * chunk_size * chunk_size + (y - 1) * chunk_size + x;
//						data[dataoffset + index_below] &= 0xff; // Memory segmentation might be here (block below).
//					}
//						
//				}//block_type(data[dataoffset + index]) == 0 && 
//				else if(data[dataoffset + index] == 0 && !(y == 0 && gy == reg_size - 1)){
//					if(y == 0){
//						int tdataoffset = gx * chunk_cubed + (reg_size - gy - 2) * (chunk_cubed * reg_size) + gz * (chunk_cubed * reg_size * reg_size);
//						data[tdataoffset + z * chunk_size * chunk_size + (chunk_size - 1) * chunk_size + x] &= 0x00ff; // Memory segmentation might be here (block below).
//					}else{
//						data[dataoffset + index - chunk_size ] &= 0x00ff; // Memory segmentation might be here (block below).
//					}
////					data[dataoffset + index - chunk_size ] &= 0x00ff; // Memory segmentation might be here (block below).
////					printf("miau\n");
//				}else if(data[dataoffset + index] == 0){}
			}
		}
		
		for(int z = 0; z < chunk_size; z++){
			for(int x = 0; x < chunk_size; x++){
				int index = z * chunk_size * chunk_size + y * chunk_size + x;
				region_dtype& block = data[dataoffset + index];
				if(mask[z][x] == 0xf00 && (block & 0xff) == 0 &&
					is_near_light(x, z, mask)
				){
//					printf("miał\n");
					block &= 0xff;// | mask[z][x];
					block |= 0x100;
					spread_queue.push_back(glm::ivec3(x + gx * chunk_size, y + gy * chunk_size, z + gz * chunk_size));
				}
			}
		}
	}
	
	return spread_queue;
}

void Region::propagateLight(std::vector<glm::ivec3>& queue){
	std::vector<glm::ivec3> new_queue;
	while(queue.size() > 0){
		const glm::ivec3& pos = queue.back();
		queue.pop_back();
		
		
		// push other poses to 
	}
}

void Region::calculateSunlight(){
//	int chunk_cubed = chunk_size * chunk_size * chunk_size;
//	int chunk_squared = chunk_size * chunk_size;
	
//	int dataoffset = x * chunk_cubed + y * (chunk_cubed * reg_size) + z * (chunk_cubed * reg_size * reg_size);
	
	int mask[chunk_size][chunk_size] = {0};
	
	std::vector<glm::ivec3> spread_queue;
	
	for(int z = 0; z < reg_size; z++){
		for(int x = 0; x < reg_size; x++){
			for(int y = 0; y < reg_size; y++){
				std::vector<glm::ivec3> tqueue = calculateSunInChunk(x, y, z, mask);
				spread_queue.insert(spread_queue.end(), tqueue.begin(), tqueue.end());
			}
		}
	}
	
	brand_new = false;
}

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

const region_dtype Region::valueAt(int x, int y, int z){
	int chunk_cubed = chunk_size * chunk_size * chunk_size;
//		
	int cx = (x / chunk_size) * (chunk_cubed) + x % chunk_size;
	int cy = (y / chunk_size) * (chunk_cubed * reg_size) + ((y % (chunk_size))) * chunk_size;
	int cz = (z / chunk_size) * (chunk_cubed * (reg_size * reg_size)) + ((z % chunk_size)) * (chunk_size * chunk_size);
	
	unsigned int pos = cx + cy + cz;
	
//		unsigned int pos = z * (region_size * region_size) + y * region_size + x;
	if(pos > data_size || pos < 0){
		return 0;
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