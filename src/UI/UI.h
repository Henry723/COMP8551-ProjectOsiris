// Created using code from https://learnopengl.com/In-Practice/Text-Rendering

#pragma once

#include "../components.hpp"
#include "../SceneManager.hpp"
#include "TextShader.h"
#include "../Renderer3D/Shader.h"
#include "../Renderer3D/stb_image.h"
#include <map>
#include <string>
#include "../SceneManager.hpp"


#include "../Events/EventSystem.h"
#include <entityx/Event.h>

#include "../gamestate.h"

using entityx::EventManager;
using entityx::Event;
using entityx::Receiver;

// TextElement struct used to add new text elements to be rendered
struct TextElement {
    std::string value; // Value of the string being rendered
    float posX, posY, scale; // Coordinates of where the text should begin being rendered (not the center of the element), and it's scale
    glm::vec3 color; // Three values between 0 and 1 (RGB)
    bool active; // Should this text element be rendered
};

// UISystem is a singleton class
//class UISystem : public System<UISystem>
struct UISystem : public System<UISystem>, public Receiver<UISystem>, EntityX
{
    static UISystem& instance()
    {
        static UISystem instance;
        return instance;
    }
    // Character struct used to configure each character that will be supported
    struct Character {
        unsigned int TextureID; // ID handle of the glyph texture
        glm::ivec2   Size;      // Size of glyph
        glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
        unsigned int Advance;   // Horizontal offset to advance to next glyph
    };

    std::map<GLchar, Character> Characters;
    unsigned int VAO, VBO;
    TextShader shader; // Uninitialized shader so RenderText can reference it without calling ShaderSetup() every time
    Shader shapeShader; // Uninitialized shader so RenderText can reference it without calling ShaderSetup() every time
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    int numElements; // number of text elements in the vector textElements
    struct Shape2D {
        unsigned int TextureID;
        glm::vec4    Vertices[6] = {

            glm::vec4(0.0f,   600.0f, 0.0f, 0.0f),
            glm::vec4(0.0f,   0.0f,   0.0f, 1.0f),
            glm::vec4(800.0f, 600.0f, 1.0f, 0.0f),

            glm::vec4(0.0f,   0.0f, 0.0f, 1.0f),
            glm::vec4(800.0f, 0.0f,   1.0f, 1.0f),
            glm::vec4(800.0f, 600.0f, 1.0f, 0.0f)

        };
        //glm::vec4    Vertices[6] = {
        //    glm::vec4(0.0f,   600.0f, 0.0f, 0.0f),
        //    glm::vec4(0.0f,   0.0f,   0.0f, 1.0f),
        //    glm::vec4(800.0f, 600.0f, 1.0f, 1.0f),


        //    glm::vec4(0.0f,   600.0f, 0.0f, 0.0f),
        //    glm::vec4(800.0f, 600.0f, 1.0f, 1.0f),
        //    glm::vec4(800.0f, 0.0f,   1.0f, 0.0f)
        //};
    };
    Shape2D StartMenu;
public:
    static UISystem& getInstance()
    {
        static UISystem instance;
        return instance;
    }
    void update(EntityManager&, EventManager&, TimeDelta) override; // unused as rendering is done within RenderSystem.cpp
    void configure(EventManager& em) override; // Subscribes to each input event
    void receive(const ControlInput& event); // Toggles movement bools to be picked up by update
    void setup(); // Can be called to begin doing all steps to setup the system (LoadFreeType, CreateVAOVBO)
    void ShaderSetup(); // Returns a shader that is used for text rendering
    int LoadFreeType(); // Initializes FreeType by loading a font and configuring glyphs / bitmaps for the specified font
    int LoadStartMenu(); // Initializes FreeType by loading a font and configuring glyphs / bitmaps for the specified font
    void CreateVAOVBO(); // Sets up the VAO and VBOs which will be used by the RenderText function
    void RenderText(TextShader& shader, std::string text, float x, float y, float scale, glm::vec3 color); // Used within the render loop to render 2D text
    void RenderShape2d(Shader& shader, Shape2D& shape); // render 2D shape 
    void RenderStartMenu();
    void RenderAll(); // Loops through all elements in textElements and renders them if they are set to active
    int NewTextElement(std::string value, float posX, float posY, float scale, glm::vec3 color, bool active); // Adds a new text element and returns the ID used to access it from textElements
    std::vector<TextElement> textElements; // Container for all textElements
    void generateMenuText();
    void RenderMenuText();
    std::vector<TextElement> textMenuElements; // Container for all textElements
    bool configured = false; // Set to false and indicates to the UI system that setup should be performed
};