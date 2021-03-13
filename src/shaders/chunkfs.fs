#version 330 core

out vec4 FragColor;

in vec3 texCoord;
in vec3 vCoord;
uniform sampler2D blockTexture;

flat in int block_param;
in float ao_test;
in vec3 ao_color_debug;

void main(){
	
	float texBorder = 0.01;
	const float ntexes = 10;
	const float tex1size = 1.0 / ntexes;
	float tex1sizeb = tex1size - texBorder*2;
	
	vec2 texCoordClip = texCoord.xy;
	texCoordClip = mod(texCoordClip, 1.0f);
	texCoordClip *= tex1sizeb;
	
	texCoordClip += texBorder;
	
	//int texId = int(texCoord.z);
	int texId = block_param;
	//int top = (texId & 0x100);
	//float textureId = 1.0f;//float(block_param & 0xff);
	float textureId = float(texId & 0xff);
	
	//float ao = float((texId >> 9) & 3) / 6.0f;
	
	if((texId & 0x100) == 0){
		texCoordClip.x += tex1size;
	}
	texCoordClip.y += (textureId - 1.0f)*tex1size;
	
	float y = float(block_param >> 20) / 15.0f;
	float x = float((block_param & 0xf0000) >> 16) / 15.0f;
	
	int global_light = (block_param >> 24);// & 0xf;
	float light_intensity = float(15 - global_light) / 20.0f + 0.25f;
	
	FragColor = texture(blockTexture, texCoordClip) * vec4(vec3(ao_test), 1.0f) * vec4(vec3(light_intensity), 1.0f);
	//FragColor = texture(blockTexture, texCoordClip) * vec4(vec3(ao_color_debug), 1.0f);
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