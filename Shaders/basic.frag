#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
    float constant;
	float linear;
	float quadratic;
};

in vec3 fragColor;
out vec4 outColor;
in vec2 texCoords;
in vec3 vertNormal;
in vec3 fragPos;
uniform sampler2D outTex;
uniform bool wireframeMode;

uniform vec3 viewPos;
uniform Light light;
uniform Material material;


void main()
{
    float dist = distance(light.position, fragPos);
    float attenuation = 1.0 / (light.constant + light.linear*dist + light.quadratic * dist * dist);

    vec3 ambient = light.ambient*material.ambient * attenuation;

    //diffuse
    vec3 norm = normalize(vertNormal);
    vec3 lightDir = normalize(fragPos - light.position);
    float diff_koef = max(dot(norm, -lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff_koef * material.diffuse) * attenuation;
    //specular
    vec3 lightpos = light.position;
    float spec_koef = 0;
    
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(fragPos - viewPos);
    spec_koef += pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    
    
    
    vec3 specular = light.specular * (spec_koef * material.specular) * attenuation;
        
    if (wireframeMode) {
        outColor = vec4(fragColor, 1.0f);
    } else {        
        outColor = texture(outTex, texCoords) * vec4(ambient + diffuse + specular, 1.0f);
    }
}
