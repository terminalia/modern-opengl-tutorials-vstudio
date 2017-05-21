#version 410

/*
	POINT LIGHTS
	
	Point lights add attenuation to the Phong Shading model

	We can compute attenuation using these two methods:
	-linear attenuation
	-quadratic attenuation (more realistic)

    QUADRATIC ATTENUATION
	https://learnopengl.com/#!Lighting/Light-casters
    To get more realistic lighting we can compute the attenuation using the following formula:

        attenuation = 1.0 / Kc + Kl * d + Kq * d * d

    Where:
    -Kc = constant term usually kept 1.0 to make sure the resulting denominator never gets smaller than 1
    -Kl * d = linear term that is multiplied with the distance that reduces intensity in a linear fashion.
              More important when the distance is small.
    -Kq * d * d = quadratic term that is multiplied with the quadrant of the distance.
                  It gets more important as the distance grows.

    Kl and Kq values depends on many factors, on the tutorial page there's a nice table that reports
    values based on the distance we want to cover.
*/

in vec2 uv;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 uCameraPosition;
uniform sampler2D diffuseTexture;

out vec4 finalColor;

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularStrength;
	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLight;

void main () {
	vec3 diffuseMap = vec3(texture(diffuseTexture, uv));

	//Compute quadratic attenuation
	float distance = length(pointLight.position - FragPos);
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(pointLight.position - FragPos);
	vec3 viewDir = normalize(uCameraPosition - FragPos);

	//Ambient
	vec3 ambient = pointLight.ambient * diffuseMap;

	//Diffuse
	//Find the angle between the fragment normal and the direction to the light source
	float diff = clamp(dot(normal, lightDir), 0.0f, 1.0f);
	vec3 diffuse = pointLight.diffuse * diff;

	//Specular
	//Compute R vector. Negate the lightDir vector because reflect wants the first parameter to point from light position to the vertex position
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(clamp(dot(viewDir, reflectDir), 0.0f, 1.0f), pointLight.specularStrength);
	vec3 specular = pointLight.specular * spec;

	//Apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 lighting = ambient + (diffuse + specular) * diffuseMap;
	finalColor = vec4(lighting, 1.0f);
}