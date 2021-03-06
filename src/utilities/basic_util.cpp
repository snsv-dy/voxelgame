#include "basic_util.h"

// TODO Change from c style file reading to c++ style file reading.
unsigned int getShaderFromFile(const std::string& filename, const ShaderType& type ){
	unsigned int shader;
	
	
	FILE *f = fopen(filename.c_str(), "r");
	if(f == NULL){
		
		printf("no file\n");
		return 0;
	}
	
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	char *buffer = (char *)calloc((fsize + 1), sizeof(char));
	if(buffer == NULL){
		printf("no buffer\n");
		fclose(f);
		return 0;
	}
	
	int bytes_read = fread(buffer, sizeof(char), fsize, f);
//	char *res = fgets(buffer, fsize + 1, f);
//	printf("shader: %s\n", buffer);
//	printf("fsize: %d\n", fsize);
	if(bytes_read == 0){
		printf("no bytes\n");
		fclose(f);
		free(buffer);
		return 0;
	}
	
	if(type == ShaderType::VERTEX){
		shader = glCreateShader(GL_VERTEX_SHADER);
	}else{
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}
	
	glShaderSource(shader, 1, &buffer, NULL);
//	glShaderSource()
	glCompileShader(shader);
	
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success){
		printf("Some shader failed: %s\n", filename);
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		printf("%s\n", infoLog);
	}
	
	fclose(f);
	free(buffer);
	
	return shader;
}

shaderParams getShaderParams(unsigned int program){
//shaderParams getShaderParams(char *vsSource, char *fsSource){
	if(program == 0)
		return {0};
		
//	unsigned int vertex_shader = getShaderFromFile(vsSource, shaderType::VERTEX);
//	if(vertex_shader == 0){
//		printf("No vs\n");
//	}
//	
//	unsigned int fragment_shader = getShaderFromFile(fsSource, shaderType::FRAGMENT);
//	if(this->fragment_shader == 0){
//		printf("No fs\n");
//	}
	
	shaderParams params;
	
	params.program = program;//getShaderProgram( this->vertex_shader, this->fragment_shader);
	
	params.projection = glGetUniformLocation(program, "projection");
	params.view = glGetUniformLocation(program, "view");
	params.model = glGetUniformLocation(program, "model");
//	this->locations.select = glGetUniformLocation(program, "selected");
//	this->locations.side = glGetUniformLocation(program, "selected_plane");
	
	params.light = glGetUniformLocation(program, "light_position");	
//	int light_pos_loc = glGetUniformLocation(program, "light_position");
	return params;
}

int getShaderProgram(int vs, int fs){
//	if(vs == 0 || fs == 0)
	
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	
	glLinkProgram(shaderProgram);
	
	// CHECK IF PROGRAM HAS LINKED
	char error_msg[512];
	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(shaderProgram, 512, NULL, error_msg);
		printf("prorgam log:\n%s\n", error_msg);
		shaderProgram = 0;
		printf("succ = %d\n", success);
	}
	
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	
	return shaderProgram;
}

//chunkCoords toChunkCoords(glm::ivec3 cord, int chunkSize){
//	glm::ivec3 chpos;
//	glm::ivec3 dpos;
//	if(cord.x >= 0){
//		chpos.x = cord.x / chunkSize;
//		dpos.x = cord.x % chunkSize;
//	}else{
//		int mx = (-cord.x - 1);
//		chpos.x = -(mx / chunkSize + 1);
//		dpos.x = -(mx % chunkSize) + chunkSize - 1;
//	}
//	
//	if(cord.y >= 0){
//		chpos.y = cord.y / chunkSize;
//		dpos.y = cord.y % chunkSize;
//	}else{
//		int my = (-cord.y - 1);
//		chpos.y = -(my / chunkSize + 1);
//		dpos.y = -(my % chunkSize) + chunkSize - 1;
//	}
//	
//	if(cord.z >= 0){
//		chpos.z = cord.z / chunkSize;
//		dpos.z = cord.z % chunkSize;
//	}else{
//		int mz = (-cord.z - 1);
//		chpos.z = -(mz / chunkSize + 1);
//		dpos.z = -(mz % chunkSize) + chunkSize - 1;
//	}
//	
//	return {chpos, dpos};
//}

std::tuple<glm::ivec3, glm::ivec3> toChunkCoords(const glm::ivec3& position, const int& chunkSize){
	glm::ivec3 chunk_index;
	glm::ivec3 index_in_chunk;
	
	int temp_chunk_i, temp_i_in_chunk;
	std::tie(temp_chunk_i, temp_i_in_chunk) = toNonSymetricSpace(position.x, chunkSize);
	chunk_index.x = temp_chunk_i;
	index_in_chunk.x = temp_i_in_chunk;
	
	std::tie(temp_chunk_i, temp_i_in_chunk) = toNonSymetricSpace(position.y, chunkSize);
	chunk_index.y = temp_chunk_i;
	index_in_chunk.y = temp_i_in_chunk;
	
	std::tie(temp_chunk_i, temp_i_in_chunk) = toNonSymetricSpace(position.z, chunkSize);
	chunk_index.z = temp_chunk_i;
	index_in_chunk.z = temp_i_in_chunk;
	
	return std::make_tuple(chunk_index, index_in_chunk);
}

inline std::tuple<int, int> toNonSymetricSpace(const int& value, const int& chunkSize){
	int chunk_index;
	int index_in_chunk;
	
	if(value >= 0){
		chunk_index = value / chunkSize;
		index_in_chunk = value % chunkSize;
	}else{
		int mx = (-value - 1);
		chunk_index = -(mx / chunkSize + 1);
		index_in_chunk = -(mx % chunkSize) + chunkSize - 1;
	}
	
	return std::make_tuple(chunk_index, index_in_chunk);
}

bool compareVec3::operator()(const glm::ivec3 &a, const glm::ivec3 &b) const{
	if(a.z != b.z) return a.z < b.z;
	if(a.y != b.y) return a.y < b.y;
	return a.x < b.x;
}
