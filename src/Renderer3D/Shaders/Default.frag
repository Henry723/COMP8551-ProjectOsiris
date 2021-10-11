#version 330 core
out vec4 FragColor;

in vec3 vertexColor; // the input variable from the vertex shader (same name and type as in vert)
in vec2 TexCoord;

uniform vec3 positionOffset; // The global "uniform" position variable, possibly updated per frame through an external loop?

uniform sampler2D texture1; // This uniform allows us to assign our texture as output for our Fragment shader

void main()
{
	FragColor = texture(texture1, TexCoord) * vec4(1.0f);
}