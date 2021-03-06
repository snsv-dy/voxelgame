#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTex;
layout (location = 2) in int intval;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

//out vec3 Normal;
//out vec3 fragPos;

out vec3 texCoord;
out vec3 vCoord;

flat out int block_param;
out float ao_test;

#define chunkSize 16
#define chunkSizeZ 256
#define chunkSizeHalf 8

vec3 colors[4];
out vec3 ao_color_debug;

void main()
{
	colors[0] = vec3(1, 0, 0);
	colors[1] = vec3(0, 1, 0);
	colors[2] = vec3(0, 0, 1);
	colors[3] = vec3(1, 1, 0);
	
	block_param = intval;
	//ao_test = min(float((intval >> 9) & 3) / 4.0f, 1.0f);
	// CORRECT ONE 
	ao_test = (float( (intval >> 9) & 3) / 3.0f) * (2.0f / 3.0f) + 0.3f;
	
	int ao_color = intval >> 9;
	ao_color &= 3;
	
	ao_color_debug = colors[ao_color];
	
	texCoord = aTex;
	//float z = int(aPos.x / chunkSizeZ);
	//float y = (int(aPos.x) % chunkSizeZ) / chunkSize;
	//float x = int(aPos.x) % chunkSize;
	//vec3 trans = vec3(x - chunkSizeHalf, y - chunkSizeHalf, z + chunkSizeHalf);
	//int i = int(aPos.y) * 3;
	//vec3 vs = vec3(cubebase[i], cubebase[i + 1], cubebase[i + 2]);// + trans;
	
	//Normal = aNorm;
	//Normal = mat3(transpose(inverse(model))) * aNorm;
	//fragPos = vec3(model * vec4(aPos, 1.0));
	
	vCoord = vec3(model * vec4(aPos.xyz, 1.0));
	
	gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
	//gl_Position = vec4(vs, 1.0);
}