#version 300 es

precision mediump float;
in vec2 texCoord;
out vec4 FragColor;
uniform sampler2D hudTexture;

// uniform float textureWidth;
// uniform float textureHeight;

void main() {
	FragColor = vec4(1.0, 1.0, 1.0, 1.0 - texture(hudTexture, texCoord).r);
	// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}