#version 330

uniform sampler2D hmap;
out vec4 bufferColor;

in vec2 coord;
in vec3 pos;
in vec4 fragmentColor;

void main() {
  bufferColor = fragmentColor;
}
