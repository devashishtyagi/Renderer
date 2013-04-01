layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;

out vec3 colour;
uniform mat4 view, proj, model;

void main () {
  colour = vertex_colour;
  gl_Position = proj * view * model * vec4 (vertex_position, 1.0);
}
