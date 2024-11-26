class Shader {
	public:
		Shader(const char *vertexPath, const char *fragmentPath);
		~Shader();
		void Bind() const;
		void Unbind() const;
		void SetUniform1i(const char *name, int value);
		void SetUniform1f(const char *name, float value);
		void SetUniform2f(const char *name, const glm::vec2 &value);
		void SetUniform3f(const char *name, const glm::vec3 &value);
		void SetUniform4f(const char *name, const glm::vec4 &value);
		void SetUniformMat4f(const char *name, const glm::mat4 &value);
	private:
		unsigned int renderer_id;
		int GetUniformLocation(const char *name);
		const char* ParseShader(const char *filePath);
};
