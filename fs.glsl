#version 420

in vec3 colour, position_eye, normal_eye;
out vec4 frag_colour;
uniform mat4 view, proj, model, orig_model_mat;

// virtual lighting source
vec3 light_position_world = vec3(0.0, 0.0, 7.0);
vec3 Ls = vec3(0.7, 0.7, 0.7);
vec3 Ld = vec3(1.0, 1.0, 1.0);
vec3 La = vec3(0.2, 0.2, 0.2);

// surface reflectance
vec3 Ks = vec3(1.0, 1.0, 1.0);
vec3 Kd = vec3(1.0, 1.0, 0.0);
vec3 Ka = vec3(1.0, 1.0, 1.0);
float specular_exponent = 100.0;


void main () {
    // ambient intensity
    vec3 la = La*Ka;

    // diffuse lighting
    vec3 light_position_eye = vec3(view * orig_model_mat * vec4(light_position_world, 1.0));
    vec3 distance_to_light_eye = light_position_eye - position_eye;
    vec3 direction_to_light_eye = normalize(distance_to_light_eye);
    vec3 norm_normal_eye = normalize(normal_eye);
    float dot_prod = max(dot(direction_to_light_eye, norm_normal_eye), 0.0);
    vec3 ld = Ld*Kd*dot_prod; // final diffuse intensity
    //vec3 ld = colour*dot_prod;

    //specualr intensity
    vec3 reflection_eye = reflect(-direction_to_light_eye, normal_eye);
    reflection_eye = normalize(reflection_eye);
    vec3 surface_to_viewer_eye = normalize(-position_eye);
    float dot_prod_specular = max(dot(reflection_eye, surface_to_viewer_eye), 0.0);
    float specular_factor = pow(dot_prod_specular, specular_exponent);
    vec3 ls = Ls*Ks*specular_factor;

    float t = 1.0;
    frag_colour = vec4 ((ld)*colour, 1.0);
}
