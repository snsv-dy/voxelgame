#include "WorldLoader.h"

WorldLoader::WorldLoader(glm::mat4 *projection, glm::mat4 *view, unsigned int textures, struct shaderParams params){
	last_camera_pos = glm::ivec3(0);
	
	this->projection = projection;
	this->view = view;
	this->textures = textures;
	
	this->shParams = params;
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
		
		last_block = provider.valueAt(int_pos.x, int_pos.y, int_pos.z);
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
		
		chunks[block_pos.chunk].changeBlock(block_type, in_chunk_pos, action);
//		Region& containing_region = provider.getRegion(pos);
//		containing_region.modified = true;
		// Bug is when loading/meshing/most likely calculating sunlight for new chunk with blocklight
		// fixed?
		blocks_changed.push_back({block_pos, action == BlockAction::PLACE, block_type, block_before});
		
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
	if (last_camera_pos != chpos || first) {
		light_needed.clear();
		provider.update(chpos);
		
		glm::ivec3 change = chpos - last_camera_pos;
		glm::ivec3 abs_change = glm::abs(change);
		
//		if(abs_change.x > 1 || abs_change.y > 1 || abs_change.z > 1 || first) {
		if (first) {
			abs_change = glm::ivec3(radius * 2, 0, 0); // This refreshed whole observable terrain.
		}else{
			abs_change = glm::ivec3(0);
		}
		
		
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
			
		}
		
		// Removing chunks
		if(false){
			for(auto i = chunks.begin(), nexti = i; i != chunks.end(); i = nexti){
				nexti++;
				if(ivec3len2(i->first - chpos) > unloadRadiusSquared){
					glm::ivec3 pos = i->first;
					chunks.erase(i);
				}
			}
			
		}
		
		last_camera_pos = chpos;
		
		first = false;
	}
}

void WorldLoader::updateGeometry() {
	if(!chunks_to_update.empty()){
		for(const glm::ivec3& c : chunks_to_update){
			if(auto it = chunks.find(c); it != chunks.end())
				it->second.update_data();
		}
		chunks_to_update.clear();
	}
	light_needed.clear();
}

std::list<ChangedBlock> WorldLoader::getChangedBlocks() {
	auto ret = blocks_changed;
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
	auto [chunk_data, data_offset] = provider.getChunkData(pos);
	if(chunk_data != nullptr) {
		chunks[pos] = Chunk(shParams.model, pos, this, chunk_data, data_offset);
		if(light_needed != nullptr) {
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

std::pair<bool, glm::vec3> WorldLoader::playerIntersects(Player& player) {
	
	int oldTouching[3];
	for(int i = 0; i < 3; i++) {
		oldTouching[i] = player.touching[i];
		player.touching[i] = 0;
	}
	
	int movingDirection[3];
	for(int ax = 0; ax < 3; ax++) {
		movingDirection[ax] = 0;
		if (player.velocity[ax] > 0) {
			movingDirection[ax] = 1;
		} else if (player.velocity[ax] < 0) {
			movingDirection[ax] = -1;
		}
		
		// If player attempts to move in direction which is already touching the ground, set velocity in this axis to 0.
//		if (player.touching[ax] == movingDirection[ax]) {
//			player.velocity[ax] = 0.0f;
//		}else if(player.touching[ax] == -movingDirection[ax]) { 
//			player.touching[ax] = 0; // resetting touching if player moves in opposite direction.
//		}
	}
	
	glm::vec3 pointsToCheck[2] {
//		player.getPosition(),// + player.headPosition - glm::vec3(0.1f)
//		player.getPosition() + player.headPosition
		player.aabb.min,
		player.aabb.max,
	};
	
	for(int i = 0; i < 2; i++) {
		auto point = pointsToCheck[i];
		for(int ax = 0; ax < 3; ax++) {
			if (movingDirection[ax] == 0 || player.touching[ax] != 0)
				continue;
			
			glm::vec3 movedInAxis(0.0f);
			movedInAxis[ax] = player.velocity[ax];
			auto [chunkPosition, blockPosition] = toChunkCoordsReal(point + movedInAxis, TerrainConfig::ChunkSize);
			block_position blockPos = block_position(blockPosition, chunkPosition);
			auto [block, found] = getBlock(blockPos);
			
			float globalBlockPosition = blockPosition[ax] + chunkPosition[ax] * TerrainConfig::ChunkSize;
			
			if(found && getBlockType(block) != 0) {
				// calculate maximum allowed velocity
//				player.velocity[ax] = 0.0f;
				float& pos = point[ax];
//				float& vel = player.velocity[ax];
				float maxAllowed = 0.0f;
				
				if( movingDirection[ax] == 1) {
					maxAllowed = globalBlockPosition - pos;
				} else {
					maxAllowed = pos - (globalBlockPosition + 1);
				}
				
				
				if (oldTouching[ax] == 0) {
					player.velocity[ax] = (maxAllowed - 0.01) * (float) movingDirection[ax];	
				} else {
					player.velocity[ax] = 0;
				}
				
				// fordebug
				glm::ivec3 debPos = blockPosition + chunkPosition * TerrainConfig::ChunkSize;
				printf("ax touched: %d, direction: %d, allowed: %2.2f, position: [%d %d %d] , playerPosition: [%2.2f %2.2f %2.2f], ", ax, movingDirection[ax], maxAllowed, debPos.x, debPos.y, debPos.z, point.x, point.y, point.z);
				printf("player velocity [%2.2f, %2.2f, %2.2f]\n", player.velocity.x, player.velocity.y, player.velocity.z);
				// endfor
				
				// set that player is touching in this axis in this direction
				player.touching[ax] = movingDirection[ax];
			}
		}
	}
	
//	auto [chunkPosition, blockPosition] = toChunkCoordsReal(player.position + player.velocity, TerrainConfig::ChunkSize);
//	block_position blockPos = block_position(blockPosition, chunkPosition);
//	auto [block, found] = getBlock(blockPos);
//	
//	if (found && block_type(block) != 0) {
////		glm::vec3 d = glm::vec3(blockPos.block + blockPos.chunk * TerrainConfig::ChunkSize) - player.position;
//		glm::vec3 d = player.position - glm::vec3(blockPos.block + blockPos.chunk * TerrainConfig::ChunkSize);
////		if(d.x < -0.5f)
////			d.x += 1.0f;
////		if(d.y < -0.5f)
////			d.y += 1.0f;
////		if(d.z < -0.5f)
////			d.z += 1.0f;
//		
//		int max_ax = 0;
//		int max_ax_val = 0;
//		bool allEqual = true;
//		for(int ax = 0; ax < 3; ax++) { // Kolega jest kanjtem i wali z axa, a ja jestem sorcem i walę z różdżki.
//			if(abs(d[ax]) > max_ax_val) {
//				max_ax = ax;
//				max_ax_val = abs(d[ax]);
//				allEqual = false;
////				player.velocity[ax] = 0.0f;
////				player.velocity[ax] = d[ax];
//			}
//		}
//		
//		if (!allEqual) {
//			player.velocity[max_ax] = 0.0f;
//		}
//		
//		
//		
//		return {true, player.velocity + d};
//	}
//	
//	glm::vec3 normVelocity = player.velocity;
//	float velocitySize = glm::length(normVelocity);
//	
//	if(velocitySize == 0.0f){
//		return {false, {0.0f, 0.0f, 0.0f}};
//	}
//	
//	for(int i = 0; i < 3; i++) {
//		normVelocity[i] /= velocitySize;
//	}
//	
////	printf("velocity: %2.2f, %2.2f, %2.2f\n", normVelocity.x, normVelocity.y, normVelocity.z);
//	
//	float test = sweep(player.aabb, normVelocity);
//	printf("test: %f\n", test);
//	return {true, glm::vec3(test)};
	return {true, glm::vec3(0.0f)};
}

// http://www.cse.chalmers.se/edu/year/2010/course/TDA361/grid.pdf
float WorldLoader::fastRay(glm::vec3 origin, glm::vec3 direction, glm::vec3& penetration) {
	penetration = glm::vec3(0.0f);
	glm::vec3 normalDirection = glm::normalize(direction);
	
	auto [chunkPosition, blockPosition] = toChunkCoordsReal(origin, TerrainConfig::ChunkSize);
	block_position blockPos = block_position(blockPosition, chunkPosition);
	glm::ivec3 globalBlockPosition = blockPosition + chunkPosition * TerrainConfig::ChunkSize;
	
	int step[3];
	for (int i = 0; i < 3; i++) {
		step[i] = direction[i] > 0 ? 1 : direction[i] < 0 ? -1 : 0;
	}
	
	float tDelta[3];
	for (int i = 0; i < 3; i++) {
		tDelta[i] = 1.0f / normalDirection[i];
	}
	
	float tMax[3];
	for (int i = 0; i < 3; i++) {
		tMax[i] = step[i] > 0 ? globalBlockPosition[i] + 1 - origin[i] : origin[i] - globalBlockPosition[i];
	}
	
	float maxt = 2.0f;
	float t = 0.0f;
	while( t <= maxt) {
		
	}
	
//	
//	int gridPos[3] {};
	
//	auto [block, found] = getBlock(blockPos);
	
	
}

// Basically stolen from here.
// https://github.com/andyhall/voxel-aabb-sweep
float WorldLoader::sweep(AABB box, glm::vec3 vec) {
	glm::vec3 max = box.max;
	glm::vec3 min = box.min;
	
	float max_t = glm::length(vec);
	
	if(max_t == 0) {
		return 0;
	}
	
	int step[3];
	float trailing[3];
	
	int leading_i[3];
	int trailing_i[3];
	
	float normed[3];
	float tDelta[3];
	
	float tNext[3];
	
	// init step
	
	float t = 0.0f;
	float cumulative_t = 0.0f;
	for (int i = 0; i < 3; i++) {
		int dir = vec[i] >= 0;
		step[i] = dir ? 1 : -1;
		
		int lead = dir ? max[i] : min[i];
		trailing[i] = dir ? min[i] : max[i];
		
		leading_i[i] = leadEdgeToInt(lead, step[i]);
		trailing_i[i] = trailEdgeToInt(lead, step[i]);
		
		normed[i] = vec[i] / max_t;
		tDelta[i] = abs(1 / normed[i]);
		
		int dist = dir ? (leading_i[i] + 1 - lead) : (lead - leading_i[i]);
		tNext[i] = (tDelta[i] < INFINITY) ? tDelta[i] * dist : INFINITY;
	}
	
	// init end
	
	int axis = stepForward(tNext, t, leading_i, trailing, trailing_i, step, tDelta, normed);
	while( t <= max_t ) {
		if (checkCollision(axis, leading_i, trailing_i, step)) {
//			printf("is collision\n");
			return cumulative_t;
		}
		
		axis = stepForward(tNext, t, leading_i, trailing, trailing_i, step, tDelta, normed);
	}
	
	cumulative_t += max_t;
	for (int i = 0; i < 3; i++) {
		min[i] += vec[i];
		max[i] += vec[i];
	}
	
//	printf("isnt collision\n");
	return cumulative_t;
}

bool WorldLoader::checkCollision(int i_axis, int leading_i[3], int trailing_i[3], int step[3]) {
	int stepx = step[0];
	int x0 = (i_axis == 0) ? leading_i[0] : trailing_i[0];
	int x1 = leading_i[0] + stepx;
	
	int stepy = step[1];
	int y0 = (i_axis == 1) ? leading_i[1] : trailing_i[1];
	int y1 = leading_i[1] + stepy;
	
	int stepz = step[2];
	int z0 = (i_axis == 2) ? leading_i[2] : trailing_i[2];
	int z1 = leading_i[2] + stepz;
	
	for( int x = x0; x != x1; x += stepx) {
		for( int y = y0; y != y1; y += stepy) {
			for( int z = z0; z != z1; z += stepz) {
				auto [chunkPosition, blockPosition] = toChunkCoordsReal(glm::vec3(x, y, z), TerrainConfig::ChunkSize);
				block_position blockPos = block_position(blockPosition, chunkPosition);
				
				if ( auto [block, found] = getBlock(blockPos); found && getBlockType(block) != 0) {
					return true;
				}
			}
		}
	}
	
	return false;
}

int stepForward(float tNext[3], float& t, int leading_i[3], float trailing[3], int trailing_i[3], int step[3], float tDelta[3], float normed[3]) {
	int axis = (tNext[0] < tNext[1]) ?
		((tNext[0] < tNext[2]) ? 0 : 2) :
		((tNext[1] < tNext[2]) ? 1 : 2);
	float dt = tNext[axis] - t;
	t = tNext[axis];
	leading_i[axis] += step[axis];
	tNext[axis] += tDelta[axis];
	for (int i = 0; i < 3; i++) {
		trailing[i] += dt * normed[i];
		trailing_i[i] = trailEdgeToInt(trailing[i], step[i]);
	}
	
	return axis;
}

float epsilon = 1e-10;
int leadEdgeToInt(int coord, int step) {
	return floor(-epsilon * step + coord);
}

int trailEdgeToInt(int coord, int step) {
	return floor(epsilon * step + coord);
}