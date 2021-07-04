#include "WorldLoader.h"

void inline disperseLight(block_position pos, std::list<propagateParam>& lights, region_dtype light_value){
	for(int i = 0; i < 6; i++){
		lights.push_back(propagateParam(pos + neighbouring_offsets[i], light_value));
	}
}

WorldLoader::WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params){
	last_camera_pos = glm::ivec3(0);
	
	this->projection = projection;
	this->view = view;
	this->textures = textures;
	
	this->shParams = params;
}

void WorldLoader::draw(glm::vec3 cameraPos){
	
	glm::ivec3 chunkPos;
	std::tie(chunkPos, std::ignore) = toChunkCoords(cameraPos, WorldLoader::chunkSize);
	
	glm::ivec3 off = glm::ivec3(0.0);
	chunkPos.y = 0;
	
	glBindTexture(GL_TEXTURE_2D, textures);
	
	glUseProgram(shParams.program);
	glUniformMatrix4fv(shParams.projection, 1, GL_FALSE, glm::value_ptr(*projection));
	glUniformMatrix4fv(shParams.view, 1, GL_FALSE, glm::value_ptr(*view));
	
	for(off.z = -radius + chunkPos.z; off.z <= radius + chunkPos.z; off.z++){
		for(off.y = -radius + chunkPos.y; off.y <= radius + chunkPos.y; off.y++){
			for(off.x = -radius + chunkPos.x; off.x <= radius + chunkPos.x; off.x++){
				
				if(auto chunk_iter = chunks.find(off); chunk_iter != chunks.end()){
					auto& chunk = chunk_iter->second;
//					glUniformMatrix4fv(shParams.view, 1, GL_FALSE, glm::value_ptr(chunk.mesh));
					chunk.draw();
				}
			}
		}
	}
}

std::tuple<glm::ivec3, glm::ivec3, region_dtype> WorldLoader::collideRay(const glm::vec3& origin, const glm::vec3& direction, const int& range){
	float step = 0.1f;
	float range_mul = step;
	int n = (int)((float)range / step);
	glm::ivec3 prev_pos {0};
	glm::ivec3 int_pos {0};
	region_dtype last_block = 0;
	for(int i = 0; i < n; i++, range_mul += 0.1f){
//		glm::ivec3 chunk_pos;
//		std::tie(chunk_pos, std::ignore) = toChunkCoords(origin + direction * range_mul, WorldLoader::chunkSize);
		glm::vec3 pos = origin + direction * range_mul;
		int_pos = pos;
		
//		int mx = (-value - 1);
//		chunk_index = -(mx / chunkSize + 1);
		if(pos.x < 0){
			int_pos.x -= 1;
		}
		if(pos.y < 0){
			int_pos.y -= 1;
		}
		if(pos.z < 0){
			int_pos.z -= 1;
		}
		
		last_block = provider.valueAt(int_pos.x, int_pos.y, int_pos.z);
		if(block_type(last_block) != 0){
			break;
		}
		
		prev_pos = int_pos;
	}
	
	return std::make_tuple(int_pos, prev_pos, last_block);
}

void WorldLoader::updateLightSource(region_dtype& block, block_position& pos, region_dtype intensity, bool placed){
	if(placed){
		block &= 0x0fff;
		block |= intensity;
		
		std::list<propagateParam> prop_list;
		disperseLight(pos, prop_list, intensity - 0x1000);
		propagateLight(prop_list, LightType::Block);
	}else{
		std::list<propagateParam> prop_list;
		prop_list.push_back(propagateParam(pos, getBlockLight(block)));
		propagateDark(prop_list, LightType::Block);
	}
}

void WorldLoader::updateTerrain(const int& block_type, const glm::ivec3 &pos, BlockAction action){
	glm::ivec3 chunk_pos, in_chunk_pos;
	std::tie(chunk_pos, in_chunk_pos) = toChunkCoords(pos, WorldLoader::chunkSize);
	block_position block_pos = block_position(in_chunk_pos, chunk_pos);
	
	if(auto [block, found] = getBlock(block_pos); found) {
		region_dtype block_before = block;
		
		chunks[block_pos.chunk].changeBlock(block_type, in_chunk_pos, action);
//		Region& containing_region = provider.getRegion(pos);
//		containing_region.modified = true;
		// Bug is when loading/meshing/most likely calculating sunlight for new chunk with blocklight

		printf("updating blocklight\n");
		updateBlocklightForBlock(block_pos, action == BlockAction::PLACE);
		
		if(block_type == 4 || (block_before & 0xff) == 4){
			updateLightSource(block, block_pos, 0xb000, action == BlockAction::PLACE);
			printf("lightsource made\n");
		}
		
		printf("updating sunlight\n");
		updateSunlightForBlock(block_pos, action == BlockAction::PLACE);
		
		printf("lights updated\n");
		// Not enough adjacent chunks are updated. ( Check -1 0/1 -1 cursor position)
		
		// I tried to make it not ugly, but in the end it didn't even matter. :<
		glm::ivec3 update_positions[3];
		bool update_condition[3] {false};
		
		if(in_chunk_pos.x == 0){
			update_positions[0] = chunk_pos - glm::ivec3(1, 0, 0);
			update_condition[0] = true;
		}else if(in_chunk_pos.x == Chunk::size - 1){
			update_positions[0] = chunk_pos + glm::ivec3(1, 0, 0);
			update_condition[0] = true;
		}
		
		if(in_chunk_pos.y == 0){
			update_positions[1] = chunk_pos + glm::ivec3(0, 1, 0);
			update_condition[1] = true;
		}else if(in_chunk_pos.y == Chunk::size - 1){
			update_positions[1] = chunk_pos - glm::ivec3(0, 1, 0);
			update_condition[1] = true;
		}
		
		if(in_chunk_pos.z == 0){
			update_positions[2] = chunk_pos - glm::ivec3(0, 0, 1);
			update_condition[2] = true;
		}else if(in_chunk_pos.z == Chunk::size - 1){
			update_positions[2] = chunk_pos + glm::ivec3(0, 0, 1);
			update_condition[2] = true;
		}
		
		for(int i = 0; i < 3; i++){
			if(update_condition[i]){
				if(auto c = chunks.find(update_positions[i]); c != chunks.end()){
					c->second.update_data();
				}
			}
		}
	}
}
//
std::pair<region_dtype&, bool> WorldLoader::getBlock(const block_position& pos){
	if(auto it = chunks.find(pos.chunk); it != chunks.end()){
		region_dtype *data;
		int data_offset;
		std::tie(data, data_offset) = it->second.giveData();
		
		int index = pos.block.x + pos.block.y * Chunk::size + pos.block.z * Chunk::size * Chunk::size;
		region_dtype& block = data[data_offset + index];
		
		return {block, true};
	}
	
	return {ref0, false};
}

void WorldLoader::updateBlocklightForBlock(block_position pos, bool placed) {
	
	if(auto [block, found] = getBlock(pos); found && (block & 0xff) != 4){
		region_dtype light = getBlockLight(block);
		
		if(placed){
			std::list<propagateParam> darks;
//			if (light > 0) {
				disperseLight(pos, darks, getBlockLight(block) - 0x1000 * (light > 0));
//			}
			
			block &= 0x0fff; // Dim block light.
			
			for(const glm::ivec3& pos : propagateDark(darks, LightType::Block)){
				chunks_to_update.insert(pos);
			}
			
		}else{
			std::list<propagateParam> propagate_list;
			
			region_dtype min_light = 0;
			for(int i = 0; i < 6; i++){
				if(auto [block, found] = getBlock(pos + neighbouring_offsets[i]); found && getBlockLight(block) > min_light){
					min_light = getBlockLight(block);
				}
			}
			
			region_dtype next_light_value = min_light - 0x1000 * (min_light > 0);
			propagate_list.push_back(propagateParam(pos, next_light_value));
					
			for(const glm::ivec3& pos : propagateLight(propagate_list, LightType::Block)){
				chunks_to_update.insert(pos);
			}
		}
	}
}

void WorldLoader::updateSunlightForBlock(block_position pos, bool placed){
	
	region_dtype sunlight_value = 0;
	auto [above,found] = getBlock(pos + glm::ivec3(0, 1, 0));
	
	const region_dtype above_light = getSunLight(above);
	
	if(placed){
		std::list<propagateParam> darks;
		
		bool first = true;
		while(true){
			auto [block, found] = getBlock(pos);
			if(!found || (getBlockType(block) != 0 && !first))
				break;
			
			disperseLight(pos, darks, getSunLight(block) - 0x100); // tutaj ?
			
//			block |= 0xf00;
			block &= 0xf0ff; // darkens sunlight.
			
			pos += glm::ivec3(0, -1, 0);
			first = false;
		}
		
		for(const glm::ivec3& pos : propagateDark(darks)){
			chunks_to_update.insert(pos);
		}
		
	}else{
		std::list<propagateParam> propagate_list;
		
		if(found ){ 
			if(getBlockType(above) == 0 && above_light == 0xf00){
				// go all the way down until you come across a block.
				// set lights to 0, and then propagate.
		
				auto [current, found] = getBlock(pos);
				
				while(true){
					auto [current, found] = getBlock(pos);
					if(!found || getBlockType(current) != 0)
						break;
					
//					current &= 0xf0ff; // sets sun light to 0.
					current |= 0x0f00;
					
					chunks_to_update.insert(pos.chunk);
					
					// propagate light outwards
					for(int i = 0; i < 4; i++){
						propagate_list.push_back(propagateParam(pos + neighbouring_offsets[i], 0xe00));
					}
								
					pos += glm::ivec3(0, -1, 0);
				}
			}else{
				region_dtype min_light = 0;
				for(int i = 0; i < 6; i++){
					auto [block, found] = getBlock(pos + neighbouring_offsets[i]);
					
					if(found && getSunLight(block) > min_light){
						min_light = getSunLight(block);
					}
				}
				
				if(min_light > 0)
					propagate_list.push_back(propagateParam(pos, min_light - 0x100) );
			}
		}
		
		for(const glm::ivec3& pos : propagateLight(propagate_list)){
			chunks_to_update.insert(pos);
		}
	}
}

region_dtype WorldLoader::valueAt(int x, int y, int z){
	return provider.valueAt(x, y, z);

	return false;
}

inline float ivec3len2(glm::ivec3 a){
	return a.x*a.x + a.b*a.b + a.z*a.z;
}

void WorldLoader::update(glm::vec3 cameraPos){
	glm::ivec3 chpos;
	
	cameraPos.y = 0.0f;
	
	std::tie(chpos, std::ignore) = toChunkCoords(cameraPos, TerrainConfig::ChunkSize);
	
	std::list<propagateParam> lights_to_propagate;
	if(last_camera_pos != chpos || first){
		provider.update(chpos);
		
		glm::ivec3 change = chpos - last_camera_pos;
		glm::ivec3 abs_change = glm::abs(change);
		
		if(abs_change.x > 1 || abs_change.y > 1 || abs_change.z > 1 || first) {
			abs_change = glm::ivec3(radius * 2);
		}
		
		std::set<glm::ivec3, compareVec> light_needed;
//		printf("change: %d %d %d\n", change[0], change[1], change[2]);
		
		for(int i = 0; i < 3; i++){
			if(abs_change[i] == 0){
				continue;
			}
			
			int back = change[i] > 0 ? 1 : -1;
			
			int norm = i;
			int tan = (norm + 2) % 3;
			int biTan = (norm + 1) % 3;
			
			glm::ivec3 cursor(0);
			
			int updateRange = radius - abs_change[i];
			
			for (cursor[norm] = updateRange; cursor[norm] < radius; cursor[norm]++) {
				for (cursor[biTan] = -radius; cursor[biTan] <= radius; cursor[biTan]++) {
					for (cursor[tan] = -radius; cursor[tan] <= radius; cursor[tan]++) {
						cursor[norm] *= back;
						glm::ivec3 pos = chpos + cursor;
						cursor[norm] *= back;
						
						if (chunks.find(pos) == chunks.end()) {
							loadChunk(pos, &light_needed);
						}
					}
				}
			}
			
			for (const glm::ivec3& pos : light_needed) {
				std::list<propagateParam> tpropagate = updateSunlightInColumn(pos.x, pos.z);
				lights_to_propagate.insert(lights_to_propagate.end(), tpropagate.begin(), tpropagate.end());
			}
			
		}
		propagateLight(lights_to_propagate);
		
		// Removing chunks
		for(auto i = chunks.begin(), nexti = i; i != chunks.end(); i = nexti){
			nexti++;
			if(ivec3len2(i->first - chpos) > unloadRadiusSquared){
				glm::ivec3 pos = i->first;
				chunks.erase(i);
			}
		}
		
		last_camera_pos = chpos;
		
		first = false;
	}
	
	if(!chunks_to_update.empty()){
		for(const glm::ivec3& c : chunks_to_update){
			if(auto it = chunks.find(c); it != chunks.end())
				it->second.update_data();
		}
		chunks_to_update.clear();
	}
}

void WorldLoader::loadChunk(const glm::ivec3& pos, std::set<glm::ivec3, compareVec> *light_needed) {
	auto [chunk_data, data_offset] = provider.getChunkData(pos);
	if(chunk_data != nullptr) {
		chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);
		if(light_needed != nullptr) {
			light_needed->insert(glm::ivec3(pos.x, 0, pos.z));
		}
		chunks_to_update.insert(pos);
	}
}

//
//enum class direction {
//	LEFT,
//	RIGHT,
//	FRONT,
//	BACK
//};

struct DirAndSide{
	glm::ivec3 position;
	Direction face;
};

// Direction to get position of an adjecent chunk.
DirAndSide directions[4] {
	{glm::ivec3(-1, 0, 0), Direction::RIGHT},
	{glm::ivec3(1, 0, 0), Direction::LEFT},
	{glm::ivec3(0, 0, 1), Direction::FRONT},
	{glm::ivec3(0, 0, -1), Direction::BACK}
};

std::list<propagateParam> WorldLoader::updateSunlightInColumn(int x, int z){
//	return {};
	glm::ivec3 top = last_camera_pos;
	top.z = z;
	top.x = x;
	
	top.y += unloadRadius;
	
	int bottom = last_camera_pos.y - unloadRadius;
	
	while(top.y > bottom && chunks.find(top) == chunks.end()){
		top.y--;
	}
	
			
	std::list<propagateParam> lights_to_propagate;
					
	if(top.y > bottom){
		int mask[Chunk::size][Chunk::size];
		for (int y = 0; y < Chunk::size; y++){
			for (int x = 0; x < Chunk::size; x++){
				mask[y][x] = 0xf00;
			}
		}
		
		
		while(true){
			bool allDark = false;
			if(auto it = chunks.find(top); it != chunks.end()){
				Chunk& ch = it->second;
				
				
				if(!allDark){
					std::list<propagateParam> tpropagate = ch.sunlightPass(mask, allDark);
					if(allDark)
						printf("");
					lights_to_propagate.insert(lights_to_propagate.end(), tpropagate.begin(), tpropagate.end());
				}
					
				// Get light coming from adjecent chunks
				for(const DirAndSide& dir : directions){
					if(auto it = chunks.find(dir.position + top); it != chunks.end()){
						std::list<propagateParam> side_emit = it->second.getEmmitedLightFromSide(dir.face);
						lights_to_propagate.insert(lights_to_propagate.end(), side_emit.begin(), side_emit.end());
					}
				}
				
			}else{
				break;
			}
			top.y--;
		}
	}else{
		printf("[Light] Invalid column? (%d %d)\n", x, z);
	}
	
	return lights_to_propagate;
}

// Don't be an idiot, don't propagate dim lights ( where light_value == 0)!
std::set<glm::ivec3, compareVec> WorldLoader::propagateLight(std::list<propagateParam>& lights, const LightType& type){
	
	std::set<glm::ivec3, compareVec> affected_chunks;
	
	printf("%s Propagate list size: %d, ", type == LightType::Block ? "BLOCK" : "SUN  ", lights.size());
	printf("\n");
	int insertions = 0;
	int deletions = 0;
	
	while(!lights.empty()){
		propagateParam param = lights.front();
		lights.pop_front();
		deletions++;
		
		if(auto [block, found] = getBlock(param.position); found){
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
				
				chunks_to_update.insert(param.position.chunk);
				
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

std::set<glm::ivec3, compareVec> WorldLoader::propagateDark(std::list<propagateParam> darks, const LightType& type){
	
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
		
		if(auto [block, found] = getBlock(param.position); found){
			
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
				chunks_to_update.insert(param.position.chunk);
				
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
							auto [adjecent_block, found] = getBlock(param.position + neighbouring_offsets[i]);
							
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
				chunks_to_update.insert(param.position.chunk);
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
