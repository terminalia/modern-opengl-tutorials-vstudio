#version 410

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

out vec2 Texcoords;

void main () {
    Texcoords = inTexCoords;
	//Since the quad on we render is on NDC coordinates, we don't multiply by any matix
	gl_Position = vec4(inPosition.x, inPosition.y, 0.0f, 1.0f);
}