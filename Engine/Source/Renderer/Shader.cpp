#include "Shader.h"

#include "../platform.h"

#include <glad/glad.h>

#include <iostream>
#include <fstream>

Shader::Shader(const std::string& filePath, const std::string& OldShaderName)
	: path(filePath), name(OldShaderName)
{
	source = ReadFile();
	ConstructShader();
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::Bind()
{
    glUseProgram(id);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetUniformBool(const std::string& name, bool b)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform1f(location, b);
}

void Shader::SetUniform1i(const std::string& name, int i)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform1i(location, i);
}

void Shader::SetUniform1f(const std::string& name, float f)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform1f(location, f);
}

void Shader::SetUniformVec2f(const std::string& name, float v0, float v1)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform2f(location, v0, v1);
}

void Shader::SetUniformVec2f(const std::string& name, glm::vec2 v)
{
    SetUniformVec2f(name, v.x, v.y);
}

void Shader::SetUniformVec3f(const std::string& name, float v0, float v1, float v2)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform3f(location, v0, v1, v2);
}

void Shader::SetUniformVec3f(const std::string& name, glm::vec3 v)
{
    SetUniformVec3f(name, v.x, v.y, v.z);
}

void Shader::SetUniformVec4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniform4f(location, v0, v1, v2, v3);
}

void Shader::SetUniformVec4f(const std::string& name, glm::vec4 v)
{
    SetUniformVec4f(name, v.x, v.y, v.z, v.w);
}

void Shader::SetUniformMatrix3f(const std::string& name, const glm::mat3x3& mat)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetUniformMatrix4f(const std::string& name, const glm::mat4x4& mat)
{
    GLint location = glGetUniformLocation(id, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

std::string Shader::ReadFile()
{
	std::string ret;
	std::ifstream in(path, std::ios::in, std::ios::binary);

	if (in)
	{
		in.seekg(0, std::ios::end);
		ret.resize(in.tellg());

		in.seekg(0, std::ios::beg);
		in.read(&ret[0], ret.size());

		in.close();
	}
	else
	{
		std::cout << "Could not locate the OldShader: " << path << std::endl;
	}

	return ret;
}

void Shader::ConstructShader()
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 430\n";
    char OldShaderNameDefine[128];
    sprintf(OldShaderNameDefine, "#define %s\n", name.c_str());
    char vertexOldShaderDefine[] = "#define VERTEX\n";
    char fragmentOldShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        OldShaderNameDefine,
        vertexOldShaderDefine,
        source.c_str()
    };
    const GLint vertexShaderLengths[] = {
        (GLint)strlen(versionString),
        (GLint)strlen(OldShaderNameDefine),
        (GLint)strlen(vertexOldShaderDefine),
        (GLint)source.size()
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        OldShaderNameDefine,
        fragmentOldShaderDefine,
        source.c_str()
    };
    const GLint fragmentShaderLengths[] = {
        (GLint)strlen(versionString),
        (GLint)strlen(OldShaderNameDefine),
        (GLint)strlen(fragmentOldShaderDefine),
        (GLint)source.size()
    };

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vShader);
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vShader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        std::cout << "glCompileShader() failed with vertex Shader " << name << "\n\tReported message : " << infoLogBuffer << std::endl;
    }

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fShader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        std::cout << "glCompileShader() failed with fragment Shader " << name << "\n\tReported message : " << infoLogBuffer << std::endl;        
    }

    id = glCreateProgram();
    glAttachShader(id, vShader);
    glAttachShader(id, fShader);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(id, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        std::cout << "glLinkProgram() failed with Shader " << name << "\n\tReported message : " << infoLogBuffer << std::endl;
    }

    glUseProgram(0);

    glDetachShader(id, vShader);
    glDetachShader(id, fShader);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    std::cout << name << " OldShader loaded successfully." << std::endl;

}
