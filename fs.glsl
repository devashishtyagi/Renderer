#version 410

in vec3 colourtes;
in vec2 TexCoord_FS_in;
uniform sampler2D gColorMap;                                                                
out vec4 frag_colour;

void main () {
//	frag_colour = vec4 (1.0,1.0,1.0, 1.0);
//	frag_colour = vec4 (colourtes,1.0);
	frag_colour = texture(gColorMap, TexCoord_FS_in.xy);
}