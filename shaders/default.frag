#version 330

uniform sampler2D texTerrain;

in vec2 coord;

out vec4 outbuffer;

void main() {

  outbuffer = texture(texTerrain,coord);
}
