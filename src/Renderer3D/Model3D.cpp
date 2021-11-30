#include "Model3D.h"
#define TEST_MODEL3D       false

// Member functions definitions including constructor
Model3D::Model3D(const char* modelSource, const char* vertPath, const char* fragPath, const char* texPath) {

#if TEST_MODEL3D
	cout << endl << "<<<<<<<<<<<<<<<<<<< RENDER SYSTEM TEST START >>>>>>>>>>>>>>>>>>>>>" << endl;
	cout << endl << "Model3D is being created." << endl;

	cout << "In Model3D: Assigning Attribs of " << modelSource << " to vector ID #0x" << &attributes << endl;
	cout << "In Model3D: Assigning Indices of " << modelSource << " to vector ID #0x" << &indices << endl;
	cout << "=====================================" << endl;
#endif

	ModelImporter model_imp = ModelImporter();
	model_imp.loadModel(modelSource, attributes, indices);
	// MODEL_IMP.loadModel(modelSource, attributes, indices);

	//cout << "In Model3D: AFTER index load: " << indices.size() << endl;
	numIndices = indices.size();

	// ..:: Initialization code (done once (unless your object frequently changes)) ::..
	unsigned int VBO, EBO, VAO;
	glGenVertexArrays(1, &VAO);

	// 1. Create an ID for a new VBO & EBO to send to the Vertex Shader for rendering, stored in the GPU.
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// ------------------ Vertex Array Object --------------------
	glBindVertexArray(VAO);

	// VBO's must be bound to a unique buffer object type, in this case GL_ARRAY_BUFFER.
	//		Any buffer calls made on GL_ARRAY_BUFFER will refer to and configure our VBO until it is re-bound.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Copy the previously defined vertex data into the bound buffer's memory.
	//		The graphics card will manage the data as follows:
	//			GL_STREAM_DRAW: the data is set only once, and used by the GPU at most a few times.
	//			GL_STATIC_DRAW: the data is set only once, and used many times.
	//			GL_DYNAMIC_DRAW : the data is changed a lot, and used many times.
	glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(float), &attributes[0], GL_STATIC_DRAW);

	// Next, we bind our index array in the same way as our VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// ------------- TEXTURES ----------------
	glGenTextures(1, &texture); // Takes in how many textures are required, stores them in an unsigned int array
	glActiveTexture(GL_TEXTURE0); // Activate the texture unit before binding it. Default is 0.
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set how textures will be wrapped if a vertex falls outside the given coordinates
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set how texels are interpolated when scaling the image up or down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST); // Textures downscaled
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Textures upscaled

	// Fill the data by passing references into the stbi_load functions.
	int imgWidth, imgHeight, nrChannels;
	stbi_set_flip_vertically_on_load(false); // accounts for conversion between 1.0y and 0.0y to prevent upside-down textures.
	unsigned char* textureData = stbi_load(texPath, &imgWidth, &imgHeight, &nrChannels, 0);

	if (textureData)
	{
		// This function call applies the image to the currently bound texture object.
		// After the image is loaded, we generate the mipmaps to account for distant objects.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture at " << texPath << std::endl;
	}

	// Once we've generated the texture and mipmaps, we free the image memory
	stbi_image_free(textureData);

	// 2. OpenGL does not yet know how it should interpret the vertex data in memory.
	//		Now we define how it should connect the vertex data to the vertex shader's attributes
	// ---
	//		The first parameter specifies which vertex attribute we want to configure. Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0). This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
	//		The next argument specifies the size of the vertex attribute.The vertex attribute is a vec3 so it is composed of 3 values.
	//		The third argument specifies the type of the data which is GL_FLOAT(a vec * in GLSL consists of floating point values).
	//		The next argument specifies if we want the data to be normalized.If we're inputting integer data types (int, byte) and we've set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float.This is not relevant for us so we'll leave this at GL_FALSE.
	//		The fifth argument is known as the stride and tells us the space between consecutive vertex attributes. Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride.Note that since we know that the array is tightly packed(there is no space between the next vertex attribute value) we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed). Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute.
	//		The last parameter is of type void*and thus requires that weird cast.This is the offset of where the position data begins in the buffer.Since the position data is at the start of the data array this value is just 0. We will explore this parameter in more detail later on
	// ---

	// Here, I'm defining the attributes as variables to clearly label HOW these values are being used below.
	int positionAttributeID = 0;
	int posAttributeSize = 3; // 3 - x, y, z.
	int posOffset = 0;

	int colorAttributeID = 1;
	int colAttributeSize = 3; // 3 - r, g, b. Could be 4 if 'a' is also needed
	int colOffset = posAttributeSize;

	int textureAttributeID = 2;
	int texAttributeSize = 2; // 2 - Only need a 2D coord for UV's
	int texOffset = (posAttributeSize + colAttributeSize);

	int attributeOffset = (posAttributeSize + colAttributeSize + texAttributeSize); // The sum of all attribute sizes

	// Vertex Position Attribute
	glVertexAttribPointer(positionAttributeID, posAttributeSize, GL_FLOAT, GL_FALSE, attributeOffset * sizeof(float), (void*)posOffset); // 0 is the offset.
	glEnableVertexAttribArray(positionAttributeID);

	// Vertex Normal Attribute
	glVertexAttribPointer(colorAttributeID, colAttributeSize, GL_FLOAT, GL_FALSE, attributeOffset * sizeof(float), (void*)(colOffset * sizeof(float))); // Start at the END of position attribute
	glEnableVertexAttribArray(colorAttributeID);

	// Texture Coordinate Attribute
	glVertexAttribPointer(textureAttributeID, texAttributeSize, GL_FLOAT, GL_FALSE, attributeOffset * sizeof(float), (void*)(texOffset * sizeof(float)));
	glEnableVertexAttribArray(textureAttributeID);

	vao = VAO;
	vbo = VBO;
	ebo = EBO;
	transformation_matrix = glm::mat4(1.0);
	shader_program = Shader(vertPath, fragPath);

#if TEST_MODEL3D
	//cout << endl << "<<<<<<<<<<<<<<<<<<< RENDER SYSTEM TEST END >>>>>>>>>>>>>>>>>>>>>" << endl << endl;
#endif
}

void Model3D::translate(glm::vec3 translation) {
	transformation_matrix = glm::translate(transformation_matrix, translation);
}

void Model3D::rotate(glm::vec3 rotationAxis, float degrees) {
	transformation_matrix = glm::rotate(transformation_matrix, degrees, rotationAxis);
}

void Model3D::scale(glm::vec3 scale) {
	transformation_matrix = glm::scale(transformation_matrix, scale);
}

void Model3D::resetModelMatrix() {
	transformation_matrix = glm::mat4(1.0f);
}

glm::mat4 Model3D::getModelMatrix()
{
	return transformation_matrix;
}