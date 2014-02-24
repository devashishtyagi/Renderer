#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in float displace;

out vec3 colour, position_eye, normal_eye;
out vec2 texture_coordinates;
out float depth;
uniform mat4 view, proj, model, orig_model_mat;

void main () {
	texture_coordinates = vec2(vertex_colour.r, vertex_colour.g);
    colour = vertex_colour;
    vec3 vertex_nrml = normalize(vertex_normal);
    position_eye = vec3(view*model*vec4(vertex_position, 1.0));
    normal_eye = vec3(view*model*vec4(vertex_nrml, 0.0));
    gl_Position = proj * view * model * vec4 (vertex_position, 1.0);
    depth = abs(gl_Position.z);
}
