#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    GLuint vert = compile(vertPath, GL_VERTEX_SHADER);
    GLuint frag = compile(fragPath, GL_FRAGMENT_SHADER);

    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);

    GLint ok;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(id, 512, nullptr, log);
        std::cerr << "Shader link error:\n" << log << "\n";
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

Shader::~Shader() {
    if (id) glDeleteProgram(id);
}

void Shader::use() const { glUseProgram(id); }

GLuint Shader::compile(const std::string& path, GLenum type) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Cannot open shader: " << path << "\n";
        return 0;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string src  = ss.str();
    const char* csrc = src.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error (" << path << "):\n" << log << "\n";
    }
    return shader;
}

GLint Shader::loc(const std::string& name) const {
    return glGetUniformLocation(id, name.c_str());
}

void Shader::setBool (const std::string& n, bool v)       const { glUniform1i(loc(n), (int)v); }
void Shader::setInt  (const std::string& n, int v)        const { glUniform1i(loc(n), v); }
void Shader::setFloat(const std::string& n, float v)      const { glUniform1f(loc(n), v); }
void Shader::setVec3 (const std::string& n, glm::vec3 v)  const { glUniform3fv(loc(n), 1, glm::value_ptr(v)); }
void Shader::setMat3 (const std::string& n, glm::mat3 m)  const { glUniformMatrix3fv(loc(n), 1, GL_FALSE, glm::value_ptr(m)); }
void Shader::setMat4 (const std::string& n, glm::mat4 m)  const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, glm::value_ptr(m)); }
