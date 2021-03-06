#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D fontTexture;

uniform float textureWidth;
uniform float textureHeight;

void main(){
	FragColor = vec4(1.0, 1.0, 1.0, texture(fontTexture, texCoord).r);
	// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}