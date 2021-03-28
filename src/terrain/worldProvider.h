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
	
	std::list<propagateParam> pending_light_updates;
	
	// Return chunk poses to remove
	// Pos position in chunk coordinates
	std::vector<glm::ivec3> update(glm::ivec3 pos){
		
		std::vector<glm::ivec3> chunks_to_remove;
		
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
					for(const glm::ivec3 &p : i->second.getLoadedChunks())
						chunks_to_remove.push_back(p);
					
					regions.erase(i);
				}
			}
			
//			if(firstLoop){
//				printf("Making sun\n");
//				for(glm::ivec3& reg_pos : new_regions){
//					if(auto it = regions.find(reg_pos); it != regions.end() && it->second.brand_new){
////					if(auto it = regions.find(glm::ivec3(0)); it != regions.end() && it->second.brand_new){
//						Region& region = it->second;
////						std::list<propagateParam> remaining_lights = 
//						region.calculateSunlight();
////						printf("Remaining len: %d\n", remaining_lights.size());
////						pending_light_updates.insert(pending_light_updates.end(), remaining_lights.begin(), remaining_lights.end());
//					}
//				}
//				
//			// Maybye it was a better idea to lookup if there is some light coming from the sides every time a new region is created,
//			// because keeping this huge list for regions that may be generated some time in the future, could take too much memory.
//			// And these chunks may never be generated.
//			
//			std::list<propagateParam>::iterator pend_iter = pending_light_updates.begin();
////			for(propagateParam& p : pending_light_updates){
//			while(pend_iter != pending_light_updates.end()){
//				propagateParam& p = *pend_iter;
//				
//				glm::ivec3 reg_pos, in_reg_pos;
//				std::tie(reg_pos, in_reg_pos) = toChunkCoords(p.position, this->regionN);
//				
//				if(auto it = regions.find(reg_pos); it != regions.end()){
//					Region& region = it->second;
//					
//					propagateParam param;
//					param.light_value = p.light_value;
//					param.position = in_reg_pos;
//					
//					region.pending_lights.push_back(param);
//					
//					pend_iter = pending_light_updates.erase(pend_iter);
////					remove p somehow;
//				}else{
//					pend_iter++;
//				}
//			}
//			
//			for(auto& it : regions){
//				Region& region = it.second;
//				region.propagatePendingLight();
//			}
////				printf("Sun made\n");
////			}
//
//			if(firstLoop){
//				printf("Pending lights len: %d\n", pending_light_updates.size());
//			}
			
			firstLoop = false;
//			printf("Regions end\n");
		}
		
		if(firstLoop){
//			printf("Regions len: %d\n", regions.size());
		}
		
		
		return chunks_to_remove;
	}
	
//	void propagateLights(std::list<PropagateParam>){
//		
//	}
	
	Region& getRegion(int x, int y, int z){
		// 	CHANGE NAME
		glm::ivec3 chpos, dpos;
		
		std::tie(chpos, dpos) = toChunkCoords(glm::ivec3(x, y, z), regionSize);
		
		if(auto reg_it = regions.find(chpos); reg_it != regions.end()){
			return reg_it->second;
		}
		
		return nullRegion;
	}
	
	Region& getRegion(glm::ivec3 pos){
		return getRegion(pos.x, pos.y, pos.z);
	}
		
	bool isThereAChunk(glm::ivec3 pos){
		Region& regp = getRegion(pos.x, pos.y, pos.z);
		
		return regp.type != RegionType::NULL_REGION && regp.getChunkOffset(glm::ivec3(pos.x, pos.y, pos.z)) != -1; // TEMP
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