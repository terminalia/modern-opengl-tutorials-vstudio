#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

uniform mat4 uProjectionMat;
uniform mat4 uViewMat;
uniform mat4 uModelMat;
uniform mat4 uNormalMat;

out vec2 uv;
out vec3 Normal;
out vec3 FragPos;

void main () {
    uv = inTexCoords;
	//Pass to fragment shader the vertex position in world space
	FragPos = vec3(uModelMat * vec4(inPosition, 1.0f));
	//Pass to fragment shader the surface normal ready for scaling and rotation
	Normal = vec3(mat3(uNormalMat) * inNormal);

	gl_Position = uProjectionMat *uViewMat * uModelMat * vec4(inPosition, 1.0f);
}