#include "Shader.h"

#include <glad/glad.h>

#include <stdio.h>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	const char *vertexSource = ParseShader(vertexPath);
	const char *fragmentSource = ParseShader(fragmentPath);
	const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);
	int success = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512] = {0};
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		printf("Vertex shader compilation failed! %s\n", infoLog);
		glDeleteShader(vertexShader);
		return;
	}
	const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	success = 0;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512] = {0};
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		printf("Fragment shader compilation failed! %s\n", infoLog);
		glDeleteShader(fragmentShader);
		return;
	}
	const unsigned int program = glCreateProgram();
	renderer_id = program;

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	int isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (!isLinked) {
		char infoLog[512] = {0};
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		printf("Shader program linking failed! %s\n", infoLog);
		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return;
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);
}

Shader::~Shader() {
	glDeleteProgram(renderer_id);
}

void Shader::Bind() const {
	glUseProgram(renderer_id);
}

void Shader::Unbind() const {
	glUseProgram(0);
}

void Shader::SetUniform1f(const char *name, float value) {
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1i(const char *name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform2f(const char* name, const glm::vec2& value) {
	glUniform2f(GetUniformLocation(name), value.x, value.y);
}

void Shader::SetUniform3f(const char* name, const glm::vec3& value) {
	glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
}

void Shader::SetUniform4f(const char* name, const glm::vec4& value) {
	glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::SetUniformMat4f(const char* name, const glm::mat4& value) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

// could be improved with caching (e.g. a hash table)
int Shader::GetUniformLocation(const char* name) {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}
	m_UniformLocationCache[name] = glGetUniformLocation(renderer_id, name);
	return m_UniformLocationCache[name];
}

const char* Shader::ParseShader(const char* filePath) {
	FILE* file = fopen(filePath, "r");
	if (!file) {
		printf("Failed to open file %s\n", filePath);
		return nullptr;
	}
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* data = new char[length + 1];
	fread(data, 1, length, file);
	data[length] = '\0';
	fclose(file);
	return data;
}

