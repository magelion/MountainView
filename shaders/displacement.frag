#version 330

uniform sampler2D hmap;
uniform vec3 light;
out vec4 bufferColor;

in vec3 camView;
in vec4 fragmentColor;
in vec3 normal;
in vec3 lightDir;
in vec3 pos;


void main() {

    bufferColor = fragmentColor;
}
