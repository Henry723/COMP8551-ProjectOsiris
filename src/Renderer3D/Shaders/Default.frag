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

void main()
{
    // The ambient light that will apply to all of the objects
    vec3 ambient = light.ambient * texture(texture1, TexCoord).rgb;

    // Diffuse lighting to show how light bounces when it comes to each faces
    vec3 normal = normalize(vertexColor);
    vec3 lightDirection = normalize(light.position - FragPos);
    float diff = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse = vec3(light.diffuse) * diff * texture(texture1, TexCoord).rgb;

    // Specular lighting to show how light reflects when it comes to each faces
    vec3 viewDir = normalize(positionOffset - FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);  

    //the final results of the color 
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}