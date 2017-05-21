#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

uniform mat4 uProjectionMat;
uniform mat4 uViewMat;
uniform mat4 uModelMat;

out vec2 uv;

void main () {
    uv = inTexCoords;
	mat4 MVP = uProjectionMat *uViewMat * uModelMat;
	gl_Position = MVP * vec4(inPosition, 1.0f);
}