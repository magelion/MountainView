#version 330

uniform sampler2D hmap;
uniform sampler2D normmap;
uniform vec3 light;
out vec4 bufferColor;

in vec3 camView;
in vec4 fragmentColor;
in vec3 normal;
in vec3 lightDir;
in vec3 pos;


void main() {

    vec4 ambientColor = vec4(0.0,0.3,0.0,1.0);
    int TMP_factor = 150;

    if(pos.z<0.20) { TMP_factor = 10; ambientColor = vec4(0.0,0.3,0.5,1.0); }

    vec3 refletDir = normalize(reflect(-lightDir, normal));
    vec4 frag = ambientColor + vec4(0.5,0.5,0.5,1.0)*max(0.0, dot(lightDir, normal));  
    frag = frag+vec4(1.0,1.0,0.0,1.0)*pow(max(0.0, dot(refletDir, camView)), TMP_factor);
    frag.a = 1.0;

    bufferColor = frag + fragmentColor*0.4;
}
