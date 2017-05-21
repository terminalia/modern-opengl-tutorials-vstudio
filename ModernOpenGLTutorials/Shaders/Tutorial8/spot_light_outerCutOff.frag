#version 410

/*
	SPOT LIGHTS
	
	A spotlight in OpenGL is represented by a world-space position, a direction and a cutoff angle that specifies 
	the radius of the spotlight. For each fragment we calculate if the fragment is between the spotlight's cutoff 
	directions (thus in its cone) and if so, we lit the fragment accordingly.

	-LightDir: the vector pointing from the fragment to the light source.
	-SpotDir: the direction the spotlight is aiming at.
	-Phi: the cutoff angle that specifies the spotlight's radius. 
	      Everything outside this angle is not lit by the spotlight.	
	-Theta: the angle between the LightDir vector and the SpotDir vector. 
	        The Theta value should be smaller than the Phi value to be inside the spotlight.

*/

in vec2 uv;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 uCameraPosition;
uniform sampler2D diffuseTexture;

out vec4 finalColor;

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularStrength;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};
uniform SpotLight spotLight;

void main () {
	vec3 diffuseMap = vec3(texture(diffuseTexture, uv));

	//Compute quadratic attenuation
	float distance = length(spotLight.position - FragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(spotLight.position - FragPos);
	vec3 viewDir = normalize(uCameraPosition - FragPos);

	//Ambient
	vec3 ambient = spotLight.ambient * diffuseMap;

	//Diffuse
	//Find the angle between the fragment normal and the direction to the light source
	float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
	vec3 diffuse = spotLight.diffuse * diff;

	//Specular
	//Compute R vector. Negate the lightDir vector because reflect wants the first parameter to point from light position to the vertex position
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), spotLight.specularStrength);
	vec3 specular = spotLight.specular * spec;

	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = (spotLight.cutOff - spotLight.outerCutOff);
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

	ambient  *= intensity; 
	diffuse  *= intensity;
	specular *= intensity;   

	//Apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 lighting = ambient + (diffuse + specular) * diffuseMap;
	finalColor = vec4(lighting, 1.0f);
}