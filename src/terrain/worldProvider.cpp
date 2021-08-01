#include "worldProvider.hpp"
	
int WorldProvider::regions_size() {
	return regions.size();
}

	// WorldProvider() {
	// }
	
// Return chunk poses to remove
// Pos position in chunk coordinates
void WorldProvider::update(glm::ivec3 pos) {
//	std::vector<glm::ivec3> update(glm::ivec3 pos){
	
//		std::vector<glm::ivec3> chunks_to_remove;
	
	glm::ivec3 amount_moved = pos - last_pos;
	xmoved += amount_moved.x;
	zmoved += amount_moved.z;
	
	last_pos = pos;
	
//		printf("Provider move: %2d %2d\n", xmoved, zmoved);
	
	if(abs(xmoved) >= move_required || abs(zmoved) >= move_required || firstLoop){
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
					regions[position] = Region(position);
					
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
std::pair<region_dtype*, int> WorldProvider::getChunkData(glm::ivec3 position){
	// Loading regions could be done here
	// 	CHANGE NAME
	position *= chunkSize;
	auto [reg_pos, chunk_pos] = toChunkCoords(position, regionSize);
	
	if(auto reg_it = regions.find(reg_pos); reg_it != regions.end()){
		Region& region = reg_it->second;
		return {region.getData(), region.getChunkOffset(position)};
	}
	
	return {nullptr, 0};
}
	
region_dtype WorldProvider::valueAt(int x, int y, int z) {
	glm::ivec3 chpos, dpos;
	std::tie(chpos, dpos) = toChunkCoords(glm::ivec3(x, y, z), regionSize);
	
	if(auto reg_it = regions.find(chpos); reg_it != regions.end()){
		return reg_it->second.valueAt(dpos.x, dpos.y, dpos.z);
	}
	
	return 0;
}