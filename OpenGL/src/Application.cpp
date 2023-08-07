#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}


int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create a GLFW window!" << '\n';
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Creating and binding vertices
	float vertices[32] = {	
	// positions          // colors           // texture coords
	  0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,		// top right
	  0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,		// bottom right
	 -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,		// bottom left
	 -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f		// top left
	};

	unsigned int indices[6] = {
		1, 3, 0, // first triangle
		1, 2, 3  // second triangle
	};

	unsigned int EBO;
	unsigned int VBO;
	unsigned int VAO;
	unsigned int texture1;
	unsigned int texture2;

	glGenTextures(1, &texture1);
	glGenTextures(1, &texture2);
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Creating and compiling shaders
	Shader ShaderObject("res/shaders/Vertex.shader", "res/shaders/Fragment.shader");
	ShaderObject.compile_link_shader_and_check_success();

	// Telling openGL how to interpret the vertex data
	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	stbi_set_flip_vertically_on_load(true);

	// Texture config
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Load box texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("res/textures/good_container.JPG", &width, &height, &nrChannels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Textured loaded!" << '\n';
	}
	else
	{
		std::cout << "Failed to load texture: " << stbi_failure_reason() << '\n';
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Load awesome face texture
	int width1, height1, nrChannels1;
	unsigned char* data1 = stbi_load("res/textures/awesomeface.png", &width1, &height1, &nrChannels1, 4);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width1, height1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Texture loaded!" << '\n';
	}
	else
	{
		std::cout << "Failed to load texture: " << stbi_failure_reason() << '\n';
	}
	stbi_image_free(data1);

	float transparent_factor = 0.25;

	ShaderObject.use();
	glUniform1i(glGetUniformLocation(ShaderObject.ID, "texture1"), 0);
	glUniform1i(glGetUniformLocation(ShaderObject.ID, "texture2"), 1);
	glUniform1f(glGetUniformLocation(ShaderObject.ID, "transparency"), transparent_factor);

	/*ShaderObject.use();
	int uniformLocation = glGetUniformLocation(ShaderObject.ID, "ourTexture");
	glUniform1i(uniformLocation, 0);*/

	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 transformation = glm::mat4(1.0f);

	transformation = glm::translate(transformation, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = transformation * vec;
	std::cout << vec.x << ', ' << vec.y << ',' << vec.z << ',' << vec.w << '\n';

	while (!glfwWindowShouldClose(window))
	{	
		// Input
		process_input(window);
		
		// Rendering commands
		glClearColor(0.3f, 0.3f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (transparent_factor <= 1.0))
		{
			std::cout << "up arrow" << '\n';
			transparent_factor += 0.05;
		}
		
		if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (transparent_factor >= 0.0))
		{
			std::cout << "down arrow: " << transparent_factor << '\n';
			transparent_factor -= 0.05;
		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glUniform1f(glGetUniformLocation(ShaderObject.ID, "transparency"), transparent_factor);

		glBindVertexArray(VAO);
		ShaderObject.use();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
