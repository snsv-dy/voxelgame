#include "WorldLoader.h"

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

void WorldLoader::updateTerrain(const int& block_type, const glm::ivec3 &pos, BlockAction action){
	glm::ivec3 chunk_pos, in_chunk_pos;
	std::tie(chunk_pos, in_chunk_pos) = toChunkCoords(pos, WorldLoader::chunkSize);
	
	if(auto c = chunks.find(chunk_pos); c != chunks.end()){
		Chunk& chunk = c->second;
		
		chunk.changeBlock(block_type, in_chunk_pos, action);
		Region& containing_region = provider.getRegion(pos);
		containing_region.modified = true;
		
		updateSunlightForBlock(block_position(in_chunk_pos, chunk_pos), action == BlockAction::PLACE);
		
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
		Chunk& chunk = it->second;
		region_dtype *data;
		int data_offset;
		std::tie(data, data_offset) = chunk.giveData();
		
		int index = pos.block.x + pos.block.y * Chunk::size + pos.block.z * Chunk::size * Chunk::size;
		region_dtype& block = data[data_offset + index];
		
		return {block, true};
	}
	
	return {ref0, false};
}

void WorldLoader::updateSunlightForBlock(block_position pos, bool placed){
	std::set<glm::ivec3, compareVec> chunks_to_update;
	
	region_dtype sunlight_value = 0;
	auto [above,found] = getBlock(pos + glm::ivec3(0, 1, 0));
	
	const region_dtype above_light = above & 0xf00;
	
	if(placed){
		std::list<propagateParam> darks;
		
		bool first = true;
		while(true){
			auto [block, found] = getBlock(pos);
			if(!found || ((block & 0xff) != 0 && !first))
				break;
			
			for(int i = 0; i < 6; i++){
				darks.push_back(propagateParam(pos + neighbouring_offsets[i], (block & 0xf00) + 0x100));
			}
			
			block |= 0xf00;
			
			pos += glm::ivec3(0, -1, 0);
			first = false;
		}
		
		for(const glm::ivec3& pos : propagateDark(darks)){
			chunks_to_update.insert(pos);
		}
		
	}else{
		std::list<propagateParam> propagate_list;
		
		if(found ){ 
			if((above & 0xff) == 0 && above_light == 0){
			// go all the way down until you come across a block.
			// set lights to 0, and then propagate.
		
			auto [current, found] = getBlock(pos);
			
			while(true){
				auto [current, found] = getBlock(pos);
				if(!found || (current & 0xff) != 0)
					break;
				
				current &= 0xf0ff; // sets sun light to 0.
				chunks_to_update.insert(pos.chunk);
				
				// propagate light outwards
				for(int i = 0; i < 4; i++){
					propagate_list.push_back(propagateParam(pos + neighbouring_offsets[i], 0x100));
				}
							
				pos += glm::ivec3(0, -1, 0);
			}
			}else{
				region_dtype max_light = 0xf00;
				for(int i = 0; i < 6; i++){
					auto [block, found] = getBlock(pos + neighbouring_offsets[i]);
					printf("%d found: %d\n", i, found);
					if(found && (block & 0xf00) < max_light){
						max_light = block & 0xf00;
					}
				}
				propagate_list.push_back(propagateParam(pos, max_light + 0x100));
			}
		}
		
		for(const glm::ivec3& pos : propagateLight(propagate_list)){
			chunks_to_update.insert(pos);
		}
	}
	
	for(const glm::ivec3& c : chunks_to_update){
		chunks[c].update_data();
	}
}

std::set<glm::ivec3, compareVec> WorldLoader::propagateDark(std::list<propagateParam> darks){
	
	std::set<glm::ivec3, compareVec> affected_chunks;
	std::list<propagateParam> lights;
	
	while(!darks.empty()){
		propagateParam param = darks.front();
		darks.pop_front();
		
		glm::ivec3 chunk_pos = param.position.chunk;
		glm::ivec3 in_chunk_pos = param.position.block;
		if(auto it = chunks.find(chunk_pos); it != chunks.end()){
			region_dtype* data;
			int data_offset;
			std::tie(data, data_offset) = it->second.giveData();
			
			int index = in_chunk_pos.z * chunkSize * chunkSize + in_chunk_pos.y * chunkSize + in_chunk_pos.x;
			region_dtype& block = data[data_offset + index];
			
			if((block & 0xff) == 0 && (block & 0xf00) < 0xf00){
				
				affected_chunks.insert(chunk_pos);
				region_dtype block_light = block & 0xf00;
				
				if(block_light == param.light_value){
					block |= 0xf00; // sun light to zero
					
//					propagate dark to neighbours
					region_dtype next_light_value = param.light_value + 0x100;
					if( next_light_value < 0xf00 ){
						for(int i = 0; i < 6; i++){
							darks.push_back(propagateParam(param.position + neighbouring_offsets[i], next_light_value));
						}
					}
				}else if(block_light < param.light_value){
					// propagate LIGHT to neighbours starting from this block
					region_dtype& next_light_value = param.light_value;
					for(int i = 0; i < 6; i++){
						lights.push_back(propagateParam(param.position + neighbouring_offsets[i], block_light + 0x100));
					}
				}
			}
		}
	}
	
	// Propagating light should probably be done outside this function.
	for(const glm::ivec3& e : propagateLight(lights)){
		affected_chunks.insert(e);
	}
	
	return affected_chunks;
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
	
	std::tie(chpos, std::ignore) = toChunkCoords(cameraPos, 16);
	
	if(last_camera_pos != chpos || first){
		provider.update(chpos);
		
//		last_camera_pos = chpos;
//		
//		first = false;
//		return;
		
		glm::ivec3 change = chpos - last_camera_pos;
		glm::ivec3 abs_change = glm::abs(change);
		
		int highest_y;
		
		if(abs_change.x > 1 || abs_change.y > 1 || abs_change.z > 1 || first){
			for(int z = -radius; z <= radius; z++){
				for(int y = -radius; y <= radius; y++){
					for(int x = -radius; x <= radius; x++){
						glm::ivec3 pos = glm::ivec3(x + chpos.x, y + chpos.y, z + chpos.z);
	
						if(chunks.find(pos) == chunks.end() && provider.isThereAChunk(pos * chunkSize)){
							
							Region& region = provider.getRegion(pos * chunkSize);
//							std::shared_ptr<char[]> chunk_data = region.getData();
							region_dtype* chunk_data = region.getData();
							int data_offset = region.getChunkOffset(pos * chunkSize);
//							Chunk t(projection, view, textures, shParams, pos, this, chunk_data, data_offset);
							chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);
							
//							Chunk& ch = chunks[pos];
//							int mask[Chunk::size][Chunk::size] = {0};
//							ch.sunlightPass(mask);
//							if( y > highest_y){
//								highest_y = y;
//							}
						}
					}
				}
			}
			
			std::list<propagateParam> lights_to_propagate;
			
			for(int z = -radius; z <= radius; z++){
				for(int x = -radius; x <= radius; x++){
					std::list<propagateParam> tpropagate = updateSunlightInColumn(x, z);
					lights_to_propagate.insert(lights_to_propagate.end(), tpropagate.begin(), tpropagate.end());
					
//					for(int y = -radius; y <= radius; y++){
//						if(auto it = chunks.find(glm::ivec3(x, y, z)); it != chunks.end()){
//							Chunk& a = it->second;
//							a.update_data();
//						}
//					}
				}
			}
			
			std::set<glm::ivec3, compareVec> chunks_to_update = propagateLight(lights_to_propagate);
			
			for(int z = -radius; z <= radius; z++){
				for(int x = -radius; x <= radius; x++){
					for(int y = -radius; y <= radius; y++){
						if(auto it = chunks.find(glm::ivec3(x, y, z)); it != chunks.end()){
							Chunk& a = it->second;
							a.update_data();
						}
					}
				}
			}
//			for(int z = -radius; z <= radius; z++){
//				for(int y = -radius; y <= radius; y++){
//					for(int x = -radius; x <= radius; x++){
//						if(chunks.find(pos) == chunks.end() && provider.isThereAChunk(pos * chunkSize)){
//					}
//				}
//			}
		}else{
			std::set<glm::ivec3, compareVec> light_needed;
//			std::list<glm::ivec3> light_needed;
			
			printf("change: %d %d %d\n", change[0], change[1], change[2]);
			
			for(int i = 0; i < 3; i++){
				if(change[i] == 0){
					continue;
				}
				
				int back = change[i];
				
				int norm = i;
				int tan = (norm + 2) % 3;
				int biTan = (norm + 1) % 3;
				
				glm::ivec3 cursor(0);
				
				std::list<propagateParam> lights_to_propagate;
				std::set<glm::ivec3, compareVec> chunks_to_update;
					
				for(cursor[norm] = 0; cursor[norm] < radius; cursor[norm]++){
					for(cursor[biTan] = -radius; cursor[biTan] <= radius; cursor[biTan]++){
						for(cursor[tan] = -radius; cursor[tan] <= radius; cursor[tan]++){
							cursor[norm] *= back;
							glm::ivec3 pos = cursor + chpos;
							cursor[norm] *= back;
							bool cond1 = chunks.find(pos) == chunks.end();
							bool cond2 = provider.isThereAChunk(pos * chunkSize);
							if(cond1 && cond2){
								
								Region& region = provider.getRegion(pos * chunkSize);
//								std::shared_ptr<char[]> chunk_data = region.getData();
								region_dtype* chunk_data = region.getData();
								int data_offset = region.getChunkOffset(pos * chunkSize);
//								Chunk t(projection, view, textures, shParams, pos, this, chunk_data, data_offset);
		//						t.setLoader(this);
								
								chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);
								light_needed.insert(glm::ivec3(pos.x, 0, pos.z));
								chunks_to_update.insert(pos);
								chunks[pos].update_data();
//								if( y > highest_y){
//									highest_y = y;
//								}
							}
//							else{
//								printf("Chunk is not visible becouse: %d %d\n", cond1, cond2);
//							}
						}
					}
				}
				
				printf("Light needed: %d\n", light_needed.size());
				for(const glm::ivec3& pos : light_needed){
					std::list<propagateParam> tpropagate = updateSunlightInColumn(pos.x, pos.z);
					lights_to_propagate.insert(lights_to_propagate.end(), tpropagate.begin(), tpropagate.end());
				}
//					
//				printf("Propagating light %d\n", lights_to_propagate.size());
				std::set<glm::ivec3, compareVec> chunks_to_updatet = propagateLight(lights_to_propagate);
				for(const glm::ivec3& posz :chunks_to_updatet)
					chunks_to_update.insert(posz);
//				printf("Light propagated (affected chunks %d)\n", chunks_to_update.size());
				
				for(const glm::ivec3& pos : chunks_to_update){
					if(auto it = chunks.find(pos); it != chunks.end()){
						it->second.update_data();
					}
				}
//here
//					
//					cursor[norm] = 0;
//					for(cursor[biTan] = -radius; cursor[biTan] <= radius; cursor[biTan]++){
//						for(cursor[tan] = -radius; cursor[tan] <= radius; cursor[tan]++){
//							cursor[norm] *= back;
//							glm::ivec3 pos = cursor + chpos;
//							cursor[norm] *= back;
//							
//							if(auto it = chunks.find(pos); it != chunks.end()){
//								it->second.update_data();
//							}
//						}
//					}
//here
//					for(const glm::ivec3& column : light_needed){
//						for(int x = -radius; x <= radius; x++){
//							if(auto it = chunks.find(glm::ivec3(x, y, z)); it != chunks.end()){
//								Chunk& a = it->second;
//								a.update_data();
//							}
//						}
//					}
			}
			
			
//			for(int y = -radius; y <= radius; y++){
			
//			}
		}
		
		// Removing chunks
		for(auto i = chunks.begin(), nexti = i; i != chunks.end(); i = nexti){
			nexti++;
			if(ivec3len2(i->first - chpos) > unloadRadiusSquared){
				glm::ivec3 pos = i->first;
//				printf("Clearing chunk (%d %d %d)\n", pos.x, pos.y, pos.z);
				chunks.erase(i);
			}
		}
		
		last_camera_pos = chpos;
		
		first = false;
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
		int mask[Chunk::size][Chunk::size] = {0};
		
		
		while(true){
			bool allDark = false;
			if(auto it = chunks.find(top); it != chunks.end()){
				Chunk& ch = it->second;
				
				
				if(!allDark){
					std::list<propagateParam> tpropagate = ch.sunlightPass(mask, allDark);
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


std::set<glm::ivec3, compareVec> WorldLoader::propagateLight(std::list<propagateParam>& lights){
	
	std::set<glm::ivec3, compareVec> affected_chunks;
	
	printf("Propagate list size: %d, ", lights.size());
	int insertions = 0;
	int deletions = 0;
	
	while(!lights.empty()){
		propagateParam param = lights.front();
		lights.pop_front();
		deletions++;
		
		glm::ivec3 chunk_pos = param.position.chunk;
		glm::ivec3 in_chunk_pos = param.position.block;
		
		if(auto it = chunks.find(chunk_pos); it != chunks.end()){
			region_dtype* data;
			int data_offset;
			std::tie(data, data_offset) = it->second.giveData();
			
			int index = in_chunk_pos.z * chunkSize * chunkSize + in_chunk_pos.y * chunkSize + in_chunk_pos.x;
			region_dtype& block = data[data_offset + index];
			
			if((block & 0xff) == 0 && (block & 0xff00) > param.light_value){
				block &= 0xff;
				block |= param.light_value;
				
				affected_chunks.insert(chunk_pos);
				
				region_dtype next_light_value = param.light_value + 0x100;
				
				if(next_light_value == 0xf00){
					continue;
				}
					
				for(int i = 0; i < 6; i++){
					lights.push_back(propagateParam(param.position + neighbouring_offsets[i], next_light_value));
					insertions++;
				}
				
			}
		}
		
	}
	
	printf("insertions: %d, deletions: %d\n", insertions, deletions);
	
	return affected_chunks;
}