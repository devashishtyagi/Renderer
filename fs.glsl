#version 420

uniform sampler2D basic_texture;

in vec3 colour, position_eye, normal_eye;
in vec2 texture_coordinates;
out vec4 frag_colour;
uniform mat4 view, proj, model, orig_model_mat;

// virtual lighting source
vec3 light_position_world = vec3(0.0, 10.0, 30.0);
vec3 Ls = vec3(1.0, 0.0, 0.0);
vec3 Ld = vec3(0.7, 0.7, 0.7);
vec3 La = vec3(0.2, 0.2, 0.2);

// surface reflectance
vec3 Ks = vec3(1.0, 1.0, 1.0);
vec3 Kd = colour;
vec3 Ka = colour;
float specular_exponent = 100.0;


void main () {
    vec4 texel = texture2D(basic_texture, texture_coordinates);
    
    // ambient intensity
    vec3 la = La*Ka;

    // diffuse lighting
    vec3 light_position_eye = vec3(view * orig_model_mat * vec4(light_position_world, 1.0));
    vec3 distance_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize(distance_to_light_eye);
    vec3 norm_normal_eye = normalize(normal_eye);
    float dot_prod = max(dot(direction_to_light_eye, norm_normal_eye), 0.0);
    vec3 ld = Ld*Kd*dot_prod; // final diffuse intensity

    //specualr intensity
    vec3 reflection_eye = reflect(-direction_to_light_eye, normal_eye);
    reflection_eye = normalize(reflection_eye);
    vec3 surface_to_viewer_eye = normalize(-position_eye);
    float dot_prod_specular = max(dot(reflection_eye, surface_to_viewer_eye), 0.0);
    float specular_factor = pow(dot_prod_specular, specular_exponent);
    vec3 ls = Ls*Ks*specular_factor;

	
    frag_colour = vec4(la + ls + ld, 1.0);
}
