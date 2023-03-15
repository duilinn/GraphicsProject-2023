#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

in vec3 Normal;  
in vec3 FragPos;  
in float distance;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;

//adapted from https://learnopengl.com/Lighting/Materials and https://learnopengl.com/Getting-started/Shaders

uniform struct Light {
    vec3 position;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
}; 

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;

uniform float alpha;
uniform Light light;
uniform Light colouredLight;

float weight(float a, float b, float aWeight)
{
    return (a * aWeight) + (b * (1-aWeight));
}

void main()
{
//adapted from https://learnopengl.com/Lighting/Materials

// ambient
    float ambientStrength = 0.3;
    vec3 ambient = material.ambient * light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 colouredLightDir = normalize(colouredLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float colouredDiff = max(dot(norm, colouredLightDir), 0.0);
    vec3 diffuse = ((diff * material.diffuse) * light.diffuse) + (colouredDiff * colouredLight.diffuse);
    
    //specular
    
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128.0);
    vec3 specular = material.specular * spec * light.specular;  

    //specular for colouredLight
    float colouredSpecularStrength = 1.0;
    vec3 colouredReflectDir = reflect(-colouredLightDir, norm);

    float colouredSpec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    vec3 colouredSpecular = material.specular * colouredSpec * colouredLight.specular;

    vec4 result = vec4((ambient + diffuse + ((specular + colouredSpecular))), alpha) * texture(texture_diffuse1, TexCoords);

    //add fog
    vec4 fogColour = vec4(1.0, 1.0, 1.0, result.w);
    float fogWeighting = 1 - (1/(distance / 5));

    vec4 addedFog = (result * (1 - fogWeighting)) + (fogColour * fogWeighting);
    FragColor = addedFog;
}