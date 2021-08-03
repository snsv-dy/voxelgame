#include <functional>
#include <cstdio>
#include <glm/glm.hpp>
#include <fstream>
#include <string>
#include <set>
#include "../../src/terrain/PerlinNoise.hpp"

using namespace std;
struct cmpVec {
	bool operator()(const glm::vec3& a, const glm::vec3& b) const {
		return a.z > b.z || a.y > b.y || a.x > b.x;
	}
};

const unsigned int chunkSize = 16;
const unsigned int chunkSquared = chunkSize * chunkSize;
const unsigned int chunkCubed = chunkSquared * chunkSize;
const unsigned int regionSize = 2;
const unsigned int regionCubed = regionSize * regionSize * regionSize;

typedef unsigned short region_dtype;

siv::PerlinNoise perlin(1234);

class Region {
public:
	set<glm::vec3, cmpVec> chunks;
	region_dtype data[chunkCubed * regionCubed];
	Region() {
		// genSom();
	}

	void print() {
		printf("Chunks: ");
		for (glm::vec3 c : chunks) {
			printf("vec: %2.2f, %2.2f, %2.2f\n", c.x, c.y, c.z);
		}

		printf("\n");
		for(int y = 0; y < chunkSize; y++) {
			for(int x = 0; x < chunkSize; x++) {
				printf("%x ", data[y * chunkSize + x]);
			}
			printf("\n");
		}
	}

	void genChunk(int x, int y, int z) {
		int dataoffset = x * chunkCubed + y * (chunkCubed * regionSize) + z * (chunkCubed * regionSize * regionSize);
		glm::vec3 worldOffset = glm::ivec3(x * chunkSize, y * chunkSize, z * chunkSize);
		bool zero = (x % 2 == 0) && (z % 2 == 0);

		for (int z = 0; z < chunkSize; z++) {
			int zoff = z * chunkSquared;
			
			for (int y = 0; y < chunkSize; y++) {
				int yoff = y * chunkSize;
	//			
				for (int x = 0; x < chunkSize; x++) {
					#define THRESHOLD 0.5f
					#define GROUND_LEVEL 11
					region_dtype ival = 0;
					int ypos = worldOffset.y + y;
					// if(ypos <= GROUND_LEVEL){
					// 	ival = 1;
					// 	if((ypos <= 10 && ypos > 6) || (ypos <= 5 && ypos > 0) || (ypos == 11 && x == 8 && z == 8 && zero))
					// 		ival = 0;
					// }
					glm::vec3 fpos =  glm::vec3(x, y, z) / glm::vec3(10.0f);
					float cave = perlin.noise3D_0_1(fpos.x, fpos.y, fpos.z);

					if(ypos == 0 || cave < THRESHOLD){
						ival = 1;
						if(ypos < GROUND_LEVEL / 2){
							ival = 3;
						}
					}

					data[dataoffset + zoff + yoff + x] = ival | 0x100;
				}
			}
		}
		chunks.insert(glm::vec3(x, y, z));
	}

	void genSom() {
		// for (int z = 0; z < regionSize; z++) {
		// 	for (int y = 0; y < regionSize; y++) {
		// 		for (int x = 0; x < regionSize; x++) {
		// 			genChunk(x, y, z);
		// 		}
		// 	}
		// }
		genChunk(0, 0, 0);
		// for (int z = 0; z < chunkSize; z++) {
		// 	for (int y = 0; y < chunkSize; y++) {
		// 		for (int x = 0; x < chunkSize; x++) {
		// 			data[z * chunkSquared + y * chunkSize + x] = ;
		// 		}
		// 	}
		// }
	}

	vector<region_dtype> compressData(size_t length, region_dtype mask) {
		vector<region_dtype> compressed;

		const region_dtype token = 0xffff;
		region_dtype runCharacter = data[0] & mask;
		region_dtype runLength = 1;

		int runSum = 0;
		int i;
		for (i = 1; i < length; i++) {
			region_dtype block = data[i] & mask;
			if (block == runCharacter && runLength < 0xfffe) {
				runLength++;
			} else {
				runSum += runLength;
				if (runLength > 1) {
					compressed.push_back(token);
					compressed.push_back(runLength);
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

		printf("i %d, sum: %d\n", i, runSum);

		return compressed;
	}

	void expandData(vector<region_dtype> compressed, bool overwriteData = true) {
		const region_dtype token = 0xffff;
		region_dtype runCharacter = compressed[0];
		region_dtype runLength = 1;
		int dataIndex = 0;
		int expandedEnd = 0;
		int i;
		printf("lengths: ");
		for (i = 0; i < compressed.size(); ) {
			region_dtype& block = compressed[i];
			if (block == token) {
				region_dtype len = compressed[i + 1];
				region_dtype character = compressed[i + 2];
				printf("(%d [%d])", len, i);
				for (int j = 0; j < len; j++) {
					data[dataIndex] = (data[dataIndex] * !overwriteData) | character;
					dataIndex++;
					expandedEnd++;
				}
				i += 3;
				if (len == 0xb) {
					printf("0xb i: %d %d, size: %lu\n", i, dataIndex, compressed.size());
				}
			} else {
				data[dataIndex] = (data[dataIndex] * !overwriteData) | block;
				dataIndex++;
				expandedEnd++;
				printf("1 (%d)", block);
				i++;
			}
		}
		printf("\n");

		printf("expand i: %d, end: %d, index: %d\n", i, expandedEnd, dataIndex);
	}

	void save(string fname) {
		if (chunks.empty()) {
			genSom();
		}

		ofstream of(fname, ios::binary);

		// Write number of chunks.
		size_t chunksNumber = chunks.size();
		of.write((char *)&(chunksNumber), sizeof(size_t));

		// Compress data
		vector<region_dtype> compressedBlocks = compressData(chunkCubed, 0xff);
		vector<region_dtype> compressedLight = compressData(chunkCubed, 0xff00);
		
		// Write size of data.
		size_t blocksSize = compressedBlocks.size();
		size_t lightSize = compressedLight.size();
		of.write((char *)&(blocksSize), sizeof(size_t));
		of.write((char *)&(blocksSize), sizeof(size_t));

		// Write generated chunks.
		// for (auto vec : chunks) {
		// 	of.write((const char *)&vec, sizeof(glm::vec3));
		// }

		// Write region data.
		of.write((const char *)compressedBlocks.data(), blocksSize * sizeof(region_dtype));
		of.write((const char *)compressedLight.data(), lightSize * sizeof(region_dtype));

		of.close();
	}

	void load(string fname) {
		ifstream iw(fname, ios::binary | ios::in);
		
		// Read number of chunks
		size_t chunksNumber;
		iw.read((char *)&chunksNumber, sizeof(size_t));

		// Read size of data
		size_t blocksSize;
		size_t lightSize;
		iw.read((char *)&blocksSize, sizeof(size_t));
		iw.read((char *)&lightSize, sizeof(size_t));

		// Read generated chunks
		// for (int i = 0; i < chunksNumber; i++) {
		// 	glm::vec3 v;
		// 	iw.read((char *)&v, sizeof(glm::vec3));
		// 	chunks.insert(v);
		// }

		vector<region_dtype> compressedBlocks(blocksSize);
		vector<region_dtype> compressedLight(lightSize);
		iw.read((char *)compressedBlocks.data(), blocksSize * sizeof(region_dtype));
		iw.read((char *)compressedLight.data(), lightSize * sizeof(region_dtype));
		expandData(compressedBlocks, true);
		// expandData(compressedLight, false);

		iw.close();
	}
};

int main() {
	glm::vec3 vec(1.0f);
	vec.z = -12.0f;

	string fname {"hellofaseason2.file"};
	Region r;
	r.genSom();
	// r.save(fname);
	r.load(fname);
	// r.print();

	// ifstream iw(fname, ios::binary | ios::in);
	// printf("good? %d\n", iw.good());

	// glm::vec3 inc_vec;
	// char buffer[12];

	// iw.read((char *)&inc_vec, sizeof(inc_vec));
	// iw.close();

	// printf("inc_vec: %2.2f, %2.2f, %2.2f\n", inc_vec.x, inc_vec.y, inc_vec.z);
	// printf("vec: %2.2f, %2.2f, %2.2f\n", vec.x, vec.y, vec.z);


	return 0;
}