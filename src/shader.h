#pragma once
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint id = 0;

    Shader() = default;
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    void use() const;

    void setBool (const std::string& name, bool v)        const;
    void setInt  (const std::string& name, int v)         const;
    void setFloat(const std::string& name, float v)       const;
    void setVec3 (const std::string& name, glm::vec3 v)   const;
    void setMat3 (const std::string& name, glm::mat3 m)   const;
    void setMat4 (const std::string& name, glm::mat4 m)   const;

private:
    static GLuint compile(const std::string& path, GLenum type);
    GLint loc(const std::string& name) const;
};
