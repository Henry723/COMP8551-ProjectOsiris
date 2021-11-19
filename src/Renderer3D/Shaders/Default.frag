#version 330 core
struct PointLight {
    vec3 position;
    vec3 color;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 2
uniform PointLight pointLights[NR_POINT_LIGHTS];

#define OUTLINE_COLOR vec4(0, 0, 0, 1)
#define OUTLINE_THICKNESS 0.3f

struct Material {
    vec3 specular;    
    float shininess;
}; 

out vec4 FragColor;

in vec3 vertexColor; // the input variable from the vertex shader (same name and type as in vert)
in vec2 TexCoord; // the input variable from the vertex shader (same name and type as in vert)
in vec3 FragPos; // the input variable from the vertex shader (same name and type as in vert)

uniform vec3 positionOffset; // The global "uniform" position variable, possibly updated per frame through an external loop?
uniform sampler2D texture1; // This uniform allows us to assign our texture as output for our Fragment shader
//uniform Light light; // This uniform creates our light object for ambient, diffuse, and specular lights.
uniform Material material; // This uniform creates our material object for the models we imported


//This function will segment light values to create another way to cel shading 
float Posterize(float steps, float value){
    return floor(value * steps) / steps;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    //Posterize
    diff = Posterize(3, diff);
    spec = Posterize(2, spec);

    //combine results
    vec3 ambient = light.ambient * texture(texture1, TexCoord).rgb;
    vec3 diffuse = vec3(light.diffuse) * diff * texture(texture1, TexCoord).rgb;
    vec3 specular = light.specular * (spec * material.specular);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + (diffuse + specular) * light.color);
}



void main()
{
    vec3 norm = normalize(vertexColor);
    vec3 viewDir = normalize(positionOffset - FragPos);

    vec3 result = vec3(0, 0, 0);
    //point lights calculation
    for(int i=0; i<NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    
    float maxAngleVal = max(0.0, dot(norm, viewDir));

    if(maxAngleVal < OUTLINE_THICKNESS)
        FragColor = OUTLINE_COLOR;
    else
        FragColor = vec4(result, 1.0);

    // The ambient light that will apply to all of the objects
    //vec3 ambient = light.ambient * texture(texture1, TexCoord).rgb;

    // Diffuse lighting to show how light bounces when it comes to each faces
    //vec3 normal = normalize(vertexColor);
    //vec3 lightDirection = normalize(light.position - FragPos);
    //float diff = max(dot(normal, lightDirection), 0.0);
    //vec3 diffuse = vec3(light.diffuse) * diff * texture(texture1, TexCoord).rgb;

    // Specular lighting to show how light reflects when it comes to each faces
    //vec3 viewDir = normalize(positionOffset - FragPos);
    //vec3 reflectDir = reflect(-lightDirection, normal);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * (spec * material.specular);  

    //the final results of the color 
    //vec3 result = ambient + diffuse + specular;

    //FragColor = vec4(result, 1.0);
}