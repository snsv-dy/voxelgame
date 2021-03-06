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

class scrollingParams{
	int chunkSize = 16;
	
	int passedSize = chunkSize / 4; // 
	
	float playerPos = 0;	// player's position
	
	bool lpassed = false; 	// has player just passed left threshold
	int lthresh; 			// left threshold, indicates that you should load next region(s) to the left in this direction
	int rtemp_thresh;		// temporary threshold put to the left of where 
	
	bool rpassed = false;
	int rthresh;
	int ltemp_thresh;
	
	int ichunk = 0; // index of region which player is currently standing on
	
	#define LOAD_RADIUS 2
	int activeChunks[LOAD_RADIUS * 2];
	
	void init(){
		if(playerPos >= 0){
			ichunk = playerPos / chunkSize;
		}else{
			int mx = (-playerPos - 1);
			ichunk = -(mx / chunkSize + 1);
		}
		
		lthresh = (ichunk - 0.5f) * chunkSize;
		ltemp_thresh = -1;
		
		rthresh = (ichunk + 0.5f) * chunkSize;
		rtemp_thresh = -1;
		
		for(int i = 0; i < LOAD_RADIUS * 2; i++){
			activeChunks[i] = -LOAD_RADIUS + i;
		}
	}
	
public:
	static const int load_radius = LOAD_RADIUS; // how many regions will be loaded in each direction. (should be associated to chunk's radius in world loader)
	
	scrollingParams(float playerPos, int chunkSize = 16){
		this->chunkSize = chunkSize;
		this->playerPos = playerPos;
		this->init();
	}
	
	int update(float playerPos){
		this->playerPos = playerPos;
		
		if(rpassed && playerPos > ltemp_thresh){
			lthresh += passedSize;
			rpassed = false;
		}else if(lpassed && playerPos < rtemp_thresh){
			rthresh -= passedSize;
			lpassed = false;
		}
		
		int ret = 0; // 0 - nothing changed, 1 - player moved across the right threshold, -1 - player moved across the left threshold
		
		if(playerPos > rthresh){
			if(lpassed){
				rthresh -= passedSize;
			}
			
//			activeChunks[0]++;
//			activeChunks[1]++;
			for(int i = 0; i < load_radius * 2; i++){
				activeChunks[i]++;
			}
			
			lthresh = rthresh;
			rthresh += chunkSize;
			if(!lpassed){
				rpassed = true;
				ltemp_thresh = lthresh + passedSize;
				lthresh -= passedSize;
			}else{
				lpassed = false;
			}
			
			ret = 1;
		}else if(playerPos < lthresh){
			if(rpassed){
				lthresh += passedSize;
			}
			
//			activeChunks[0]--;
//			activeChunks[1]--;
			for(int i = 0; i < load_radius * 2; i++){
				activeChunks[i]--;
			}
			
			rthresh = lthresh;
			lthresh -= chunkSize;
			
			if(!rpassed){
				lpassed = true;
				rtemp_thresh = rthresh - passedSize;
				rthresh += passedSize;
			}else{
				rpassed = false;
			}
			ret = -1;
		}
		
		if(playerPos >= 0){
			ichunk = playerPos / chunkSize;
		}else{
			int mx = (-playerPos - 1);
			ichunk = -(mx / chunkSize + 1);
		}
		
		return ret;
	}
	
	int* getActive(){
		return activeChunks;
	}
};

class worldProvider{
	bool firstLoop = true;
//	char region[regionSize][regionHeight][regionSize];

//	std::map<glm::ivec3, std::shared_ptr<Region>, compareVec3> regions;
	std::map<glm::ivec3, Region, compareVec3> regions;
	Region nullRegion;
	
	glm::ivec3 center = glm::ivec3(0);
	glm::ivec3 last_pos = glm::ivec3(0.0f);
	
	// scrolling terrain control params
	scrollingParams xdir = scrollingParams(0.0f, regionSize);
	scrollingParams zdir = scrollingParams(0.0f, regionSize);
	
	inline static const std::string world_path = "../world2";
	
public:
	static const int chunkSize = 16;
	static const int regionN = 5;//Region::reg_size;
	static constexpr int regionSize = regionN * chunkSize; // Region size in single blocks, not in chunks.
	static const int regionHeight = regionN * chunkSize;
	
	int regions_size(){
		return regions.size();
	}

	worldProvider(){
//		printf("Sizeof region: %li\n", sizeof(Region));
//		printf("end of constructor\nRegions size: %d\n", regions.size());
	}
	
	// Return chunk poses to remove
	std::vector<glm::ivec3> update(glm::vec3 pos){
		
		std::vector<glm::ivec3> chunks_to_remove;
		
		int resx = xdir.update(pos.x);
		int resz = zdir.update(pos.z);
		
		if(resx != 0 || resz != 0 || firstLoop){
			int *xactive = xdir.getActive();
			int *zactive = zdir.getActive();
		
			for(auto i = regions.begin(); i != regions.end(); i++){
				i->second.removeable = true;
			}
			
			for(int i = 0; i <scrollingParams::load_radius * 2 ; i++){
				for(int j = 0; j < scrollingParams::load_radius * 2; j++){
					glm::ivec3 t = glm::ivec3(xactive[i], 0, zactive[j]);
					if(auto region = regions.find(t); region != regions.end()){
						regions[t].removeable = false;
//						region->second.removeable = false;
//						regions.erase(r1);
//						printf("Erased %2d %2d\n", r1.x, r1.z);
					}else{
						regions[t] = Region(t);
//						regions.insert(std::pair<glm::ivec3, Region>(t, Region(t, 3)));
					}
				}
			}
			
			
			for(auto i = regions.begin(), nexti = i; i != regions.end(); i = nexti){
				nexti++;
				if(i->second.removeable == true){
					glm::ivec3 pos = i->second.position;
					printf("Removing region %d %d %d\n", pos.x, pos.y, pos.z); 
					for(const glm::ivec3 &p : i->second.getLoadedChunks())
						chunks_to_remove.push_back(p);
					
					regions.erase(i);
				}
			}
			
			firstLoop = false;
		}
		
		if(firstLoop){
			printf("Regions len: %d\n", regions.size());
		}
		
		return chunks_to_remove;
	}
	
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
	
	char valueAt(int x, int y, int z){
		glm::ivec3 chpos, dpos;
		std::tie(chpos, dpos) = toChunkCoords(glm::ivec3(x, y, z), regionSize);
		
		if(auto reg_it = regions.find(chpos); reg_it != regions.end()){
			return reg_it->second.valueAt(dpos.x, dpos.y, dpos.z);
		}
		
		return 0;
	}
};

#endif