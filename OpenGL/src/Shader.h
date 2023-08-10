#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader 
{
public:
	// Shader Program ID
	unsigned int ID;
	std::string vShaderCode;
	std::string fShaderCode;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);

	void compile_link_shader_and_check_success();

	// use/activate shader
	void use();

	// utility uniform functions
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, float x, float y, float z);
};

#endif
