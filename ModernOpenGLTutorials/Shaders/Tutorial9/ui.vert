#version 410

layout(location = 0) in vec3 vertex_position;

uniform mat4 uProjectionMat;
uniform mat4 uViewMat;
uniform mat4 uModelMat;
uniform mat4 uNormalMat;
uniform vec3 uColor;

out vec3 color;

void main () {
    color = uColor;
	gl_Position = uProjectionMat * uViewMat * uModelMat * vec4(vertex_position, 1.0f);
}