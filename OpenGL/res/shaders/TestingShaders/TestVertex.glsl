#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Position;
out vec3 Normal;

uniform mat4 transformation;
uniform mat4 model;

void main()
{
	gl_Position = transformation * model * vec4(aPos, 1.0f);
	Normal = mat3(transpose(inverse(model))) * aNormal;
	Position = vec3(model * vec4(aPos, 1.0));
}
