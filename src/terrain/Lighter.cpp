#include "Lighter.hpp"

void inline disperseLight(block_position pos, std::list<propagateParam>& lights, region_dtype light_value){
	for(int i = 0; i < 6; i++){
		lights.push_back(propagateParam(pos + neighbouring_offsets[i], light_value));
	}
}

Lighter::Lighter(WorldLoader& worldLoader) : worldLoader{worldLoader} {
}

struct DirAndSide2{
	glm::ivec3 position;
	Direction face;
};
//
//// Direction to get position of an adjecent chunk.
DirAndSide2 directions2[4] {
	{glm::ivec3(-1, 0, 0), Direction::RIGHT},
	{glm::ivec3(1, 0, 0), Direction::LEFT},
	{glm::ivec3(0, 0, 1), Direction::FRONT},
	{glm::ivec3(0, 0, -1), Direction::BACK}
};
std::list<propagateParam> Lighter::updateSunlightInColumn(int x, int z){
//	return {};
//	glm::ivec3 top = last_camera_pos;
//	top.z = z;
//	top.x = x;
//	
//	top.y += unloadRadius;
//	
//	int bottom = last_camera_pos.y - unloadRadius;
//	
//	while(top.y > bottom && chunks.find(top) == chunks.end()){
//		top.y--;
//	}

	glm::ivec3 top = glm::ivec3(x, 0, z);
	while(true) {
		auto [chunk, found] = worldLoader.getChunk(top);
		if (!found) {
			top.y--;
			break;
		}
		top.y++;
	}
	
			
	std::list<propagateParam> lights_to_propagate;
					
//	if(top.y > bottom){
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
//					if(allDark)
//						printf("");
					lights_to_propagate.insert(lights_to_propagate.end(), tpropagate.begin(), tpropagate.end());
				}
					
				// Get light coming from adjecent chunks
				for(const DirAndSide2& dir : directions2){
					if(auto [adjacentChunk, found] = worldLoader.getChunk(top + dir.position); found){
						std::list<propagateParam> side_emit = adjacentChunk.getEmmitedLightFromSide(dir.face);
						lights_to_propagate.insert(lights_to_propagate.end(), side_emit.begin(), side_emit.end());
					}
				}
			}else{
				break;
			}
			top.y--;
		}
//	}else{
//		printf("[Light] Invalid column? (%d %d)\n", x, z);
//	}
	
	return lights_to_propagate;
}

std::list<propagateParam> Lighter::updateLightColumns(std::set<glm::ivec3, compareVec3> columns) {
	std::list<propagateParam> lights_to_propagate;
	
	for (const glm::ivec3& pos : columns) {
		std::list<propagateParam> tpropagate = updateSunlightInColumn(pos.x, pos.z);
		lights_to_propagate.insert(lights_to_propagate.end(), tpropagate.begin(), tpropagate.end());
	}
	
	propagateLight(lights_to_propagate);
	
	return lights_to_propagate;
}

void Lighter::updateSunlightForBlock(block_position pos, bool placed) {
	
	region_dtype sunlight_value = 0;
	auto [above,found] = worldLoader.getBlock(pos + glm::ivec3(0, 1, 0));
	
	const region_dtype above_light = getSunLight(above);
	
	if(placed){
//		std::list<propagateParam> darks;
		
		bool first = true;
		while(true){
			auto [block, found] = worldLoader.getBlock(pos);
			if(!found || (getBlockType(block) != 0 && !first))
				break;
			
			disperseLight(pos, darks, getSunLight(block) - 0x100); // tutaj ?
			
//			block |= 0xf00;
			block &= 0xf0ff; // darkens sunlight.
			
			pos += glm::ivec3(0, -1, 0);
			first = false;
			updatedChunks.insert(pos.chunk);
		}
		
//		for(const glm::ivec3& pos : propagateDark(darks)){
//			chunks_to_update.insert(pos);
//		}
		
	}else{
//		std::list<propagateParam> lights;
		
		if(found ){ 
			if(getBlockType(above) == 0 && above_light == 0xf00){
				// go all the way down until you come across a block.
				// set lights to 0, and then propagate.
		
				auto [current, found] = worldLoader.getBlock(pos);
				
				while(true){
					auto [current, found] = worldLoader.getBlock(pos);
					if(!found || getBlockType(current) != 0)
						break;
					
//					current &= 0xf0ff; // sets sun light to 0.
					current |= 0x0f00;
					
//					chunks_to_update.insert(pos.chunk);
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
		
//		for(const glm::ivec3& pos : propagateLight(propagate_list)){
//			chunks_to_update.insert(pos);
//		}
	}
}

void Lighter::updateBlocklightForBlock(block_position pos, bool placed) {
	
	if(auto [block, found] = worldLoader.getBlock(pos); found && (block & 0xff) != 4){
		region_dtype light = getBlockLight(block);
		
		if(placed){
//			std::list<propagateParam> darks;
//			if (light > 0) {
				disperseLight(pos, blockDarks, getBlockLight(block) - 0x1000 * (light > 0));
//			}
			
			block &= 0x0fff; // Dim block light.
			
//			for(const glm::ivec3& pos : propagateDark(darks, LightType::Block)){
//				chunks_to_update.insert(pos);
//			}
			
		}else{
//			std::list<propagateParam> lights;
			
			region_dtype min_light = 0;
			for(int i = 0; i < 6; i++){
				if(auto [block, found] = worldLoader.getBlock(pos + neighbouring_offsets[i]); found && getBlockLight(block) > min_light){
					min_light = getBlockLight(block);
				}
			}
			
			region_dtype next_light_value = min_light - 0x1000 * (min_light > 0);
			blockLights.push_back(propagateParam(pos, next_light_value));
					
//			for(const glm::ivec3& pos : propagateLight(lights, LightType::Block)){
//				chunks_to_update.insert(pos);
//			}
		}
	}
}

void Lighter::updateLightSource(region_dtype& block, block_position& pos, region_dtype intensity, bool placed) {
	if(placed){
		block &= 0x0fff;
		block |= intensity;
		
//		std::list<propagateParam> prop_list;
		disperseLight(pos, blockLights, intensity - 0x1000);
//		propagateLight(prop_list, LightType::Block);
	}else{
//		std::list<propagateParam> prop_list;
		blockDarks.push_back(propagateParam(pos, getBlockLight(block)));
//		propagateDark(prop_list, LightType::Block);
	}
}

void Lighter::updateLight(std::list<ChangedBlock> blocks) {
	for( auto [position, placed, type, typeBefore] : blocks) {
		updateBlocklightForBlock(position, placed);
		if(type == 4 || (typeBefore & 0xff) == 4){
			auto [block, found] = worldLoader.getBlock(position);
			updateLightSource(block, position, 0xb000, placed);
			printf("lightsource made\n");
		}
		updateSunlightForBlock(position, placed);
	}
}

std::list<propagateParam> Lighter::getLightsToPropagate() {
	auto ret = lights;
	printf("lights ret size: %d\n", ret.size());
	lights.clear();
	return ret;
}

std::list<propagateParam> Lighter::getDarksToPropagate() {
	auto ret = darks;
	darks.clear();
	return ret;
}

std::set<glm::ivec3, compareVec3> Lighter::getUpdatedChunks() {
	auto ret = updatedChunks;
	updatedChunks.clear();
	return ret;	
}

void Lighter::propagateBlockLight() {
	if (!blockLights.empty()) {
		propagateLight(blockLights, LightType::Block);
		blockLights.clear();
	}
	
	if (!blockDarks.empty()) {
		propagateDark(blockDarks, LightType::Block);
		blockDarks.clear();
	}
}

void Lighter::propagateSunLight() {
	if (!lights.empty()) {
		propagateLight(lights);
		lights.clear();
	}
	
	if (!darks.empty()) {
		propagateDark(darks);
		darks.clear();
	}
}

// Don't be an idiot, don't propagate dim lights ( where light_value == 0)!
std::set<glm::ivec3, compareVec> Lighter::propagateLight(std::list<propagateParam>& lights, const LightType& type){
	
	std::set<glm::ivec3, compareVec> affected_chunks;
	
	printf("%s Propagate list size: %d, ", type == LightType::Block ? "BLOCK" : "SUN  ", lights.size());
	printf("\n");
	int insertions = 0;
	int deletions = 0;
	
	while(!lights.empty()){
		propagateParam param = lights.front();
		lights.pop_front();
		deletions++;
		
		if(auto [block, found] = worldLoader.getBlock(param.position); found){
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
			
			if(getBlockType(block) == 0 && light < param.light_value){
				block &= ~light_mask; 		// reset light value.
				block |= param.light_value; // set new light value.
				
				updatedChunks.insert(param.position.chunk);
				
				region_dtype next_light_value = param.light_value - light_increment;// * (param.light_value > light_increment);
				
//				if(next_light_value == light_min)
				if((param.light_value & light_mask) > light_increment){
					disperseLight(param.position, lights, next_light_value);
					insertions += 6;
//					continue;
				}
			}
		}
		
	}
	
	printf("insertions: %d, deletions: %d\n", insertions, deletions);
	
	return affected_chunks;
}

std::set<glm::ivec3, compareVec> Lighter::propagateDark(std::list<propagateParam> darks, const LightType& type){
	
	std::set<glm::ivec3, compareVec> affected_chunks;
	std::list<propagateParam> lights;
	
	printf("[%s] Propagate dark size: %d, ", type == LightType::Block ? "BLOCK" : "SUN  ", darks.size());
	
	int insertions = 0, deletions = 0;
	
	while(!darks.empty()){
		propagateParam param = darks.front();
		darks.pop_front();
		deletions++;
		
//		glm::ivec3 chunk_pos = param.position.chunk;
//		glm::ivec3 in_chunk_pos = param.position.block;
		
		if(auto [block, found] = worldLoader.getBlock(param.position); found){
			
			region_dtype light_increment = 0x100;
			region_dtype light_mask = 0xf00;
			
			if(type == LightType::Block){
				light_increment = 0x1000;
				light_mask = 0xf000;
			}
			
			region_dtype light_min = 0;
			region_dtype light = block & light_mask;
			region_dtype next_light_value = light - light_increment * (light > 0); // Condition is to ensure that
																				   // decrementing light value will not cause an overflow.
			
			if(getBlockType(block) == 0){
				updatedChunks.insert(param.position.chunk);
				
				if(light == param.light_value){
//					block |= light_min; 
					block &= ~light_mask; // light to zero
					
//					propagate dark to neighbours
//					region_dtype next_light_value = param.light_value - light_increment;
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
								disperseLight(param.position + neighbouring_offsets[i], lights, adjecent_light - light_increment);
							}
						}
					}
				}else if(light > param.light_value){
//					printf("I believe you yunyun %x\n", block);
					// propagate LIGHT to neighbours starting from this block
					disperseLight(param.position, lights, next_light_value);
				}
				
			}else if(getBlockType(block) == 4){
				updatedChunks.insert(param.position.chunk);
				disperseLight(param.position, lights, next_light_value);
			}
		}
	}
	
	printf("insertions: %d, deletions: %d\n", insertions, deletions);
	
	printf("inpropagate dark\n");
	// Propagating light should probably be done outside this function.
	for(const glm::ivec3& e : propagateLight(lights, type)){
		affected_chunks.insert(e);
	}
	printf("propagate dark end\n");
	
	return affected_chunks;
}


//Lighter::~Lighter()
//{
//}

