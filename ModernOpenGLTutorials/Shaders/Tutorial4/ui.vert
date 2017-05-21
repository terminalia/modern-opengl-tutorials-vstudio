#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 uProjectionMat;
uniform mat4 uViewMat;
uniform mat4 uModelMat;
uniform mat4 uNormalMat;

out vec3 color;

void main () {
    color = vertex_color;
	gl_Position = uProjectionMat * uViewMat * uModelMat * vec4(vertex_position, 1.0f);
}