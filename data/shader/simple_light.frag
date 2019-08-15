#version 330 core

in vec3 oNormal;
in vec3 oPos;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(oNormal);
    vec3 lightDir =  normalize(light.position- oPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    float speculationStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - oPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = spec * light.specular * material.specular;

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0f);
}
