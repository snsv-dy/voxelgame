#version 300 es

in vec4 pos;

out vec2 texCoord;
uniform mat4 projection;

void main(){
	texCoord = vec2(pos.zw);
	
	gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
	//gl_Position = projection * pos.xy;
}