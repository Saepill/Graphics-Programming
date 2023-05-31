#version 430 core

in vec3 vsNormal;  
in vec3 vsPos; 
in vec2 vsTexCoord;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float c1;
	float c2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float c1;
	float c2;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
   
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

#define NUM_POINT_LIGHTS 3

uniform Material material;
uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight 
pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 fragColor;

void main()
{   
	vec3 norm = normalize(vsNormal);
	vec3 viewDir = normalize(viewPos - vsPos);

	// directional Lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	// point Light
    for(int i = 0 ; i < NUM_POINT_LIGHTS ; i++)
	{
		result += CalcPointLight(pointLights[i], norm, vsPos, viewDir);

	}
	
	// spot Light
	result += CalcSpotLight(spotLight, norm, vsPos, viewDir);

    fragColor = vec4(result, 1.0);
} 

// directional light 계산 함수
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, vsTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vsTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vsTexCoord));

	return (ambient + diffuse + specular);
}

// point light 계산 함수
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + light.c1 * distance + light.c2 * (distance * distance));

	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, vsTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vsTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vsTexCoord));

	return (ambient + diffuse + specular) * attenuation;
}

// spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// Soft edge spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (1.0 + light.c1 * distance + light.c2 * (distance * distance));

	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, vsTexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vsTexCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, vsTexCoord));

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}