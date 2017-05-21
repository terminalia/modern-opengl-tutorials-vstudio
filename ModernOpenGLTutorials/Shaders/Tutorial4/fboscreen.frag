#version 410

in vec2 Texcoords;

uniform sampler2D textureSampler;

out vec4 final_color;

void main () {
	vec4 texel = texture(textureSampler, Texcoords);
	final_color = texel;
}