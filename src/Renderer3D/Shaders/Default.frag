#version 330 core
out vec4 FragColor;

in vec3 vertexColor; // the input variable from the vertex shader (same name and type as in vert)
in vec2 TexCoord;
in vec3 ambientLight;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 positionOffset; // The global "uniform" position variable, possibly updated per frame through an external loop?
uniform sampler2D texture1; // This uniform allows us to assign our texture as output for our Fragment shader
uniform Light light; // This uniform creates our light object for the ambient light for now

void main()
{
    // The ambient light that will apply to all of the objects
    vec3 ambient = light.ambient * texture(texture1, TexCoord).rgb;

    //the final results of the color 
    vec3 result = ambient;

    FragColor = vec4(result, 1.0);
}