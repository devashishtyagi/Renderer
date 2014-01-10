#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in float displace;

out vec3 colour, position_eye, normal_eye;
out float depth;
uniform mat4 view, proj, model, orig_model_mat;

void main () {
    colour = vertex_colour;
    position_eye = vec3(view*model*vec4(vertex_position, 1.0));
    normal_eye = vec3(view*model*vec4(vertex_normal, 0.0));
    gl_Position = proj * view * model * vec4 (vertex_position + displace*vertex_normal, 1.0);
    depth = abs(gl_Position.z);
}
