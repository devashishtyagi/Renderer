#version 410

in vec3 colourtes;
in vec2 TexCoord_FS_in;
in vec3 position_eye, normal_eye;
in mat3 TBN;

uniform sampler2D gColorMap, gNormalMap;            
uniform mat4 view, model, proj;

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

void main () {
    // vec4 tex_colour = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 tex_colour = texture(gColorMap, TexCoord_FS_in.xy);
    //vec3 normal = normal_eye;
    vec3 normal = normalize(texture(gNormalMap, TexCoord_FS_in.xy).rgb*2.0 - 1.0);
    
    // ambient intensity
    La = vec3(tex_colour);
    vec3 Ia = La*Ka;

    // diffuse intensity
    Ld = vec3(tex_colour);
    vec3 light_position_eye = vec3 (view * vec4 (light_position_world, 1.0));
    vec3 distance_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize (distance_to_light_eye);
    direction_to_light_eye = TBN * direction_to_light_eye;                   // Convert to tangent space
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
    frag_colour = vec4(Id + Ia, 1.0);
    //frag_colour = texture(gNormalMap, TexCoord_FS_in.xy);
    //frag_colour = vec4(TexCoord_FS_in.xy, 0.0, 1.0);
    /*
    if (normal_eye.z < 0.0) {
        frag_colour = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else {
        frag_colour = vec4(1.0, 0.0, 0.0, 1.0);
    }
    */
}
