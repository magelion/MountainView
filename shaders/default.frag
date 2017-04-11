#version 330

uniform sampler2D heightmap;

in vec2 coord;

out vec4 outbuffer;

void main() {

  outbuffer = texture(heightmap,coord);
}
