#version 330

uniform sampler2D hmap;
uniform sampler2D nmap;
uniform vec3 light;
out vec4 bufferColor;

in vec3 camView;
in vec4 fragmentColor;
in vec3 normal;
in vec3 pos;
in vec2 coord;
in vec3 lightDir;


void main() {
    
    /*
    vec4 ambientColor = vec4(0.0,0.3,0.0,1.0);
    int TMP_factor = 150;

    if(pos.z<0.20) { TMP_factor = 10; ambientColor = vec4(0.0,0.3,0.5,1.0); }

    vec3 refletDir = normalize(reflect(-lightDir, N));
    vec4 frag = ambientColor + vec4(0.5,0.5,0.5,1.0)*max(0.0, dot(lightDir, N));  
    frag = frag+vec4(1.0,1.0,0.0,1.0)*pow(max(0.0, dot(refletDir, camView)), TMP_factor);
    frag.a = 1.0;

    bufferColor = frag + fragmentColor*0.4;
    */

    //vec3 N = normalize(texture2D(nmap, coord.st).rgb * 2.0 - 1.0);  

    vec3 light_pos = normalize(light);
    vec3 viewDir = normalize(camView - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + camView);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * vec3(1,1,0);

    float diffuse = max(dot(normal, lightDir), 0.0);  
    vec3 color = (diffuse*0.2 + specular) * vec3(0.0,0.5,0.0); 
    bufferColor = vec4(color, 1.0f);
}
