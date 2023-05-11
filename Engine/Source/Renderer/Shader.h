#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
	Shader(const std::string& filePath, const std::string& shaderName);
	virtual ~Shader();

	void Bind();
	void Unbind();

	inline unsigned int GetId() { return id; }


	void SetUniformBool(const std::string& name, bool b);

	void SetUniform1i(const std::string& name, int i);
	void SetUniform1f(const std::string& name, float f);

	void SetUniformVec2f(const std::string& name, float v0, float v1);
	void SetUniformVec2f(const std::string& name, glm::vec2 v);

	void SetUniformVec3f(const std::string& name, float v0, float v1, float v2);
	void SetUniformVec3f(const std::string& name, glm::vec3 v);

	void SetUniformVec4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformVec4f(const std::string& name, glm::vec4 v);

	void SetUniformMatrix3f(const std::string& name, const glm::mat3x3& mat);
	void SetUniformMatrix4f(const std::string& name, const glm::mat4x4& mat);


private:
	std::string ReadFile();
	void ConstructShader();

private:
	unsigned int id;
	std::string source;

	std::string name;
	std::string path;
};