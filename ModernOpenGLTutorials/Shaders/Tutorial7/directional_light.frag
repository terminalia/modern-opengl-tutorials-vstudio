#version 410

/*
	DIRECTIONAL LIGHT (SUN)
	A light that is INFINITELY far away. 
	All the rays comes from the same direction indipendently from the light position.

	Because all the light rays are parallel it does not matter how each object relates 
	to the light source's position since the light direction remains the same for 
	each object in the scene. Because the light's direction vector stays the same, 
	the lighting calculations will be similar for each object in the scene.
	We can model such a directional light by defining a light direction vector instead of a position vector

	Directional lights have no attenuation!
*/

in vec2 uv;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 uCameraPosition;
uniform sampler2D diffuseTexture;

out vec4 finalColor;

struct DirectionalLight {
	//Used instead of position in direction light
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularStrength;
};
uniform DirectionalLight dirLight;

void main () {
	vec3 diffuseMap = vec3(texture(diffuseTexture, uv));

	vec3 normal = normalize(Normal);
	//Commonly lightDir is the direction from fragment to light source but 
	//in directional light is the direction from light source to fragment that's why we negate the vector
	vec3 lightDir = normalize(-dirLight.direction);
	vec3 viewDir = normalize(uCameraPosition - FragPos);

	//Ambient
	vec3 ambient = dirLight.ambient * diffuseMap;

	//Diffuse
	//Find the angle between the fragment normal and the direction to the light source
	float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
	vec3 diffuse = dirLight.diffuse * diff;

	//Specular
	//Compute R vector. Negate the lightDir vector because reflect wants the first parameter to point from light position to the vertex position
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), dirLight.specularStrength);
	vec3 specular = dirLight.specular * spec;

	vec3 lighting = ambient + (diffuse + specular) * diffuseMap;
	finalColor = vec4(lighting, 1.0f);
}