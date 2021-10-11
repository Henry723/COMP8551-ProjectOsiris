// Created using code from https://learnopengl.com/In-Practice/Text-Rendering

#pragma once

#include "../components.hpp"
#include "../SceneManager.hpp"
#include "TextShader.h"
#include <map>
#include <string>

class UISystem : public System<UISystem>
{
    // Character struct used to configure each character that will be supported
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    // TextElement struct used to add new text elements to be rendered
    struct TextElement {
        std::string value; // Value of the string being rendered
        float posX, posY, scale; // Coordinates of where the text should begin being rendered (not the center of the element), and it's scale
        glm::vec3 color; // Three values between 0 and 1 (RGB)
        bool active; // Should this text element be rendered
    };

    std::map<GLchar, Character> Characters;
    std::vector<TextElement> textElements;
    unsigned int VAO, VBO;
    TextShader shader; // Uninitialized shader so RenderText can reference it without calling ShaderSetup() every time

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
public:
	void update(EntityManager&, EventManager&, TimeDelta) override;
    void setup(); // Can be called to begin doing all steps to setup the system (LoadFreeType, CreateVAOVBO)
    void ShaderSetup(); // Returns a shader that is used for text rendering
    int LoadFreeType(); // Initializes FreeType by loading a font and configuring glyphs / bitmaps for the specified font
    void CreateVAOVBO(); // Sets up the VAO and VBOs which will be used by the RenderText function
    void RenderText(TextShader& shader, std::string text, float x, float y, float scale, glm::vec3 color); // Used within the render loop to render 2D text
    void RenderAll(); // Loops through all elements in textElements and renders them if they are set to active
    void NewTextElement(std::string value, float posX, float posY, float scale, glm::vec3 color, bool active); // Adds a new text element
    bool configured = false; // Set to false and indicates to the UI system that setup should be performed
};
