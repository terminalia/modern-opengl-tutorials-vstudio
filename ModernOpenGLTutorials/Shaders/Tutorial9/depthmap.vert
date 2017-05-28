#version 410

layout (location = 0) in vec3 inPosition;

uniform mat4 uLightSpaceMat;
//Model matrix coming from each object in the scene
uniform mat4 uModelMat;

void main()
{
    gl_Position = uLightSpaceMat * uModelMat * vec4(inPosition, 1.0f);
}