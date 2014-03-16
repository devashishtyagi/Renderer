#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec2 texCordinates;


out vec3 controlpoint_wor;
out vec3 normaltcs;
out vec2 textcs;
out vec3 colourvs;

uniform mat4 model;

void main () {
	//controlpoint_wor = (model * vec4 (vertex_position, 1.0)).xyz;
	//normaltcs = (model * vec4(vertex_normal.xyz,1.0)).xyz;
	controlpoint_wor = vertex_position;
    normaltcs = vertex_normal;
    textcs = texCordinates;
	colourvs = vertex_colour;
}
