#ifndef _WORLD_PROVIDER_H_
#define _WORLD_PROVIDER_H_

#include <map>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <memory>
#include <tuple>
#include <string>
//#include "../vec3Comp.h"
#include "../utilities/basic_util.h"
//#include "include/basic_util.h"
#include "Region.h"


class worldProvider{
	bool firstLoop = true;
//	char region[regionSize][regionHeight][regionSize];

//	std::map<glm::ivec3, std::shared_ptr<Region>, compareVec3> regions;
	std::map<glm::ivec3, Region, compareVec3> regions;
	Region nullRegion;
	
	inline static const std::string world_path = "../world2";
	
public:
	static const int chunkSize = 16;
	static constexpr int regionN = Region::reg_size;
	static constexpr int regionSize = regionN * chunkSize; // Region size in single blocks, not in chunks.
	static const int regionHeight = regionN * chunkSize;
	
	int regions_size(){
		return regions.size();
	}

	worldProvider(){
//		printf("Sizeof region: %li\n", sizeof(Region));
//		printf("end of constructor\nRegions size: %d\n", regions.size());
	}
	
	int radius = 2;
	
	int xmoved = 0, zmoved = 0;
	int move_required = Region::reg_size / 2;
	
	glm::ivec3 last_pos = glm::ivec3(0);
	
//	std::list<propagateParam> pending_light_updates;
	
	// Return chunk poses to remove
	// Pos position in chunk coordinates
	void update(glm::ivec3 pos){
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
			std::tie(player_region, std::ignore) = toChunkCoords(pos, 4); // 8 = CHUNKS IN REGION, LITERAL VALUE MAY BE SOURCE OF BUGS HERE!!!
			
//			printf("Regions beg\n");
		
			for(auto i = regions.begin(); i != regions.end(); i++){
				i->second.removeable = true;
			}
			
			std::vector<glm::ivec3> new_regions;
			
			for(int z = -radius; z < radius; z++){
				for(int x = -radius; x < radius; x++){
					
					glm::ivec3 position = glm::ivec3(x, 0, z);
					position.x += player_region.x;
					position.z += player_region.z;
					if(auto reg_it = regions.find(position); reg_it != regions.end()){
						reg_it->second.removeable = false;
					}else{
						regions[position] = Region(position);
						
						new_regions.push_back(position);
						
//						printf("New region %2d %2d %2d\n", position.x, position.y, position.z);
					}
				}
			}
			
			
			for(auto i = regions.begin(), nexti = i; i != regions.end(); i = nexti){
				nexti++;
				if(i->second.removeable == true){
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
	
//	void propagateLights(std::list<PropagateParam>){
//		
//	}
	
//	Region& getRegion(int x, int y, int z){
//		// 	CHANGE NAME
//		glm::ivec3 chpos, dpos;
//		
//		std::tie(chpos, dpos) = toChunkCoords(glm::ivec3(x, y, z), regionSize);
//		
//		if(auto reg_it = regions.find(chpos); reg_it != regions.end()){
//			return reg_it->second;
//		}
//		
//		return nullRegion;
//	}
	
//	Region& getRegion(glm::ivec3 pos){
//		return getRegion(pos.x, pos.y, pos.z);
//	}
		
//	bool isThereAChunk(glm::ivec3 pos){
//		Region& regp = getRegion(pos.x, pos.y, pos.z);
//		
//		return regp.type != RegionType::NULL_REGION && regp.getChunkOffset(glm::ivec3(pos.x, pos.y, pos.z)) != -1; // TEMP
//	}
	
	// position - global chunk coordiates
	// returns data pointer, offset of chunk in data
	std::pair<region_dtype*, int> getChunkData(glm::ivec3 position){
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
	
	region_dtype valueAt(int x, int y, int z){
		glm::ivec3 chpos, dpos;
		std::tie(chpos, dpos) = toChunkCoords(glm::ivec3(x, y, z), regionSize);
		
		if(auto reg_it = regions.find(chpos); reg_it != regions.end()){
			return reg_it->second.valueAt(dpos.x, dpos.y, dpos.z);
		}
		
		return 0;
	}
};

#endif