#version 410

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform float near_plane;
uniform float far_plane;
uniform bool uIsDirLight;

float bug = 0.0;

// required when using a perspective projection matrix, otherwise shadowmap is all white
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main() {

	float depthValue = texture2D(depthMap, TexCoords).r;
	if (uIsDirLight) {
		// orthographic
		FragColor = vec4(vec3(depthValue), 1.0); 
		FragColor.x += bug;
	}
	else {
		// perspective
		FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0);
	}
    
}