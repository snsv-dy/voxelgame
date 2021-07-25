#version 300 es

precision mediump float;

in vec3 texCoord;
in vec3 vCoord;
uniform sampler2D blockTexture;

flat in int block_param;
in float ao_test;
in vec3 ao_color_debug;
out vec4 FragColor;

//vec3 blocklight_color = vec3(1.0f, 1.0f, 1.0f);
vec3 blocklight_color = vec3(1.0, 1.0, 0.6);

void main() {
	// FragColor = vec4(1.0, 0.5, 0.3, 1.0);
	float texBorder = 0.01;
	const float ntexes = 10.0;
	const float tex1size = 1.0 / ntexes;
	float tex1sizeb = tex1size - texBorder * 2.0;
	
	vec2 texCoordClip = texCoord.xy;
	texCoordClip = mod(texCoordClip, 1.0);
	texCoordClip *= tex1sizeb;
	
	texCoordClip += texBorder;
	
	int texId = block_param;
	float textureId = float(texId & 0xff);
	
	if((texId & 0x100) == 0){
		texCoordClip.x += tex1size;
	}
	texCoordClip.y += (textureId - 1.0) * tex1size;
	
	float y = float(block_param >> 20) / 15.0;
	float x = float((block_param & 0xf0000) >> 16) / 15.0;
	
	int sun_value = (block_param >> 24) & 0xf;
	float sun_intensity = float(sun_value) / 20.0 + 0.25;
	vec3 sun_light = vec3(sun_intensity);
	
	int block_value = ((block_param >> 28) & 0xf);
	float block_intensity = float(block_value) / 20.0 + 0.25;
	vec3 block_light = blocklight_color * block_intensity;
	
	vec3 final_light = max(block_light, sun_light);
	FragColor = texture(blockTexture, texCoordClip) * vec4(vec3(ao_test), 1.0f) * vec4(final_light, 1.0f);
}

/*
#version 330 core

out vec4 FragColor;

in vec3 texCoord;
in vec3 vCoord;
uniform sampler2D blockTexture;

void main(){
	float texBorder = 0.01;
	const float ntexes = 10;
	const float tex1size = 1.0 / ntexes;
	//const float tex1size = 0.1;
	float tex1sizeb = tex1size - texBorder*2;
	int texId = int(texCoord.z);
	int top = texId & 0x100;
	float textureId = float(texId & 0xff);
	
	float ao = float((texId >> 9) & 3) / 6.0f;
	
	vec2 texCoordClip = texCoord.xy;
	texCoordClip = mod(texCoordClip, 1.0f);
	texCoordClip *= tex1sizeb;
	
	
	//texCoordClip.x *= texCoord.x;
	//texCoordClip.y *= texCoord.y;
	
	//texCoordClip.y /= ntexes;
	//texCoordClip.x /= 2.0f;
	texCoordClip += texBorder;
	if(top == 0){
		texCoordClip.x += tex1size;
	}
	
	//texCoordClip = mod(texCoordClip, tex1sizeb) + texBorder;
	
	//texCoordClip.y += (textureId + 1.0f)*tex1size;
	texCoordClip.y += (textureId - 1.0f)*tex1size;
	//texCoordClip += texBorder;
	

}*/