#version 300 es

precision mediump float;
in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D fontTexture;

uniform float textureWidth;
uniform float textureHeight;

void main() {
	FragColor = vec4(1.0, 1.0, 1.0, texture(fontTexture, texCoord).r);
	// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}