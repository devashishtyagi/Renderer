#version 410

// triangles, quads, or isolines
layout (triangles, equal_spacing, ccw) in;
in vec3 evaluationpoint_wor[];
in vec3 colourtcs[];
in vec3 normaltes[];
in vec3 tangenttes[];
in vec3 bitangenttes[];
in vec2 textes[];
uniform sampler2D gDisplacementMap;

out vec3 colourtes;

// could use a displacement map here


vec3 WorldPos_FS_in;
vec3 Normal_FS_in;
out vec2 TexCoord_FS_in;
out vec3 position_eye, normal_eye;
out mat3 TBN;

uniform mat4 model, view, proj;

// gl_TessCoord is location within the patch
// (barycentric for triangles, UV for quads)

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}


vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main () {
	
	
	WorldPos_FS_in = interpolate3D(evaluationpoint_wor[0], evaluationpoint_wor[1], evaluationpoint_wor[2]);
	TexCoord_FS_in = interpolate2D(textes[0], textes[1], textes[2]);
    Normal_FS_in = interpolate3D(normaltes[0], normaltes[1], normaltes[2]);
    Normal_FS_in = normalize(Normal_FS_in);
    vec3 Tangent_FS_in = interpolate3D(tangenttes[0], tangenttes[1], tangenttes[2]);
    Tangent_FS_in = normalize(Tangent_FS_in);
    vec3 Bitangent_FS_in = interpolate3D(bitangenttes[0], bitangenttes[1], bitangenttes[2]);
    Bitangent_FS_in = normalize(Bitangent_FS_in);
	colourtes = interpolate3D(colourtcs[0],colourtcs[1],colourtcs[2]);
 
	// Displace the vertex along the normal
    float Displacement = texture(gDisplacementMap, TexCoord_FS_in.xy).x;
    WorldPos_FS_in += Normal_FS_in * Displacement * 0.05;

    // Construct TBN matrix
    vec3 n = Normal_FS_in;
    vec3 t = Tangent_FS_in;
    vec3 b = Bitangent_FS_in;
    vec3 t_i = normalize(t - n * dot(n, t));
    float det = dot(cross(n, t), b);
    if (det < 0.0)
        det = -1.0;
    else
        det = 1.0;
    b = cross(n, t_i)*det;
    mat3 dir_mvp = mat3(inverse( transpose(view*model)));
    n = dir_mvp*normalize(n);
    t = dir_mvp*normalize(t_i);
    b = dir_mvp*normalize(b);
    TBN = transpose(mat3(t, b, n));

    position_eye = vec3(view * model * vec4(WorldPos_FS_in, 1.0));
    normal_eye = vec3(inverse ( transpose (view * model)) * vec4(Normal_FS_in, 1.0));

	gl_Position =  proj * view * model * vec4 (WorldPos_FS_in, 1.0);
}
