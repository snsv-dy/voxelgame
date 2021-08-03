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

Region::Region(glm::ivec3 pos) {
	this->position = pos;
	this->type = RegionType::NORMAL_REGION;
	
	fileName = std::to_string(position.x) + "." + std::to_string(position.y) + "." + std::to_string(position.z) + ".reg";
	// fileName = "-1.0.-1.reg";
	std::ifstream dataFile(directory + "/" + fileName);
	bool fileExists = dataFile.good();
	dataFile.close();
	
	// generate();
		if(!fileExists){
			// generate region
			// generate();
			// modified = true;
			// save();
//			fileName = "-1.0.-1.reg";
//			load();
			// printf("not loading\n");
		} else {
			// printf("loading\n");
			// load region
			load();
			// printf("[%2d %2d %2d] loaded chunks: \n", position.x, position.y, position.z);
			// for (auto v : loaded_chunks) {
			// 	printf("%d %d %d\n", v.x, v.y, v.z);
			// }
		}
	
}

struct FrequencyParams {
	glm::vec3 position;
	double frequency, frequency_inc;
};

void Region::genChunk(int x, int y, int z) {
	// #define LIGHT_DEBUG
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
				region_dtype light_value = 0x0000;
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
				#ifdef LIGHT_DEBUG
				#define GROUND_LEVEL 11
				#else
				#define GROUND_LEVEL 40
				#endif
				region_dtype ival = 0;
				int ypos = worldOffset.y + y;
//				if(ypos <= GROUND_LEVEL && !((worldOffset.x + x) == 0 && (worldOffset.z + z) == 0)){
				if(ypos <= GROUND_LEVEL){
					// light debug

					#ifdef LIGHT_DEBUG
					ival = 1;
					if((ypos <= 10 && ypos > 6) || (ypos <= 5 && ypos > 0) || (ypos == 11 && x == 8 && z == 8 && zero))
						ival = 0;
					#else
					// light debug end
					float cave = perlin.noise3D_0_1(fpos.x, fpos.y, fpos.z);
	//				if((ypos) < (GROUND_LEVEL + mountain) && !(cave > THRESHOLD && (ypos < GROUND_LEVEL + 10))){
					if(ypos == 0 || cave < THRESHOLD){
						ival = 1;
						if(ypos < GROUND_LEVEL / 2){
							ival = 3;
						}
					}
					#endif
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


std::vector<region_dtype> Region::compressData(size_t length, region_dtype mask) {
	std::vector<region_dtype> compressed;

	const region_dtype token = 0xffff;
	region_dtype runCharacter = data[0] & mask;
	region_dtype runLength = 1;
	int runSum = 0;
	int i;
	int nothi = 0;
	int ones = 0;
	int mores = 0;
	for (i = 1; i < length; i++) {
		region_dtype block = data[i] & mask;
		if (block == runCharacter && runLength < 0xfffe) {
			runLength++;
			ones++;
		} else {
			runSum += (int)runLength;
			if (runLength > 1) {
				compressed.push_back(token);
				compressed.push_back(runLength);
			}else{
				mores++;
			}
			compressed.push_back(runCharacter);
			runCharacter = block;
			runLength = 1;
		}
	}

	if (runLength > 1) {
		compressed.push_back(token);
		compressed.push_back(runLength);
	}
	compressed.push_back(runCharacter);
	runSum += runLength;
	// printf("[%2d %2d %2d] compression i: %d, mask: %x, run: %d, nothing: %d, eh: %d\n", position.x, position.y, position.z, i, mask, runSum, nothi, mores + ones);

	return compressed;
}

void Region::expandData(std::vector<region_dtype> compressed, bool overwriteData = true) {
	const region_dtype token = 0xffff;
	int dataIndex = 0;
	int i;
	for (i = 0; i < compressed.size();) {
		region_dtype& block = compressed[i];
		if (block == token) {
			region_dtype length = compressed[i + 1];
			region_dtype character = compressed[i + 2];
			for (int j = 0; j < length; j++) {
				data[dataIndex] = (data[dataIndex] * !overwriteData) | character;
				dataIndex++;
			}
			i += 3;
		} else {
			data[dataIndex] = (data[dataIndex] * !overwriteData) | block;
			dataIndex++;
			i++;
		}
	}
	printf("expanded end: %d, i: %d, comSize: %d\n", dataIndex, i, compressed.size());
}

void Region::load() {
	std::ifstream iw;
	iw.open(directory + "/" + fileName, std::ios::in);
	if (iw.good()) {
		
		// Read number of chunks
		size_t chunksNumber = 0;
		iw.read((char *)&chunksNumber, sizeof(size_t));

		// Read size of data
		size_t blocksSize = 0;
		size_t lightSize = 0;
		iw.read((char *)&blocksSize, sizeof(size_t));
		iw.read((char *)&lightSize, sizeof(size_t));

		// Read generated chunks
		// printf("[%2d %2d %2d] loaded chunks: \n", position.x, position.y, position.z);
		for (int i = 0; i < chunksNumber; i++) {
			glm::ivec3 v;
			iw.read((char *)&v, sizeof(glm::ivec3));
			loaded_chunks.insert(v);
			// printf("%d %d %d\n", v.x, v.y, v.z);
		}

		std::vector<region_dtype> compressedBlocks(blocksSize);
		std::vector<region_dtype> compressedLight(lightSize);
		iw.read((char *)compressedBlocks.data(), blocksSize * sizeof(region_dtype));
		iw.read((char *)compressedLight.data(), lightSize * sizeof(region_dtype));
		expandData(compressedBlocks, true);
		expandData(compressedLight, false);

		iw.close();
		printf("F loaded\n");
	}else{
		printf("F not good loaded\n");
	}
}

void Region::save() {
	std::ofstream of;
	of.open(directory + "/" + fileName, std::ios::out);
	if	(of.good()) {

		// Write number of chunks.
		size_t chunksNumber = loaded_chunks.size();
		of.write((char *)&(chunksNumber), sizeof(size_t));

		const size_t chunkCubed = TerrainConfig::ChunkSize * TerrainConfig::ChunkSize * TerrainConfig::ChunkSize;
		const size_t regionCubed = TerrainConfig::RegionSize * TerrainConfig::RegionSize * TerrainConfig::RegionSize;

		// Compress data
		std::vector<region_dtype> compressedBlocks = compressData(data_size, 0xff);
		std::vector<region_dtype> compressedLight = compressData(data_size, 0xff00);
		
		// Write size of data.
		size_t blocksSize = compressedBlocks.size();
		size_t lightSize = compressedLight.size();
		of.write((char *)&(blocksSize), sizeof(size_t));
		of.write((char *)&(lightSize), sizeof(size_t));

		// Write generated chunks.
		for (const glm::ivec3& vec : loaded_chunks) {
			of.write((const char *)&vec, sizeof(glm::ivec3));
		}

		// Write region data.
		of.write((const char *)compressedBlocks.data(), blocksSize * sizeof(region_dtype));
		of.write((const char *)compressedLight.data(), lightSize * sizeof(region_dtype));

		of.close();
		// f.write((char *)data, data_size * sizeof(region_dtype));
		printf("F saved\n");
	} else {
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

std::pair<int, bool> Region::getChunkOffset(glm::ivec3 pos) {
	
	// odejmij od współrzędnych position * chsize * regsize
//	bool print = pos.z < 0;
	
	glm::ivec3 local_pos = pos - (this->position * static_cast<int>(region_size));
	
	const int chunk_cubed = chunk_size * chunk_size * chunk_size;
	
	int cx = (local_pos.x / chunk_size);
	int cy = (local_pos.y / chunk_size);
	int cz = (local_pos.z / chunk_size);
	
	int data_x = cx * (chunk_cubed);
	int data_y = cy * (chunk_cubed * reg_size);
	int data_z = cz * (chunk_cubed * (reg_size * reg_size));
	
	int index = data_x + data_y + data_z;
	if(index < 0 || index >= data_size){
//		if(print)
//			printf("nochunk %02d %02d %02d, pos: %d\n", x, y, z, pos);
		return {-1, false};
	}
	
	bool generated = false;

	if(loaded_chunks.find(pos) == loaded_chunks.end()){
		// generate
		// printf("[%2d %2d %2d] generating: %2d %2d %2d\n", position.x, position.y, position.z, pos.x, pos.y, pos.z);
		// printf("generating: %2d %2d %2d\n", cx, cy, cz);
		genChunk(cx, cy, cz);		
		loaded_chunks.insert(pos);
		modified = true;
		generated = true;
	}
		// modified = true;
	
	return {index, generated};
}

region_dtype* Region::getData(){
	return this->data;
}

Region::~Region(){
	// if(type == RegionType::NORMAL_REGION && modified) {
	if (type == RegionType::NORMAL_REGION && modified) {
		save();
	}
}