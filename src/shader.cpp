#include "shader.h"
#include "glm/gtc/type_ptr.hpp"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vxShader, fgShader;
    vxShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vxShader, 1, &vShaderCode, NULL);
    glCompileShader(vxShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vxShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vxShader, 512, NULL, infoLog);
        std::cerr << "failed to compile vertex shader:\n " << infoLog << std::endl;
    }

    fgShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fgShader, 1, &fShaderCode, NULL);
    glCompileShader(fgShader);
    glGetShaderiv(fgShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fgShader, 512, NULL, infoLog);
        std::cerr << "failed to compile fragment shader:\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vxShader);
    glAttachShader(ID, fgShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "failed to compile shader program:\n " << infoLog << std::endl;
    }

    glDeleteShader(vxShader);
    glDeleteShader(fgShader);
}

Shader::Shader(const char* vertexPath, const char *geometryPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        gShaderFile.open(geometryPath);
        std::stringstream vShaderStream, fShaderStream, gShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        gShaderStream << gShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        gShaderFile.close();
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometryCode = gShaderStream.str();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();

    unsigned int vxShader, fgShader, gShader;
    vxShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vxShader, 1, &vShaderCode, NULL);
    glCompileShader(vxShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vxShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vxShader, 512, NULL, infoLog);
        std::cerr << "failed to compile vertex shader:\n " << infoLog << std::endl;
    }

    fgShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fgShader, 1, &fShaderCode, NULL);
    glCompileShader(fgShader);
    glGetShaderiv(fgShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fgShader, 512, NULL, infoLog);
        std::cerr << "failed to compile fragment shader:\n" << infoLog << std::endl;
    }

    gShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(gShader, 1, &gShaderCode, NULL);
    glCompileShader(gShader);
    glGetShaderiv(gShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(gShader, 512, NULL, infoLog);
        std::cerr << "failed to compile geometry shader:\n" << infoLog << std::endl;
    }

    ID = glCreateProgram();
    glAttachShader(ID, vxShader);
    glAttachShader(ID, fgShader);
    glAttachShader(ID, gShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cerr << "failed to compile shader program:\n " << infoLog << std::endl;
    }

    glDeleteShader(vxShader);
    glDeleteShader(fgShader);
    glDeleteShader(gShader);
}

void Shader::Use() {
    glUseProgram(ID);
}

void Shader::SetBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), int(value));
}

void Shader::SetInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}


void Shader::SetMat4(const std::string &name, glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}


void Shader::SetVec3(const std::string &name, glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

unsigned int Shader::GetId() const {
    return ID;
}

void Shader::SetVec3(const std::string &name, float x, float y, float z) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(glm::vec3(x, y, z)));
}
