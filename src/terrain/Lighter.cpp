#include "Lighter.hpp"

void inline disperseLight(block_position pos, std::list<propagateParam>& lights, region_dtype light_value){
	for(int i = 0; i < 6; i++){
		lights.push_back(propagateParam(pos + neighbouring_offsets[i], light_value));
	}
}

Lighter::Lighter(WorldLoader& worldLoader) : worldLoader{worldLoader} {
}

//struct DirAndSide2{
//	glm::ivec3 position;
//	Direction face;
//};
//
//// Direction to get position of an adjecent chunk.
DirAndSide directions[4] {
	{glm::ivec3(-1, 0, 0), Direction::RIGHT},
	{glm::ivec3(1, 0, 0), Direction::LEFT},
	{glm::ivec3(0, 0, 1), Direction::FRONT},
	{glm::ivec3(0, 0, -1), Direction::BACK}
};
void Lighter::updateSunlightInColumn(int x, int z) {
	glm::ivec3 top = glm::ivec3(x, 0, z);
	while(true) {
		auto [chunk, found] = worldLoader.getChunk(top);
		if (!found) {
			top.y--;
			break;
		}
		top.y++;
	}
					
	int mask[Chunk::size][Chunk::size];
	for (int y = 0; y < Chunk::size; y++){
		for (int x = 0; x < Chunk::size; x++){
			mask[y][x] = 0xf00;
		}
	}
	
	
	while(true){
		bool allDark = false;
		if(auto [chunk, found] = worldLoader.getChunk(top); found) {		
			
			if(!allDark){
				std::list<propagateParam> tpropagate = chunk.sunlightPass(mask, allDark);
				lights.insert(lights.end(), tpropagate.begin(), tpropagate.end());
			}
				
			// Get light coming from adjecent chunks
			for(const DirAndSide& dir : directions){
				if(auto [adjacentChunk, found] = worldLoader.getChunk(top + dir.position); found){
					std::list<propagateParam> side_emit = adjacentChunk.getEmmitedLightFromSide(dir.face);
					lights.insert(lights.end(), side_emit.begin(), side_emit.end());
				}
			}
		}else{
			break;
		}
		top.y--;
	}
}

void Lighter::updateLightColumns(std::set<glm::ivec3, compareVec3> columns) {
	std::list<propagateParam> lights_to_propagate;
	
	for (const glm::ivec3& pos : columns) {
		updateSunlightInColumn(pos.x, pos.z);
	}
}

void Lighter::updateSunlightForBlock(block_position pos, bool placed) {
	
	region_dtype sunlight_value = 0;
	auto [above,found] = worldLoader.getBlock(pos + glm::ivec3(0, 1, 0));
	
	const region_dtype above_light = getSunLight(above);
	
	if (placed) {
		bool first = true;
		while (true) {
			auto [block, found] = worldLoader.getBlock(pos);
			if (!found || (getBlockType(block) != 0 && !first))
				break;
			
			disperseLight(pos, darks, getSunLight(block) - 0x100); // tutaj ?
			
			block &= 0xf0ff; // darkens sunlight.
			
			pos += glm::ivec3(0, -1, 0);
			updatedChunks.insert(pos.chunk);
			first = false;
		}
		
	} else {
		if (found) { 
			if(getBlockType(above) == 0 && above_light == 0xf00){
				// go all the way down until you come across a block.
				// set lights to 0, and then propagate.
		
				auto [current, found] = worldLoader.getBlock(pos);
				
				while(true){
					auto [current, found] = worldLoader.getBlock(pos);
					if(!found || getBlockType(current) != 0)
						break;
					
					current |= 0x0f00;
					
					updatedChunks.insert(pos.chunk);
					
					// propagate light outwards
					for(int i = 0; i < 4; i++){
						lights.push_back(propagateParam(pos + neighbouring_offsets[i], 0xe00));
					}
								
					pos += glm::ivec3(0, -1, 0);
				}
			}else{
				region_dtype min_light = 0;
				for(int i = 0; i < 6; i++){
					auto [block, found] = worldLoader.getBlock(pos + neighbouring_offsets[i]);
					
					if(found && getSunLight(block) > min_light){
						min_light = getSunLight(block);
					}
				}
				
				if(min_light > 0)
					lights.push_back(propagateParam(pos, min_light - 0x100) );
			}
		}
	}
}

void Lighter::updateBlocklightForBlock(block_position pos, region_dtype& block_before, bool placed) {
	
	if (auto [block, found] = worldLoader.getBlock(pos); found && ((block_before & 0xff) != 4) && (block & 0xff) != 4) {
//	if (auto [block, found] = worldLoader.getBlock(pos); found && ((block & 0xff) != 4)) {
		region_dtype light = getBlockLight(block);
		
		if (placed) {
			disperseLight(pos, blockDarks, getBlockLight(block) - 0x1000 * (light > 0));
			block &= 0x0fff; // Dim block light.
		} else if ((block_before & 0xff) != 4) {
			region_dtype min_light = 0;
			for (int i = 0; i < 6; i++) {
				if (auto [block, found] = worldLoader.getBlock(pos + neighbouring_offsets[i]); found && getBlockLight(block) > min_light) {
					min_light = getBlockLight(block);
				}
			}
			
			region_dtype next_light_value = min_light - 0x1000 * (min_light > 0);
			blockLights.push_back(propagateParam(pos, next_light_value));
		}
	}
}

void Lighter::updateLightSource(region_dtype& block, block_position& pos, region_dtype intensity, bool placed) {
	if (placed) {
		block &= 0x0fff;
		block |= intensity;
		
		disperseLight(pos, blockLights, intensity - 0x1000);
	} else {
		blockDarks.push_back(propagateParam(pos, getBlockLight(block)));
	}
}

void Lighter::updateLightForBlock(std::list<ChangedBlock> blocks) {
	for( auto [position, placed, type, typeBefore] : blocks) {
//		printf("block: %d, placed: %d\n", type, placed);
		updateBlocklightForBlock(position, typeBefore, placed);
		if(type == 4 || (typeBefore & 0xff) == 4){
			auto [block, found] = worldLoader.getBlock(position);
			updateLightSource(block, position, 0xb000, placed);
		}
		updateSunlightForBlock(position, placed);
	}
}

std::set<glm::ivec3, compareVec3> Lighter::getUpdatedChunks() {
	auto ret = updatedChunks;
	updatedChunks.clear();
	return ret;	
}

void Lighter::propagateLights() {
	if (!darks.empty()) {
		propagateDark(darks);
		darks.clear();
	}
	
	if (!lights.empty()) {
		propagateLight(lights);
		lights.clear();
	}
	
	if (!blockDarks.empty()) {
		propagateDark(blockDarks, LightType::Block);
		blockDarks.clear();
	}
	
	if (!blockLights.empty()) {
		propagateLight(blockLights, LightType::Block);
		blockLights.clear();
	}
}

// Don't be an idiot, don't propagate dim lights ( where light_value == 0)!
void Lighter::propagateLight(std::list<propagateParam>& lights, const LightType& type) {
//	#ifdef __DEBUG_PRINTF_
	// printf("[%s] Propagate list size: %d, ", type == LightType::Block ? "BLOCK" : "SUN  ", lights.size());
//	#endif
	int insertions = 0;
	int deletions = 0;
	
	while(!lights.empty()){
		propagateParam param = lights.front();
		lights.pop_front();
		deletions++;
		
		if(auto [block, found] = worldLoader.getBlock(param.position); found) {
			
			region_dtype light_mask = 0xf00;
			region_dtype light_increment = 0x100;
			
			if(type == LightType::Block){
				light_mask = 0xf000;
				light_increment = 0x1000;
			}
			
			region_dtype light_min = 0;
			region_dtype light = block & light_mask;
			param.light_value &= light_mask; // Necessary when propagating params returned by Chunk's get emitted light from side.
											 //	(returns params with both lights).
			
//				8, 1, -10
//				8, 1, 5? in chunk
			if (param.position.block.x == 8 && param.position.block.y == 1 && param.position.block.z == 5 && type == LightType::Block) {
				printf("prop light at 8, 1, 5? in chunk: %x, light in block: %x\n", param.light_value, light);
			}
			
			if(getBlockType(block) == 0 && light < param.light_value) {
				block &= ~light_mask; 		// reset light value.
				block |= param.light_value; // set new light value.
				
				updatedChunks.insert(param.position.chunk);
				
				region_dtype next_light_value = param.light_value - light_increment;// * (param.light_value > light_increment);
				
				if((param.light_value & light_mask) > light_increment) {
					disperseLight(param.position, lights, next_light_value);
					insertions += 6;
				}
			}
		}
		
	}
	
	// printf("insertions: %d, deletions: %d\n", insertions, deletions);
}

void Lighter::propagateDark(std::list<propagateParam> darks, const LightType& type) {
	// printf("[%s] Propagate dark size: %d, ", type == LightType::Block ? "BLOCK" : "SUN  ", darks.size());
	
	int insertions = 0, deletions = 0;
	std::list<propagateParam>* lightQueue = &lights;
	if (type == LightType::Block) {
		lightQueue = &blockLights;
	}
	
	
	while(!darks.empty()){
		propagateParam param = darks.front();
		darks.pop_front();
		deletions++;
		
		if(auto [block, found] = worldLoader.getBlock(param.position); found){
			
			region_dtype light_increment = 0x100;
			region_dtype light_mask = 0xf00;
			
			if (type == LightType::Block) {
				light_increment = 0x1000;
				light_mask = 0xf000;
			}
			
			region_dtype light_min = 0;
			region_dtype light = block & light_mask;
			region_dtype next_light_value = light - light_increment * (light > 0); // Condition is to ensure that
																				   // decrementing light value will not cause an overflow.
			
			if (getBlockType(block) == 0) {
				updatedChunks.insert(param.position.chunk);
				
				if (light == param.light_value) {
					block &= ~light_mask; // light to zero
					
//					propagate dark to neighbours
					if( next_light_value > light_min ){
						disperseLight(param.position, darks, next_light_value);
						insertions += 6;
					}else{
						// Check if there is some light nearby that should be propagated.
						// Bug: wall 2x3 block wall 1 block away from light with 8 strength.
						for(int i = 0; i < 6; i++){
							auto [adjecent_block, found] = worldLoader.getBlock(param.position + neighbouring_offsets[i]);
							
							region_dtype adjecent_light = (adjecent_block & light_mask);
							if(found && adjecent_light > light_min){
//							if (found) {
								disperseLight(param.position + neighbouring_offsets[i], *lightQueue, adjecent_light - light_increment);
							}
						}
					}
				} else if (light > param.light_value) {
					// propagate LIGHT to neighbours starting from this block
					disperseLight(param.position, *lightQueue, next_light_value);
				}
				
			} else if (getBlockType(block) == 4 && type == LightType::Block) {
				// Nie widziałem tego nigdy w konsoli.
				// printf(" yeah! Next light value: %x, cur light value: %x\n", next_light_value, light);
				updatedChunks.insert(param.position.chunk);
				disperseLight(param.position, *lightQueue, next_light_value);
			}
		}
	}
	
	// printf("insertions: %d, deletions: %d, lights: %d\n", insertions, deletions, lights.size());
	
//	printf("inpropagate dark\n");
	// Propagating light should probably be done outside this function.
//	for(const glm::ivec3& e : propagateLight(lights, type)){
//		affected_chunks.insert(e);
//	}
//	printf("propagate dark end\n");
}


//Lighter::~Lighter()
//{
//}

