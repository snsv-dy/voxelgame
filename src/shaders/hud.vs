#version 300 es

in vec2 pos;
in vec2 tex;

out vec2 texCoord;
uniform mat4 projection;
uniform mat4 model;

void main(){
	texCoord = tex;
	
	gl_Position = projection * model * vec4(pos.x, pos.y, 0.0, 1.0);
	//gl_Position = vec4(pos, 0.0, 1.0);
}