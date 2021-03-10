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

std::tuple<glm::ivec3, glm::ivec3> WorldLoader::collideRay(const glm::vec3& origin, const glm::vec3& direction, const int& range){
	float step = 0.1f;
	float range_mul = step;
	int n = (int)((float)range / step);
	glm::ivec3 prev_pos {0};
	glm::ivec3 int_pos {0};
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
		
		if(provider.valueAt(int_pos.x, int_pos.y, int_pos.z) != 0){
			break;
		}
		
		prev_pos = int_pos;
	}
	
	return std::make_tuple(int_pos, prev_pos);
}

void WorldLoader::updateTerrain(const glm::ivec3 &pos, BlockAction action){
	glm::ivec3 chunk_pos, in_chunk_pos;
	std::tie(chunk_pos, in_chunk_pos) = toChunkCoords(pos, WorldLoader::chunkSize);
	
	if(auto c = chunks.find(chunk_pos); c != chunks.end()){
		Chunk& chunk = c->second;
		
		chunk.changeBlock(in_chunk_pos, action);
		
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

char WorldLoader::valueAt(int x, int y, int z){
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
		provider.update(cameraPos);
		
		glm::ivec3 change = chpos - last_camera_pos;
		glm::ivec3 abs_change = glm::abs(change);
		
		if(abs_change.x > 1 || abs_change.y > 1 || abs_change.z > 1 || first){
			for(int z = -radius; z <= radius; z++){
				for(int y = -radius; y <= radius; y++){
					for(int x = -radius; x <= radius; x++){
						glm::ivec3 pos = glm::ivec3(x + chpos.x, y + chpos.y, z + chpos.z);
	
						if(chunks.find(pos) == chunks.end() && provider.isThereAChunk(pos * chunkSize)){
							
							Region& region = provider.getRegion(pos * chunkSize);
//							std::shared_ptr<char[]> chunk_data = region.getData();
							char* chunk_data = region.getData();
							int data_offset = region.getChunkOffset(pos * chunkSize);
//							Chunk t(projection, view, textures, shParams, pos, this, chunk_data, data_offset);
							chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);
						}
					}
				}
			}
			
		}else{
			for(int i = 0; i < 3; i++){
				if(change[i] == 0){
					continue;
				}
				
				int back = change[i];
				
				int norm = i;
				int tan = (norm + 2) % 3;
				int biTan = (norm + 1) % 3;
				
				glm::ivec3 cursor(0);
				
				for(cursor[norm] = 0; cursor[norm] <= radius; cursor[norm]++){
					for(cursor[biTan] = -radius; cursor[biTan] <= radius; cursor[biTan]++){
						for(cursor[tan] = -radius; cursor[tan] <= radius; cursor[tan]++){
							cursor[norm] *= back;
							glm::ivec3 pos = cursor + chpos;
							cursor[norm] *= back;
							
							if(chunks.find(pos) == chunks.end() && provider.isThereAChunk(pos * chunkSize)){
								
								Region& region = provider.getRegion(pos * chunkSize);
//								std::shared_ptr<char[]> chunk_data = region.getData();
								char* chunk_data = region.getData();
								int data_offset = region.getChunkOffset(pos * chunkSize);
//								Chunk t(projection, view, textures, shParams, pos, this, chunk_data, data_offset);
		//						t.setLoader(this);
								chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);;
							}
						}
					}
				}
			}
		}
		
		// Removing chunks
		for(auto i = chunks.begin(), nexti = i; i != chunks.end(); i = nexti){
			nexti++;
			if(ivec3len2(i->first - chpos) > unloadRadiusSquared){
				glm::vec3 pos = i->first;
//				printf("Clearing chunk %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);
				chunks.erase(i);
			}
		}
		
		last_camera_pos = chpos;
		
		first = false;
	}
}