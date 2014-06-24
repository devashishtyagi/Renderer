#version 410

// number of CPs in patch
layout (vertices = 3) out;

// from VS (use empty modifier [] so we can say anything)
in vec3 controlpoint_wor[];
in vec3 colourvs[];
in vec3 normaltcs[];
in vec3 tangenttcs[];
in vec3 bitangenttcs[];
in vec2 textcs[];
in vec3 tex_normaltcs[];
//in float weathering_degree_tcs[];
//in vec4 weathering_degree_neighbour_tcs[];

// to evluation shader. will be used to guide positioning of generated points
out vec3 evaluationpoint_wor[];
out vec3 colourtcs[];
out vec3 normaltes[];
out vec3 tangenttes[];
out vec3 bitangenttes[];
out vec2 textes[];
out vec3 tex_normaltes[];
//out float weathering_degree_tes[];
//out vec4 weathering_degree_neighbour_tes[];


uniform float tessLevelInner; // controlled by keyboard buttons
uniform float tessLevelOuter; // controlled by keyboard buttons


void main () {
    evaluationpoint_wor[gl_InvocationID] = controlpoint_wor[gl_InvocationID];
    colourtcs[gl_InvocationID] = colourvs[gl_InvocationID];
    normaltes[gl_InvocationID] = normaltcs[gl_InvocationID];
    tangenttes[gl_InvocationID] = tangenttcs[gl_InvocationID];
    bitangenttes[gl_InvocationID] = bitangenttcs[gl_InvocationID];
    textes[gl_InvocationID] = textcs[gl_InvocationID];
    tex_normaltes[gl_InvocationID] = tex_normaltcs[gl_InvocationID];
    //weathering_degree_tes[gl_InvocationID] = weathering_degree_tcs[gl_InvocationID];
    //weathering_degree_neighbour_tes[gl_InvocationID] = weathering_degree_neighbour_tcs[gl_InvocationID];

    // Calculate the tessellation levels
    gl_TessLevelInner[0] = tessLevelInner; // number of nested primitives to generate
    gl_TessLevelOuter[0] = tessLevelOuter; // times to subdivide first side
    gl_TessLevelOuter[1] = tessLevelOuter; // times to subdivide second side
    gl_TessLevelOuter[2] = tessLevelOuter; // times to subdivide third side
}
