#version 330

uniform sampler2D hmap;
uniform sampler2D nmap;
uniform vec3 light;
uniform bool spot;
out vec4 bufferColor;

in vec3 camView;
in vec4 fragmentColor;
in vec3 normal;
in vec3 pos;
in vec2 coord;
in vec3 lightDir;


void main() {
    
    
    if(spot) // Utilise la normale calculée dans le vertex shader
    {
        vec4 ambientC = vec4(0.0,0.3,0.0,1.0);
        float brillance = 64.0;

        if(pos.z<0.20) { brillance = 10; ambientC = vec4(0.0,0.3,0.5,1.0); }

        vec3 refletDir = normalize(reflect(-lightDir, normal));
        vec4 frag = ambientC + vec4(0.5,0.5,0.5,1.0)*max(0.0, dot(lightDir, normal));  
        frag = frag+vec4(1.0,1.0,0.0,1.0)*pow(max(0.0, dot(refletDir, camView)), brillance);
        frag.a = 1.0;

        bufferColor = frag + fragmentColor*0.4;
    }
    else // Utilise la normale lue dans la texture
    {
        vec3 norm = normalize(texture2D(nmap, coord.st).rgb); 
        norm = normalize(norm * 2.0 - 1.0); 
        
        vec4 ambientColor = vec4(0.0,0.3,0.0,1.0);
        float brillance = 150.0;
       
        vec3 color = vec3(0.0,0.5,0.0);
        vec3 ambient = color*0.2;
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 halfwayDir = normalize(lightDir + camView); 
        float spec = pow(max(dot(norm, halfwayDir), 0.0), brillance);
        vec3 diffuse = diff * vec3(0.6,0.6,0.2);
        //float spec = pow(max(0.0, dot(reflectDir, camView)), brillance);
        vec3 specular = vec3(1.0,0.0,0.0) * spec;
        
        
        bufferColor = vec4(ambient + diffuse + specular, 1.0f);
    }
    
}
