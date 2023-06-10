#version 430 core

in vec3 vsNormal;  
in vec3 vsPos;  
in vec2 vsTexCoord;

struct Material { 
    sampler2D diffuse; 
    sampler2D specular; 
    float shininess; 
    float alpha;
    float diffuse_control;

	vec3 defaultAmbient;
	vec3 defaultDiffuse;
	vec3 defaultSpecular;
	int useDiffuseMap;
	int useSpecularMap;
}; 

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

#define NUM_POINT_LIGHTS 2 

uniform Material material;
uniform vec3 viewPos; 
uniform DirLight dirLight;
uniform PointLight pointLights[NUM_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform int useNormal;

out vec4 fragColor;

// 광원별 컬러 계산 함수 정의
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 matAmbientColor;
vec3 matDiffuseColor;
vec3 matSpecularColor;

void main()
{
	if(useNormal != 0)
	{
		// properties
		vec3 norm = normalize(vsNormal);
		vec3 viewDir = normalize(viewPos - vsPos);

		matAmbientColor = material.defaultAmbient;
		matDiffuseColor = material.defaultDiffuse;
		matSpecularColor = material.defaultSpecular;

		if(material.useDiffuseMap != 0)
		{
            vec4 texColor = texture(material.diffuse, vsTexCoord);
            if(texColor.a < 0.1)
                discard;
            matAmbientColor = vec3(texColor) * material.diffuse_control;
			matDiffuseColor = vec3(texColor) * material.diffuse_control;
		}

		if(material.useSpecularMap != 0)
		{
			matSpecularColor = vec3(texture(material.specular, vsTexCoord));
		}
    
		// phase 1: directional lighting
		vec3 result = CalcDirLight(dirLight, norm, viewDir);
		// phase 2: point lights
		for(int i = 0; i < NUM_POINT_LIGHTS; i++) 
		{
			result += CalcPointLight(pointLights[i], norm, vsPos, viewDir);    
		}
		// phase 3: spot light
		result += CalcSpotLight(spotLight, norm, vsPos, viewDir);    
        
		fragColor = vec4(result, material.alpha);
	}
    else
        fragColor = vec4(material.defaultDiffuse, material.alpha);
} 

// directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * matAmbientColor;
    vec3 diffuse = light.diffuse * diff * matDiffuseColor;
    vec3 specular = light.specular * spec * matSpecularColor;
    
	return (ambient + diffuse + specular);
}

// point light
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
    // combine results
    vec3 ambient = light.ambient * matAmbientColor;
    vec3 diffuse = light.diffuse * diff * matDiffuseColor;
    vec3 specular = light.specular * spec * matSpecularColor;

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
    vec3 ambient = light.ambient * matAmbientColor;
    vec3 diffuse = light.diffuse * diff * matDiffuseColor;
    vec3 specular = light.specular * spec * matSpecularColor;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}