#version 330 core
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

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
uniform Light light; // This uniform creates our light object for ambient, diffuse, and specular lights.
uniform Material material; // This uniform creates our material object for the models we imported

//This function will segment light values to create another way to cel shading 
float Posterize(float steps, float value){
    return floor(value * steps) / steps;
}

void main()
{
    // The ambient light that will apply to all of the objects
    vec3 ambient = light.ambient * texture(texture1, TexCoord).rgb;

    // Diffuse lighting to show how light bounces when it comes to each faces
    vec3 normal = normalize(vertexColor);
    vec3 lightDirection = normalize(light.position - FragPos);
    float diff = max(0.0, dot(normal, lightDirection));
    //diff = step(0.4, diff); //check how far of the models being hit by diffuse light (cel).
    diff = Posterize(3, diff); 
    vec3 diffuse = vec3(light.diffuse) * diff * texture(texture1, TexCoord).rgb;

    // Specular lighting to show how light reflects when it comes to each faces
    vec3 viewDir = normalize(positionOffset - FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);  
    float spec = max(0.0, dot(viewDir, reflectDir));
    spec = pow(spec, material.shininess);
    //spec = step(0.1, spec);
    spec = Posterize(2, spec);
    vec3 specular = light.specular * (spec * material.specular);  

    //the final results of the color 
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}