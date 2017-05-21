#version 410

in vec2 uv;
uniform sampler2D textureSampler;
out vec4 frag_colour;

void main () {
	vec4 texel = texture(textureSampler, uv);
	frag_colour = texel;
}