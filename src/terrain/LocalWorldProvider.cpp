#include "LocalWorldProvider.hpp"
	
int LocalWorldProvider::regions_size() {
	return regions.size();
}

	// WorldProvider() {
	// }
// Return chunk poses to remove
// Pos position in chunk coordinates
void LocalWorldProvider::update(glm::ivec3 pos) {
//	std::vector<glm::ivec3> update(glm::ivec3 pos){
	
//		std::vector<glm::ivec3> chunks_to_remove;
	
	glm::ivec3 amount_moved = pos - last_pos;
	xmoved += amount_moved.x;
	zmoved += amount_moved.z;
	
	last_pos = pos;
	
//		printf("Provider move: %2d %2d\n", xmoved, zmoved);
	// printf("yeah\n");
	if(abs(xmoved) >= move_required || abs(zmoved) >= move_required || firstLoop) {
		printf("regions size: %lu\n", regions.size());
		if(abs(xmoved) >= move_required){
			if(xmoved < 0)
				xmoved += move_required;
			else
				xmoved -= move_required;
		}
		
		if(abs(zmoved) >= move_required){
			if(zmoved < 0)
				zmoved += move_required;
			else
				zmoved -= move_required;
		}
		
		glm::ivec3 player_region;
		std::tie(player_region, std::ignore) = toChunkCoords(pos, TerrainConfig::RegionSize); // 8 = CHUNKS IN REGION, LITERAL VALUE MAY BE SOURCE OF BUGS HERE!!!
		
//			printf("Regions beg\n");
	
		for (auto i = regions.begin(); i != regions.end(); i++) {
			i->second.removeable = true;
		}
		
		std::vector<glm::ivec3> new_regions;
		
		for (int z = -radius; z < radius; z++) {
			for (int x = -radius; x < radius; x++) {
				
				glm::ivec3 position = glm::ivec3(x, 0, z);
				position.x += player_region.x;
				position.z += player_region.z;
				if (auto reg_it = regions.find(position); reg_it != regions.end()) {
					reg_it->second.removeable = false;
				} else {
					printf("update call\n");
					// regions[position] = Region(position);
					
					new_regions.push_back(position);
					
//						printf("New region %2d %2d %2d\n", position.x, position.y, position.z);
				}
			}
		}
		
		
		for (auto i = regions.begin(), nexti = i; i != regions.end(); i = nexti) {
			nexti++;
			if (i->second.removeable == true) {
				glm::ivec3 pos = i->second.position;
//					printf("Removing region %d %d %d\n", pos.x, pos.y, pos.z); 
//					for(const glm::ivec3 &p : i->second.getLoadedChunks())
//						chunks_to_remove.push_back(p);
				
				regions.erase(i);
			}
		}
		firstLoop = false;
	}
	
//		return chunks_to_remove;
}
	
// position - global chunk coordiates
// returns data pointer, offset of chunk in data
std::tuple<region_dtype*, int, bool> LocalWorldProvider::getChunkData(glm::ivec3 position) {
	// Loading regions could be done here
	// 	CHANGE NAME
	position *= chunkSize;
	auto [reg_pos, chunk_pos] = toChunkCoords(position, regionSize);
	chunk_pos /= TerrainConfig::ChunkSize; // chunk pos is actually in blocks, but it's a multiple of chunkSize 
										   // since position was multiplied by chunkSize.
	
	if(auto reg_it = regions.find(reg_pos); reg_it != regions.end()) {
		Region& region = reg_it->second;
		auto [offset, generated] = region.getChunkOffset(position);
		if (offset == -1) {
			return {nullptr, 0, false};
		}
		region.setLoadedChunk(chunk_pos);
		return {region.getData(), offset, generated};
	} else {
		// Generating/loading from disk region
		// if (regions.size() >= maxRegions) {
			// unloadRegions();
		// }

		// Loading must be here but for chunks in regions that are less than unload distance range.
		// (Don't load regions that will be removed in next loop iteration.)
		// Well if there probably is player near if he want's to load some chunk.
		// ( his render distance might be higher than what server allows, but that is pain to implement right now)
		// bool inPlayersVicinity = !glm::any(glm::bvec3(glm::greaterThanEqual(glm::abs(r.position - regPos), unloadDistance)));
		auto [iterator, inserted] = regions.emplace(reg_pos, reg_pos); // was regions[reg_pos] = std::move(Region(reg_pos));
		// printf("New region: %2d %2d %2d \n", reg_pos.x, reg_pos.y, reg_pos.z);
		Region& region = iterator->second;
		// printf("regions len(++): %d\n", regions.size());
		auto [offset, generated] = region.getChunkOffset(position);
		if (offset == -1) {
			// printf("???\n");
			return {nullptr, 0, false};
		}
		region.setLoadedChunk(chunk_pos);

		// Might unload just generated region.
		// unloadRegions();
		// return {region.getData(), offset, generated};
		return {region.getData(), offset, generated};
	}
	
	// ????
	return {nullptr, 0, false};
}


void LocalWorldProvider::printRegions() {
	int min[3];
	int max[3];
	// min max initialization
	for (int i = 0 ; i < 3; i++) {
		min[i] = max[i] = regions.begin()->first[i];
	}

	// searching for position bounds
	for (auto it = ++regions.begin(); it != regions.end(); it++) {
		const glm::ivec3& pos = it->first;
		for (int i = 0 ; i < 3; i++) {
			if (pos[i] < min[i]) {
				min[i] = pos[i];
			} else if(pos[i] > max[i]) {
				max[i] = pos[i];
			}
		}	
	}

	int w = abs(max[0] - min[0]) + 1;
	int h = abs(max[1] - min[1]) + 1;
	int d = abs(max[2] - min[2]) + 1;

	glm::ivec3 cursor(min[0], min[1], min[2]);
	printf("Regions bound: (%2d %2d %2d)-(%2d %2d %2d)\n", min[0], min[1], min[2], max[0], max[1], max[2]);
	for (int x = 0; x < w; x++)
		printf("-");
	printf("\n");
	for (int z = 0; z < d; z++) {
		printf("|");
		for (int x = 0; x < w; x++) {
			if (regions.find(glm::ivec3(min[0] + x, 0, min[2] + z)) != regions.end()){
				printf("X");
			} else {
				printf(" ");
			}
		}
		printf("|\n");
	}
	for (int x = 0; x < w; x++)
		printf("-");
	printf("\n");

	printf("\n");
}

void LocalWorldProvider::unloadChunk(const glm::ivec3& position) {
	auto [reg_pos, chunk_pos] = toChunkCoords(position * chunkSize, regionSize);
	chunk_pos /= TerrainConfig::ChunkSize;
	if (auto it = regions.find(reg_pos); it != regions.end()) {
		int loadedChunks = it->second.unloadChunk(chunk_pos);
		if (loadedChunks == 0) {
			// printf("regions len(--): %d\n", regions.size());
			// printRegions();
			regions.erase(reg_pos);
		}
	}
}
	
region_dtype LocalWorldProvider::valueAt(int x, int y, int z) {
	glm::ivec3 chpos, dpos;
	std::tie(chpos, dpos) = toChunkCoords(glm::ivec3(x, y, z), regionSize);
	
	if(auto reg_it = regions.find(chpos); reg_it != regions.end()){
		return reg_it->second.valueAt(dpos.x, dpos.y, dpos.z);
	}
	
	return 0;
}

void LocalWorldProvider::notifyChange(glm::ivec3 chunkPos) {
	auto [reg_pos, chunk_pos] = toChunkCoords(chunkPos, regionSize);
	printf("Change: %2d %2d %2d\n", chunkPos.x, chunkPos.y, chunkPos.z);
	if (auto it = regions.find(reg_pos); it != regions.end()) {
		printf("found\n");
		it->second.modified = true;
	}
}

void LocalWorldProvider::changeBlock(ChangedBlock& block) {

}

LocalWorldProvider::~LocalWorldProvider() {
	printf("yoo\n");
}

// Make some algorithm that removes unused regions,
// or can specify how many regions you want to unload.
// This was something that I came up in a hurry.
// Removes regions that are at a certain distance from player.
void LocalWorldProvider::unloadRegions(std::set<glm::ivec3, compareVec3> playerPositions) {
// 	for (auto [position, region] : regions) {
// 		region.removeable = true;
// 	}

// 	for (auto [position, region] : regions) {
// 		for (const glm::ivec3& playerPosition : playerPositions) {
// 			auto [playerRegPos, inRegPos] = toChunkCoords(playerPosition, TerrainConfig::ChunkSize * TerrainConfig::RegionSize);
// 			bool inPlayersVicinity = !glm::any(glm::bvec3(glm::greaterThanEqual(glm::abs(region.position - playerRegPos), glm::ivec3(unloadDistance))));
// 			if (inPlayersVicinity) {
// 				region.removeable = false;
// 			}
// 		}
// 	}

// 	// Removing regions
// 	for (auto i = regions.begin(), nexti = i; i != regions.end(); i = nexti) {
// 		nexti++;
// 		if (i->second.removeable == true) {
// 			glm::ivec3 pos = i->second.position;
// 					printf("Removing region %d %d %d\n", pos.x, pos.y, pos.z); 
// //					for(const glm::ivec3 &p : i->second.getLoadedChunks())
// //						chunks_to_remove.push_back(p);
			
// 			regions.erase(i);
// 		}
// 	}
	// for (Region& r : regions) {
	// 	r.removeable = true;
	// }
}