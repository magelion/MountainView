#version 330

layout(location = 0) in vec3 position;

uniform Sampler2D hnmap;
out vec4 normal;

void main() {
  vec4 n = texture(hnmap, position.xy*0.5+0.5);
  gl_Position = projMat*mdvMat*vec4(position + d.x*vec3(0,0,1),1.0);
  normal = n;
}
