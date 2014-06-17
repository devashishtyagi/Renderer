#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_colour;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec2 texCordinates;
layout(location = 4) in vec4 vertex_tangent;

out vec3 controlpoint_wor;
out vec3 normaltcs;
out vec3 tangenttcs;
out vec3 bitangenttcs;
out vec2 textcs;
out vec3 colourvs;

uniform mat4 model, view, proj;

void main () {	
	controlpoint_wor = vertex_position;
    normaltcs = vertex_normal;
    tangenttcs = vertex_tangent.xyz;
    vec3 bitangent = cross(vertex_normal, vertex_tangent.xyz)*vertex_tangent.w;
    bitangenttcs = bitangent;
    textcs = texCordinates;
	colourvs = vertex_colour;
}
