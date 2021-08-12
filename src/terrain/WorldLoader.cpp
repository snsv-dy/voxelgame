#include "WorldLoader.h"

WorldLoader::WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params, std::shared_ptr<WorldProvider> provider) 
// : provider{std::move(std::make_unique<LocalWorldProvider>())} {
: provider{std::move(provider)} {
	last_camera_pos = glm::ivec3(0);
	
	this->projection = projection;
	this->view = view;
	this->textures = textures;
	
	this->shParams = params;
}

WorldLoader::WorldLoader(std::shared_ptr<WorldProvider> provider): provider{std::move(provider)}  {

}

void WorldLoader::draw(glm::vec3 cameraPos, const glm::mat4& view) {
	
	glm::ivec3 chunkPos;
	std::tie(chunkPos, std::ignore) = toChunkCoords(cameraPos, WorldLoader::chunkSize);
	
	glm::ivec3 off = glm::ivec3(0.0);
	chunkPos.y = 0;
	
	glBindTexture(GL_TEXTURE_2D, textures);
	
	glUseProgram(shParams.program);
	glUniformMatrix4fv(shParams.projection, 1, GL_FALSE, glm::value_ptr(*projection));
	glUniformMatrix4fv(shParams.view, 1, GL_FALSE, glm::value_ptr(view));
	
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
		
		// last_block = provider->valueAt(int_pos.x, int_pos.y, int_pos.z);
		
		if (auto [block, found] = getBlock(pos); found) {
			last_block = block;
		} else {
			last_block = 0;
		}

		if(block_type(last_block) != 0){
			break;
		}
		
		prev_pos = int_pos;
	}
	
	return std::make_tuple(int_pos, prev_pos, last_block);
}

void WorldLoader::updateTerrain(const int& block_type, const glm::ivec3 &pos, BlockAction action) {
	glm::ivec3 chunk_pos, in_chunk_pos;
	std::tie(chunk_pos, in_chunk_pos) = toChunkCoords(pos, WorldLoader::chunkSize);
	block_position block_pos = block_position(in_chunk_pos, chunk_pos);
	
	if(auto [block, found] = getBlock(block_pos); found) {
		region_dtype block_before = block;
		if (action == BlockAction::DESTROY)
			printf("block destroyed\n");

		chunks[block_pos.chunk].changeBlock(block_type, in_chunk_pos, action);
//		Region& containing_region = provider->getRegion(pos);
//		containing_region.modified = true;
		// Bug is when loading/meshing/most likely calculating sunlight for new chunk with blocklight
		// fixed?
		blocks_changed.push_back({block_pos, action == BlockAction::PLACE, (const region_dtype)block_type, block_before});
		chunks_to_update.insert(chunk_pos);
		
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
		
		for(int i = 0; i < 3; i++) {
			if(update_condition[i]) {
				if(auto c = chunks.find(update_positions[i]); c != chunks.end()) {
					chunks_to_update.insert(update_positions[i]);
					// c->second.update_data();
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

std::pair<region_dtype&, bool> WorldLoader::getBlock(const glm::vec3& pos) {
	auto [chunkPosition, blockPosition] = toChunkCoordsReal(pos, TerrainConfig::ChunkSize);
	block_position blockPos = block_position(blockPosition, chunkPosition);
	return getBlock(blockPos);
}

region_dtype WorldLoader::valueAt(int x, int y, int z) {
	auto [chunkPosition, blockPosition] = toChunkCoords(glm::ivec3(x, y, z), TerrainConfig::ChunkSize);
	if( auto it = chunks.find(chunkPosition); it != chunks.end() ) {
		auto [data, offset] =  it->second.giveData();
		return data[offset + blockPosition.z * Chunk::size_squared + blockPosition.y * TerrainConfig::ChunkSize + blockPosition.x];
	}

	// Returning opaque block when not found seem to be working for removing unnecessary meshes inside chunks, but look into it if some problems emerge.
	return BLOCK_NOT_FOUND;
}

inline float ivec3len2(glm::ivec3 a){
	return a.x*a.x + a.b*a.b + a.z*a.z;
}

void WorldLoader::checkForUpdate(glm::vec3 cameraPos) {
	// Only for net!!
	provider->update(glm::ivec3(0));
	glm::ivec3 chpos;
	
	cameraPos.y = 0.0f;
	
	std::tie(chpos, std::ignore) = toChunkCoords(cameraPos, TerrainConfig::ChunkSize);
	glm::ivec3 change = chpos - last_camera_pos;
	glm::ivec3 abs_change = glm::abs(change);
	bool remoteChunks = provider->newChunksAvailable();
	if (remoteChunks) {
		printf("New chunks available\n");
		// first = true;
	}
	
	if (!notified && (last_camera_pos != chpos || first || !chunks_to_update.empty() || remoteChunks)) {
		cur_camera_pos = chpos;
		{
			std::scoped_lock lock(updateMutex);
			notified = true;
		}
		updateNotifier.notify_one();
		// printf("notifying at: %d %d %d \n", chpos[0], chpos[1], chpos[2]);
	}
}

void WorldLoader::update(glm::ivec3 change) {
	// glm::ivec3 chpos;
	
	// cameraPos.y = 0.0f;
	
	// std::tie(chpos, std::ignore) = toChunkCoords(cameraPos, TerrainConfig::ChunkSize);
	
	// std::list<propagateParam> lights_to_propagate;
	if (true) {
		printf("updating: %d %d %d \n", change[0], change[1], change[2]);
	// if (last_camera_pos != chpos || first) {
		light_needed.clear();
		provider->update(cur_camera_pos);
		// printf("provider after\n");
		
		// glm::ivec3 change = chpos - last_camera_pos;
		glm::ivec3 abs_change = glm::abs(change);
		
//		if(abs_change.x > 1 || abs_change.y > 1 || abs_change.z > 1 || first) {
		if (first) {
			abs_change = glm::ivec3(radius * 2, 0, 0); // This refreshes whole observable terrain.
		}
		// else{
		// 	abs_change = glm::ivec3(0);
		// }
		
		
//		printf("change: %d %d %d\n", change[0], change[1], change[2]);
		

		if (provider->newChunksAvailable()) {
			printf("getting new Chunks\n");
			for (const glm::ivec3& pos : provider->getNewChunks()) {
				printf("NEW CHUNKZ: %2d %2d %2d \n", pos.x, pos.y, pos.z);
				loadChunk(pos, &light_needed);
			}
		}

		for(int i = 0; i < 3; i++) {
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
						glm::ivec3 pos = cur_camera_pos + cursor;
						cursor[norm] *= back;
						
						if (chunks.find(pos) == chunks.end()) {
							loadChunk(pos, &light_needed);
						}
					}
				}
			}
		}
		
		// Removing chunks
		if(true) {
			disposableChunksMutex.lock();
			for(auto i = chunks.begin(), nexti = i; i != chunks.end(); i = nexti) {
				nexti++;
				if(ivec3len2(i->first - cur_camera_pos) > unloadRadiusSquared) {
					disposable_chunks.insert(i->first);
				}
			}
			disposableChunksMutex.unlock();
		}
		
		last_camera_pos = cur_camera_pos;
		
		first = false;
	}
}

void WorldLoader::disposeChunks() {
	if (disposableChunksMutex.try_lock()) { // trylock
		if (!disposable_chunks.empty()) {
			for (const glm::ivec3& i : disposable_chunks) {
			// for(auto i = chunks.begin(), nexti = i; i != chunks.end(); i = nexti) {
				chunks.erase(i);
			}
			disposable_chunks.clear();
		}
		disposableChunksMutex.unlock();
	}
}

void WorldLoader::prepareGeometry() {
	if(!chunks_to_update.empty()) {
		for(const glm::ivec3& c : chunks_to_update) {
			if(auto it = chunks.find(c); it != chunks.end()) {
				it->second.prepareUpdateData();
			}
		}
		prepareSetMutex.lock();
		for(const glm::ivec3& c : chunks_to_update) {
			prepared_chunks.insert(c);
			provider->notifyChange(c);
		}
		prepareSetMutex.unlock();

		chunks_to_update.clear();
	}
	light_needed.clear();
}

void WorldLoader::updateGeometry() {
	if(!prepared_chunks.empty()) {
		if(prepareSetMutex.try_lock()) {
			// prepareSetMutex.lock();
			for(const glm::ivec3& c : prepared_chunks) {
				if(auto it = chunks.find(c); it != chunks.end()) {
					it->second.update_data();
				}
			}
			prepared_chunks.clear();
			prepareSetMutex.unlock();
		}
	}
}

std::list<ChangedBlock> WorldLoader::getChangedBlocks() {
	auto ret = blocks_changed;

	for (auto block : blocks_changed) {
		provider->changeBlock(block);
	}

	blocks_changed.clear();
	return ret;
}

void WorldLoader::addUpdatedChunks(std::set<glm::ivec3, compareVec3> updatedChunks) {
	for( const glm::ivec3& pos : updatedChunks) {
		chunks_to_update.insert(pos);
	}
}

std::set<glm::ivec3, compareVec3> WorldLoader::getUnlitColumns() {
	std::set<glm::ivec3, compareVec3> ret = light_needed;
	light_needed.clear();
	return ret;
}

void WorldLoader::loadChunk(const glm::ivec3& pos, std::set<glm::ivec3, compareVec3> *light_needed) {
	auto [chunk_data, data_offset, generated] = provider->getChunkData(pos);
	if(chunk_data != nullptr) {
		chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);
		if (light_needed != nullptr && generated) {
			light_needed->insert(glm::ivec3(pos.x, 0, pos.z));
		}
		chunks_to_update.insert(pos);
	}
}

std::pair<Chunk&, bool> WorldLoader::getChunk(glm::ivec3 position) {
	if(auto chunk = chunks.find(position); chunk != chunks.end()) {
		return {chunk->second, true};
	}
	
	return {chunks.begin()->second, false};
}

//std::pair<bool, glm::vec3> WorldLoader::playerIntersects(Player& player) {
glm::vec3 WorldLoader::collideAABB(const AABB& aabb, glm::vec3& velocity, glm::vec3 dx) {
	float dxLength = glm::length(dx);
	if (dxLength > 0.0001f) {
		int axis = -1;
		int loops = 0;
		bool hitted = true;
		while(hitted && dxLength > 0.0001f && loops < 5) {
			loops++;
			hitted = false;
			float t = fastAABB(aabb, dx, dxLength, axis);
			if (axis != -1) {
				hitted = true;
				velocity[axis] = 0.0f;						
				dx[axis] = 0.0f;
			}
			dxLength = glm::length(dx);
		}
	}
	
	return dx;
}

// As mentioned in javascript script this algorithm is from https://github.com/andyhall/voxel-aabb-sweep
float WorldLoader::fastAABB(const AABB& aabb, glm::vec3 direction, float maxt, int& collision_axis) {
//	penetration = glm::vec3(0.0f);
	glm::vec3 normalDirection = glm::normalize(direction);
	
	int step[3]; // In which direction we are moving in each axis.
	for (int i = 0; i < 3; i++) {
		step[i] = direction[i] > 0 ? 1 : -1;
	}
	
	glm::vec3 origin;
	for(int i = 0; i < 3; i++) {
		origin[i] = step[i] == 1 ? aabb.max[i] : aabb.min[i];
	}
	
	glm::ivec3 ilead;
	glm::ivec3 itrail;
	for (int i = 0; i < 3; i++) {
		ilead[i] = step[i] == 1 ? floor(aabb.max[i]) : floor(aabb.min[i]);
		itrail[i] = step[i] == 1 ? floor(aabb.min[i]) : floor(aabb.max[i]);
	}
	
	float scaling[3]; // How far along the ray we have to move to get to the next voxel.
	scaling[0] = sqrt( 1 + pow(normalDirection.z / normalDirection.x, 2));
	scaling[2] = sqrt( 1 + pow(normalDirection.x / normalDirection.z, 2));
	float xz = sqrt(normalDirection.x * normalDirection.x + normalDirection.z * normalDirection.z);
	scaling[1] = sqrt( 1 + pow(xz / normalDirection.y, 2));
	
	float tMax[3]; // Value of t where ray crosses the first voxel boundary in each axis.
	for (int i = 0; i < 3; i++) {
		if (abs(normalDirection[i]) < 0.001f) {
			tMax[i] = INFINITY;
			continue;
		}
		
		tMax[i] = step[i] >= 0 ? (float) floor(origin[i]) + 1 - origin[i] : origin[i] - (float) floor(origin[i]);
		tMax[i] *= scaling[i];
	}
	collision_axis  = tMax[0] < tMax[1] ?
						tMax[0] < tMax[2] ? 0 : 2 :
						tMax[1] < tMax[2] ? 1 : 2;
	
	float t = 0.0f;
	bool collision = false;
	while( t < maxt) {
		
		// Collision checking.
		glm::ivec3 beg;
		glm::ivec3 end;
		for (int i = 0; i < 3; i++) {
			beg[i] = collision_axis == i ? ilead[i] : itrail[i];
			end[i] = ilead[i] + step[i];
		}
		
		glm::ivec3 cursor;
		for (cursor.z = beg.z; cursor.z != end.z; cursor.z += step[2]) {
			for (cursor.y = beg.y; cursor.y != end.y; cursor.y += step[1]) {
				for (cursor.x = beg.x; cursor.x != end.x; cursor.x += step[0]) {
					if (auto [block, found] = getBlock(cursor); found && getBlockType(block) != 0) {
						// Left collision point in case i would need it in the future.
//						penetration.x = origin.x + normalDirection.x * t;
//						penetration.y = origin.y + normalDirection.y * t;
//						penetration.z = origin.z + normalDirection.z * t;
						
						collision = true;
						return t;
					}
				}
			}
		}
		// End of collision checking.
		
		
		
		// Stepping to next voxel
		int axis = (tMax[0] < tMax[1]) ? 
			(tMax[0] < tMax[2]) ? 0 : 2 : 
			(tMax[1] < tMax[2]) ? 1 : 2;
			
		t = tMax[axis];
		tMax[axis] += scaling[axis];
		itrail[axis] += step[axis];
		ilead[axis] += step[axis];
		collision_axis = axis;
		// End of stepping.
	}
	
	if (!collision) {
		collision_axis = -1;
		t = 0.0f;
	}
	
	return t;
}

// http://www.cse.chalmers.se/edu/year/2010/course/TDA361/grid.pdf
// https://github.com/andyhall/fast-voxel-raycast/blob/master/index.js
float WorldLoader::fastRay(glm::vec3 origin, glm::vec3 direction, float maxt, glm::vec3& penetration, int& collision_axis, glm::ivec3& hit) {
	penetration = glm::vec3(0.0f);
	hit = glm::ivec3(0);
	glm::vec3 normalDirection = glm::normalize(direction);
	
	glm::ivec3 ipos {
		floor(origin.x),
		floor(origin.y),
		floor(origin.z)
	};
	
	int step[3]; // In which direction we are moving in each axis.
	for (int i = 0; i < 3; i++) {
		step[i] = direction[i] > 0 ? 1 : -1;//direction[i] < 0 ? -1 : 0;
	}
	
	float scaling[3]; // How far along the ray we have to move to get to the next voxel.
	scaling[0] = sqrt( 1 + pow(normalDirection.z / normalDirection.x, 2));
	scaling[2] = sqrt( 1 + pow(normalDirection.x / normalDirection.z, 2));
	float xz = sqrt(normalDirection.x * normalDirection.x + normalDirection.z * normalDirection.z);
	scaling[1] = sqrt( 1 + pow(xz / normalDirection.y, 2));
	
	float tMax[3]; // Value of t where ray crosses the first voxel boundary in each axis.
	for (int i = 0; i < 3; i++) {
		if (abs(normalDirection[i]) < 0.001f) {
			tMax[i] = INFINITY;
			continue;
		}
		
		tMax[i] = step[i] >= 0 ? (float) ipos[i] + 1 - origin[i] : origin[i] - (float) ipos[i];
		tMax[i] *= scaling[i];
	}
	
	float t = 0.0f;
	bool collision = false;
	while( t < maxt) {
		// check voxel
		
		auto [block, found] = getBlock(ipos);
		if (found && getBlockType(block) != 0) {
			penetration.x = origin.x + normalDirection.x * t;
			penetration.y = origin.y + normalDirection.y * t;
			penetration.z = origin.z + normalDirection.z * t;
			
			collision = true;
			if (collision_axis == -1) {
				collision_axis = tMax[0] < tMax[1] ?
									tMax[0] < tMax[2] ? 0 : 2 :
									tMax[1] < tMax[2] ? 1 : 2;
			}
			hit[collision_axis] = step[collision_axis];
			return t;
		}
		
		if (tMax[0] < tMax[1]) {
			if (tMax[0] < tMax[2]) { 
				t = tMax[0];
				tMax[0] += scaling[0];
				ipos[0] += step[0];
				collision_axis = 0;
			} else {
				t = tMax[2];
				tMax[2] += scaling[2];
				ipos[2] += step[2];
				collision_axis = 2;
			}
		} else {
			if (tMax[1] < tMax[2]) {
				t = tMax[1];
				tMax[1] += scaling[1];
				ipos[1] += step[1];
				collision_axis = 1;
			} else {
				t = tMax[2];
				tMax[2] += scaling[2];
				ipos[2] += step[2];
				collision_axis = 2;
			}
		}
	}
	
	if (!collision) {
		collision_axis = -1;
		t = 0.0f;
	}
	
	return t;
}

// WorldLoader::~WorldLoader() {}