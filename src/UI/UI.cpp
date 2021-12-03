// Created using code from https://learnopengl.com/In-Practice/Text-Rendering
#include "UI.h"

int UISystem::LoadFreeType() {
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "./src/UI/fonts/arial.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Move onto creating VAO and VBOs
    CreateVAOVBO();
    cout << "UI System: Loaded FreeType" << endl;
    return 1;
}


int UISystem::LoadStartMenu() {
    glGenTextures(1, &StartMenu.TextureID);

    int width, height, nrComponents;
    const char* path = "./src/UI/textures/startmenu.jpg";
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, StartMenu.TextureID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Set texture image 4byte-alignment

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return 1;
}

int UISystem::LoadTimer() {
    glGenTextures(1, &Timer.TextureID);

    int width, height, nrComponents;
    const char* path = "./src/UI/textures/timer.jpg";
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, Timer.TextureID);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Set texture image 4byte-alignment

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return 1;
}

void UISystem::CreateVAOVBO() {
    // configure VAO/VBO for texture quads
// -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    cout << "UI System: Created VAO & VBOs" << endl;
}

void UISystem::RenderText(TextShader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void UISystem::RenderShape2d(Shader& shader, Shape2D& shape) {
    // activate corresponding render state	
    shader.use();
    //glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);


    glBindTexture(GL_TEXTURE_2D, shape.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shape.Vertices), &shape.Vertices);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(shape.Vertices), &shape.Vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64 pixels)

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void UISystem::RenderStartMenu() {
    RenderShape2d(shapeShader, StartMenu);
}

void UISystem::RenderAll()
{
    for (int i = 0; i < textElements.size(); i++)
    {
        TextElement curElement = textElements.at(i);
        if (curElement.active) // Only render the element if it is active
        {
            RenderText(shader, curElement.value, curElement.posX, curElement.posY, curElement.scale, curElement.color);
        }
    }
    RenderTimer();
}

void UISystem::RenderTimer()
{
    glm::vec4 TimerVertices[6] = {

            glm::vec4(TimerPivot - TimerWidth / 2, 595.0f, 0.0f, 0.0f),
            glm::vec4(TimerPivot - TimerWidth / 2, 565.0f,   0.0f, 1.0f),
            glm::vec4(TimerPivot + TimerWidth / 2, 595.0f, 1.0f, 0.0f),

            glm::vec4(TimerPivot - TimerWidth / 2, 565.0f, 0.0f, 1.0f),
            glm::vec4(TimerPivot + TimerWidth / 2, 565.0f,   1.0f, 1.0f),
            glm::vec4(TimerPivot + TimerWidth / 2, 595.0f, 1.0f, 0.0f)
    };
    std::copy(TimerVertices, TimerVertices + 6, Timer.Vertices);
    RenderShape2d(shapeShader, Timer);
}

int UISystem::NewTextElement(std::string value, float posX, float posY, float scale, glm::vec3 color, bool active)
{
    TextElement text = { value, posX, posY, scale, color, active };
    textElements.push_back(text);
    numElements += 1;
    //std::cout << "UISystem: " << value << ": numElements = " << numElements << std::endl;
    return numElements - 1; // returns element "ID" used to access the element in textElements
}

void UISystem::generateMenuText() {
    TextElement textHiScore = { "Hi-Score: 0000", 300.f, 560.f, 0.75f, glm::vec3(1.0, 1.0, 1.0), true };
    textMenuElements.push_back(textHiScore);

    TextElement textGameOver = { "Game Over", 250.f, 460.f, 1.3f, glm::vec3(1.0, 1.0, 1.0), true };
    textMenuElements.push_back(textGameOver);

    TextElement textScore = { "Score: 0000", 320.f, 400.f, 0.75f, glm::vec3(1.0, 1.0, 1.0), true };
    textMenuElements.push_back(textScore);

    TextElement textTime = { "Time Elapsed: 00:00:00", 215.f, 350.f, 0.75f, glm::vec3(1.0, 1.0, 1.0), true };
    textMenuElements.push_back(textTime);

    TextElement textReset = { "Press space to reset", 275.f, 180.f, 0.75f, glm::vec3(1.0, 1.0, 1.0), true };
    textMenuElements.push_back(textReset);

    TextElement textExit = { "Press esc to exit", 275.f, 130.f, 0.75f, glm::vec3(1.0, 1.0, 1.0), true };
    textMenuElements.push_back(textExit);
    //		healthText = ui.NewTextElement("Health: 1/1", 15.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
    //		scoreText = ui.NewTextElement("Score: 0000", 585.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
}

void UISystem::RenderMenuText() {
    for (int i = 0; i < textMenuElements.size(); i++)
    {
        TextElement curElement = textMenuElements.at(i);
        if (curElement.active) // Only render the element if it is active
        {
            RenderText(shader, curElement.value, curElement.posX, curElement.posY, curElement.scale, curElement.color);
        }
    }
}

void UISystem::setup() {
    // Currently only calls the LoadFreeType function and sets up the shader. Other setup steps can also be added here
    LoadFreeType();
    ShaderSetup();

    // TODO : REMOVE ONCE LoadStartMenu() can run outside of the setup method
    LoadStartMenu();
    LoadTimer();
    generateMenuText();
    numElements = 0;
}

void UISystem::ShaderSetup() // Create and return a shader for text rendering
{
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

    shader = TextShader("./src/UI/shaders/Default.vert", "./src/UI/shaders/Default.frag"); // Initialize the shader program
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    shapeShader = Shader("./src/UI/shaders/Default.vert", "./src/UI/shaders/shape2d.frag"); // Initialize the shader program
    shapeShader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void UISystem::update(EntityManager& es, EventManager& ev, TimeDelta dt)
{
    if (!configured) { // Initialize FreeType and VAO/VBOs + adds text elements to be rendered
        setup();
        //healthText = ui.NewTextElement("Health: 1/1", 15.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
        //scoreText = ui.NewTextElement("Score: 0000", 585.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
        //scoreText = ui.NewTextElement("Score: 0000", 585.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
        //scoreText = ui.NewTextElement("Score: 0000", 585.0f, 565.0f, 0.75f, glm::vec3(1.0, 1.0f, 1.0f), true);
        configured = true;
    }

    if (gameState != GameState::MENU) return;

    ////ComponentHandle<Color> hcolor;
    ComponentHandle<Window> hwindow;

    // Loop through window components (there will likely only be one.)
    ////auto en = es.entities_with_components(hwindow, hcolor);
    auto en = es.entities_with_components(hwindow);
    for (auto entity = en.begin(); entity != en.end(); ++entity)
    {
        ////Color* col = (*entity).component<Color>().get();
        // rendering commands

        // ...
        //glClearColor(col->red, col->green, col->blue, col->alpha); // state-setting function of OpenGL
        //glClear(GL_COLOR_BUFFER_BIT); // state-using function. Uses the current state defined to retrieve the clearing color.
        //
        // Enable these properties so FreeType can properly render text
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //RenderShape2d(shapeShader, StartMenu);
        RenderStartMenu();
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        ////update color buffer (a 2D buffer that contains color values for each pixel) to render during this iteration and show it as output to the screen.
        glfwSwapBuffers((*entity).component<Window>().get()->window);
    }

    //if (!configured) { // Initialize FreeType and VAO/VBOs 
    //    UISystem& ui = UISystem::getInstance();
    //    ui.setup();
    //    configured = true;
    //}

    // This is broken up, unfortunately, since the swapBuffers call must be after the Draw Call.
    //for (auto entity = en.begin(); entity != en.end(); ++entity)
    //{
    //    // Disable these properties so the RenderSystem can properly render
    //    glDisable(GL_CULL_FACE);
    //    glDisable(GL_BLEND);
    //    //update color buffer (a 2D buffer that contains color values for each pixel) to render during this iteration and show it as output to the screen.
    //    glfwSwapBuffers((*entity).component<Window>().get()->window);
    //}
}


void UISystem::configure(EventManager& em) {
    //em.subscribe<Collision>(*this);
}

