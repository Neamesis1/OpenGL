#include <iostream>
#include <filesystem>
#include <map>

#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#define NOMINMAX
#include <Windows.h>

#include <assimp/material.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


//void print_to_output(const char* szFormat)
//{
//	char szBuff[1024];
//	va_list arg;
//	va_start(arg, szFormat);
//	_vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
//	va_end(arg);
//
//	LPCWSTR A = szBuff;
//
//	OutputDebugString(szBuff);
//}


int screen_width = 800;
int screen_height = 600;

float aspect_ratio = (float)screen_width / (float)screen_height;


float lastX = (float)screen_width / 2, lastY = (float)screen_height / 2;
bool firstMouse = true;
float cameraSpeed = CAMERA_SPEED;

// timing
float dT = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 12.0f);

glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

glm::vec3 worldSpaceUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(worldSpaceUp, cameraDirection));
glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));

Camera CameraObject(cameraPos, worldSpaceUp);
Camera_Movement direction;


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


void camera_move(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		CameraObject.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		CameraObject.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		CameraObject.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		CameraObject.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		CameraObject.ProcessKeyboard(UP, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		CameraObject.ProcessKeyboard(DOWN, deltaTime);
	}
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = (float)xposIn;
	float ypos = (float)yposIn;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	CameraObject.ProcessMouse(xOffset, yOffset);

}


void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	CameraObject.ProcessMouseScroll((float)yOffset);
}


void drawTwocubes(unsigned int VAO, Shader shader, unsigned int texture, glm::mat4 view, glm::mat4 projection, glm::vec3 pos1, glm::vec3 pos2, float scale)
{
	// Draw call for two cubes
	glBindVertexArray(VAO);
	shader.use();

	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// first cube
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos1);
	model = glm::scale(model, glm::vec3(scale));
	glm::mat4 final_matrix = projection * view;
	shader.setMat4("transformation", final_matrix);
	shader.setMat4("model", model);
	shader.setVec3("cameraPos", CameraObject.Position);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// second cube
	model = glm::mat4(1.0f);
	model = glm::translate(model, pos2);
	model = glm::scale(model, glm::vec3(scale));
	final_matrix = projection * view;
	shader.setMat4("transformation", final_matrix);
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void create_VBO_VAO(unsigned int* VBO, unsigned int* VAO, const unsigned int& sizeOfVertexArray, float* VertexArray, const unsigned int& sizeOfVertex,
	const unsigned int& sizeOfPosition, const unsigned int& sizeOfTexture = 0, const unsigned int& sizeOfNormal = 0)
{
	glGenBuffers(1, VBO);
	glGenVertexArrays(1, VAO);

	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeOfVertexArray, VertexArray, GL_STATIC_DRAW);

	// Vertex positions
	glVertexAttribPointer(0, sizeOfPosition, GL_FLOAT, GL_FALSE, sizeOfVertex * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	if (sizeOfTexture != 0)
	{
		// Vertex Texture coords
		glVertexAttribPointer(1, sizeOfTexture, GL_FLOAT, GL_FALSE, sizeOfVertex * sizeof(float), (void*)(sizeOfPosition * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	if (sizeOfNormal != 0)
	{
		// Vertex Texture coords
		glVertexAttribPointer(1, sizeOfNormal, GL_FLOAT, GL_FALSE, sizeOfVertex * sizeof(float), (void*)((sizeOfPosition + sizeOfTexture) * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
}


unsigned int loadCubemap(std::vector<std::string> face_path)
{
	unsigned int cubemapID;
	glGenTextures(1, &cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	int width, height, nrChannels;
	for (int i = 0; i < face_path.size(); i++)
	{
		unsigned char* data = stbi_load(face_path[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			std::cout << "Cubemap texture " << i << " has loaded succesfully!" << "\n";
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load path: " << face_path[i] << " Reason: " << stbi_failure_reason() << '\n';
			stbi_image_free(data);
		}

	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubemapID;
}


int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
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

	glfwSwapInterval(0);

	glViewport(0, 0, screen_width, screen_height);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	// Create framebuffer
	unsigned int FBO;
	glGenFramebuffers(1, &FBO); // Try using glGenBuffers() instead
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Generate texture attachment and attach it
	unsigned int colorBufferTexture;
	glGenTextures(1, &colorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, colorBufferTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach color attachment to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferTexture, 0);


	// Create render buffer object
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer is complete!\n";
	}
	else
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float quadVertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	float cubeVertices[] = {
	// Position			 //Normals
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// Vertex buffers
	unsigned int cubeVBO;
	unsigned int quadVBO;
	unsigned int skyboxVBO;

	// Vertex array objects
	unsigned int cubeVAO;
	unsigned int quadVAO;
	unsigned int skyboxVAO;

	create_VBO_VAO(&cubeVBO, &cubeVAO, sizeof(cubeVertices), cubeVertices, 6, 3, 0, 3);
	create_VBO_VAO(&quadVBO, &quadVAO, sizeof(quadVertices), quadVertices, 4, 2, 2);
	create_VBO_VAO(&skyboxVBO, &skyboxVAO, sizeof(skyboxVertices), skyboxVertices, 3, 3);

	std::vector<std::string> face_path =
	{
		"res/textures/CubeMapTexture/skybox/right.jpg",
		"res/textures/CubeMapTexture/skybox/left.jpg",
		"res/textures/CubeMapTexture/skybox/top.jpg",
		"res/textures/CubeMapTexture/skybox/bottom.jpg",
		"res/textures/CubeMapTexture/skybox/front.jpg",
		"res/textures/CubeMapTexture/skybox/back.jpg"
	};

	unsigned int cubeMapTexture = loadCubemap(face_path);

	// Creating and compiling shaders
	Shader TestShader("res/shaders/TestingShaders/TestVertex.glsl", "res/shaders/TestingShaders/TestFragment.glsl");
	TestShader.compile_link_shader_and_check_success();

	Shader QuadShader("res/shaders/TestingShaders/QuadVertex.glsl", "res/shaders/TestingShaders/QuadFragment.glsl");
	QuadShader.compile_link_shader_and_check_success();

	Shader SkyboxShader("res/shaders/TestingShaders/SkyboxVertex.glsl", "res/shaders/TestingShaders/SkyboxFragment.glsl");
	SkyboxShader.compile_link_shader_and_check_success();

	// Loading textures (texture loading function is a part of the model class)
	Model dummy("");

	unsigned int cube_texture;

	cube_texture = dummy.TextureFromFile("res/textures/good_container.JPG");

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Enabling depth, stencil testing
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Colors
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 toyColor(1.0f, 0.5f, 0.31f);
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	glm::mat4 final_matrix(1.0f);

	glm::vec3 cubePos1(1.0f, 0.001f, 0.0f);
	glm::vec3 cubePos2(-1.0f, 0.001f, -2.0f);
	glm::vec3 planePos(0.0f, 0.0f, 0.0f);


	while (!glfwWindowShouldClose(window))
	{	
		float currentFrame = static_cast<float>(glfwGetTime());
		dT = currentFrame - lastFrame;
		lastFrame = currentFrame;

		float fps = 1 / dT;
		OutputDebugString((L"fps: " + std::to_wstring(fps) + L"\n").c_str());

		// Input
		process_input(window);
		
		// Rendering commands
		glClearColor(0.1f, 0.1f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		camera_move(window, dT);
		
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);


		view = glm::mat4(1.0f);
		view = CameraObject.GetViewMatrix();

		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(CameraObject.Zoom), (float)(screen_width / screen_height), 0.1f, 100.0f);

		final_matrix = projection * view * model;

		// Binding our framebuffer and rendering the scene to a texture
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glClearColor(0.1f, 0.1f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Draw cubes
		drawTwocubes(cubeVAO, TestShader, cubeMapTexture, view, projection, cubePos1, cubePos2, 1.0f);

		// draw skybox
		glDepthFunc(GL_LEQUAL);
		SkyboxShader.use();
		view = glm::mat4(glm::mat3(CameraObject.GetViewMatrix()));
		SkyboxShader.setMat4("transform", projection * view);
		std::cout << (projection * view * model)[2][3] << '\n';
		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);

		// Drawing the texture onto a quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		
		glBindVertexArray(quadVAO);
		QuadShader.use();
		glBindTexture(GL_TEXTURE_2D, colorBufferTexture);
		QuadShader.setInt("screenTexture", 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Check and call events and swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}
