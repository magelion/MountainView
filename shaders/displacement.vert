#version 330

layout(location = 0) in vec3 position;

uniform sampler2D hmap;
uniform mat4 mdvMat;
uniform mat4 projMat;
uniform mat3 normalMat;
uniform vec3 light;

out vec3 normalView;
out vec3 camView;
out vec3 tangent;

out vec3 normal;
out vec3 lightDir;
out vec3 pos;
out vec2 coord;

out vec4 fragmentColor;

void main() {

    pos = position;
    coord = position.xy*0.5+0.5;

    //deplacement sur z
    pos.z = texture(hmap,coord).x;
    gl_Position = projMat*mdvMat*vec4(pos,1.0);

    fragmentColor = vec4(dot(vec3(1,0,0),light));

}
