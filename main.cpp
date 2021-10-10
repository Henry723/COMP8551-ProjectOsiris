#include "src/GameControl.h"

// ---
// (TEMP) Global Properties
// ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// ---
// Function declarations / prototypes.
// ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {
	// Instantiate the GLFW window
	glfwInit();

	// Configure options provided by glfw.
	//	Full option list at https://www.glfw.org/docs/latest/window.html#window_hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Set what version of opengl we're using
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Explicitly tell GLFW to use the core profile
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // This line is for MacOSX support
	#endif
	
	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GLFW Window", NULL, NULL);


	// 1. If the window failed to create, return -1.
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// 2. Set the context to our new window.
	glfwMakeContextCurrent(window);

	// 3. GLAD manages function pointers to OpenGL.
	//		Any OpenGL function can be called through GLAD.
	//		The glfwGetProcAddress defines the correct function based on which OS we're compiling for.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 4. Let OpenGL know the initial dimensions (in pixels) of the window.
	//		First two parameters are location of lower left corner.
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// 5. Link our function to dispatch when a window is resized.
	//		There are many GLFW callbacks that can be linked to functions,
	//		including joystick input and error messages.
	//
	//		We link these AFTER the window is created, but BEFORE the render loop is initiated.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Use scene manager to Create game control
	SceneManager& scene = SceneManager::getInstance();
	scene.setScene("filename");
	GameControl* gamecontrol = new GameControl(window, scene.getScene());

	// ---
	// This is our Render Loop!
	//		We want the application to keep looping until explicitly being told to stop.
	// --- 
	while (!glfwWindowShouldClose(window)) // glfwWindowShouldClose checks each render iteration for a signal to close.
	{
		// TODO allow for new "scene"
		gamecontrol->Update(0.1f);

		if (scene.isNewScene())
			gamecontrol = new GameControl(window, scene.getScene());

	}
	delete gamecontrol;
	// Once we exit the Render Loop, we clean-up & return.
	glfwTerminate();

	return 0;
}

// ---
// Function definitions.
// ---

// A callback function that is set to dispatch when the user resizes the window.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// A function to process contextual input in the GLFW Window.
//		This must be called each Render iteration (frame).
void processInput(GLFWwindow* window)
{
	// If Escape key is pressed, send the "Close Window" signal to GLFW and break the Render Loop.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // GLFW_RELEASE is the opposite of GLFW_PRESS, like KeyUp and KeyDown
		glfwSetWindowShouldClose(window, true);
}