#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "glad/glad.h"
#include "glm/glm.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
private:
    unsigned int ID;
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    void Use();
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetMat4(const std::string &name, glm::mat4 value) const;
    void SetVec3(const std::string &name, glm::vec3 value) const;
    void SetVec3(const std::string &name, float x, float y, float z) const;
    unsigned int GetId() const;
};

#endif //OPENGL_SHADER_H
