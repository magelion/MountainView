#version 330

uniform sampler2D texTerrain;

in vec2 coord;

out vec4 outBuffer;

void main() {

  outBuffer = texture(texTerrain,coord);
}
