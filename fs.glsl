#version 410

in vec3 colourtes;
in vec3 tex_normalfs;
//in float weathering_degree_fs;
//in vec4 weathering_degree_neighbour_fs;
in vec2 TexCoord_FS_in;
in vec3 position_eye, normal_eye;
in mat3 TBN;

uniform sampler2D gColorMap, gNormalMap, gWeatheringMap;            
uniform mat4 view, model, proj;
uniform float weatheringMapH;
uniform float weatheringMapW;


out vec4 frag_colour; 

// fixed point light properties
vec3 light_position_world = vec3 (0.0, 0.0, 4.0);
vec3 Ls = vec3 (1.0, 1.0, 1.0); // white specular colour
vec3 Ld = vec3 (0.7, 0.7, 0.7); // dull white diffuse light colour
vec3 La = vec3 (0.2, 0.2, 0.2); // grey ambient colour

// surface reflectance
vec3 Ks = vec3 (1.0, 1.0, 1.0); // fully reflect specular light
vec3 Kd = vec3 (1.0, 0.5, 0.0); // orange diffuse surface reflectance
vec3 Ka = vec3 (1.0, 1.0, 1.0); // fully reflect ambient light
float specular_exponent = 10.0; // specular 'power'
float ambience_factor = 0.9;

/*
vec2 calculateUV(float corrosion, float h, float w) {
    float small = 5.0;
    const float EPS = 0.00001;
    vec2 r = vec2(0.0,0.0);
    w = 50;
    h = 50;
    float dx = 1.0/w, dy = 1.0/h;

    float x = weathering_degree_neighbour_fs.x;
    float y = weathering_degree_neighbour_fs.y;
    float z = weathering_degree_neighbour_fs.z;
    float v = weathering_degree_neighbour_fs.w;
    
    for(float i = 0; i <= 1; i += dx){
        for(float j = 0; j <= 1; j += dy){
            float p = i;
            float q = j;
            vec2 sam = vec2(p,q);
            float a = texture(gWeatheringMap,sam).r;

            sam.x = max(p-dx, 0.0);
            float b = texture(gWeatheringMap, sam).r;

            sam.x = min(p+dx, 1.0);
            float c = texture(gWeatheringMap, sam).r;

            sam.y = max(q-dy, 0.0);
            float d = texture(gWeatheringMap, sam).r;

            sam.y = min(q+dy, 1.0);
            float e = texture(gWeatheringMap, sam).r;

            float l2Norm = pow(a-corrosion, 2); //+ pow(b - x, 2) + pow(c - y, 2) + pow(d - z, 2) + pow(e - v, 2); 
            
            if(l2Norm < small){
                small = l2Norm;
                r.x = p;
                r.y = q;
            } 
        }
    }
    return r;
}
*/


void main () {
    
    //vec2 UV = calculateUV(weathering_degree_fs,weatheringMapH, weatheringMapW);

    // IF READING FROM A MAP
    // vec4 tex_colour = texture(gColorMap, TexCoord_FS_in.xy);
    // vec3 normal = normalize(texture(gNormalMap, TexCoord_FS_in.xy).rgb*2.0 - 1.0);
    //vec4 tex_colour = texture(gColorMap, UV);
    //vec4 tex_colour = vec4(1.0,1.0,1.0,1.0);
    //vec3 normal = normalize(texture(gNormalMap, UV).rgb*2.0 - 1.0);

    // ELSE
    vec4 tex_colour = vec4(colourtes, 1.0);
    vec3 normal = tex_normalfs;
    normal.r = normal.r/255.0;
    normal.g = normal.g/255.0;
    normal.b = normal.b/255.0;
    normal = normal.rgb*2.0 - 1.0;
    normal = normalize(normal);
    
    // ambient intensity
    vec3 Ia = ambience_factor*La*Ka;

    // diffuse intensity

    Kd = vec3(tex_colour);
    vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));
    vec3 distance_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize (distance_to_light_eye);
    
    // Comment if the normal is not in tangent space
    direction_to_light_eye = TBN * direction_to_light_eye;  
    
    // replace normal with normal_eye if you dont want to do normal mapping
    float dot_prod = dot (direction_to_light_eye, normal);  
    
    dot_prod = max(dot_prod, 0.0);     // needs correction
    vec3 Id = Ld * Kd * dot_prod; // final diffuse intensity
    
    // specular intensity
    vec3 reflection_eye = reflect (-direction_to_light_eye, normal);   
    vec3 surface_to_viewer_eye = normalize (-position_eye);
    surface_to_viewer_eye = TBN * surface_to_viewer_eye;                    // Convert to tangent space
    float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
    dot_prod_specular = max (dot_prod_specular, 0.0);
    float specular_factor = pow (dot_prod_specular, specular_exponent);
    vec3 Is = Ls * Ks * specular_factor; // final specular intensity

    // final colour
    //frag_colour = vec4(Id + Ia, 1.0);
    frag_colour = tex_colour;
    //frag_colour = texture(gNormalMap, TexCoord_FS_in.xy);
    //frag_colour = vec4(TexCoord_FS_in.xy, 0.0, 1.0);
    //frag_colour = vec4(colourtes, 1.0);
    //vec2 sam = vec2(0.0,0.4) + WeatheringMapD;
    //vec2 sam = WeatheringMapD;
    //vec4 al = texture(gWeatheringMap,TexCoord_FS_in.xy);
    //frag_colour = vec4(al.r,al.r,1.0 - al.r,1.0);
    //frag_colour = vec4(weathering_degree_fs, weathering_degree_fs, 1.0 - weathering_degree_fs, 1.0);
    //frag_colour = vec4(weatheringMapH, weatheringMapW,1-weatheringMapW,1.0);


    /*
    if (normal_eye.z < 0.0) {
        frag_colour = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else {
        frag_colour = vec4(1.0, 0.0, 0.0, 1.0);
    }
    */
}
