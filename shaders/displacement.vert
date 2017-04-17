#version 330

layout(location = 0) in vec3 position;

uniform sampler2D hmap;
uniform mat4 mdvMat, projMat, normalMat;
uniform vec3 light;

out vec3 normal;
out vec3 camDir;
out vec3 lightPos;
out vec2 coord;
out vec3 pos;
out vec4 fragmentColor;

/* finite difference method */

vec3 normalGrid(){
    
    // Central Difference Scheme (stackoverflow.com/questions/13983189/opengl-how-to-calculate-normals-in-a-terrain-height-grid)

    vec4 xf = texture2D(hmap, coord.xy+vec2(1.0/512, 0.0));
    vec4 xb = texture2D(hmap, coord.xy-vec2(1.0/512, 0.0));
	vec4 yf = texture2D(hmap, coord.xy+vec2(0.0, 1.0/512));
	vec4 yb = texture2D(hmap, coord.xy-vec2(0.0, 1.0/512));

	vec3 tangX = vec3(1.0, 0.0, 2.0*(xf.x-xb.x));
	vec3 tangY = vec3(0.0, 1.0, 2.0*(yf.x-yb.x));  
    vec3 hNorm = normalize(cross(tangX, tangY));
    
    return hNorm;
}


void main() {
    pos = position;
    coord = position.xy*0.5+0.5;

    //deplacement sur z
    pos.z = texture(hmap,coord).x;
    gl_Position = projMat*mdvMat*vec4(pos,1.0);
    //gl_Position = projMat*mdvMat*vec4(position,1.0);

    vec3 N = normalGrid();

    camDir = normalize((mdvMat*vec4(pos,1.0)).xyz);
    vec3 viewDir = normalize(camDir - pos);

    //phong
    float diff = max(dot(N, light), 0.0);
    vec3 diffuse = diff * vec3(0,0,1);
    vec3 reflectDir = reflect(-light, N); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.75 * spec * vec3(1,0,0);  
    
    vec3 res = (vec3(0.2,0.2,0.2) + diffuse + specular) * vec3(0.9,0.9,0.9);
    //fragmentColor = vec4(dot(N,light));
    fragmentColor = vec4(res,1.0f);
}
