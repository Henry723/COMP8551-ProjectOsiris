// FragmentShader to render 2d shape
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D textureMap;

void main()
{    
    vec3 sampled = texture(textureMap, TexCoords).rgb;
    color = vec4(sampled, 1.0);
}