#version 330 core

in vec3 oPos;
in vec3 oNormal;
in vec2 oTex;

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
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
    vec3 diffuseMap = texture(material.diffuse, oTex).rgb;
    vec3 specularMap = texture(material.specular, oTex).rgb;

    vec3 norm = normalize(oNormal);
    vec3 lightDir =  normalize(light.position- oPos);
    float diff = max(dot(norm, lightDir), 0.0f);

    vec3 viewDir = normalize(viewPos - oPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);


    vec3 ambient = light.ambient * diffuseMap;
    vec3 diffuse = light.diffuse * diff * diffuseMap;
    vec3 specular = light.specular * spec * specularMap;

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}
