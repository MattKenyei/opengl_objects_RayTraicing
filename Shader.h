#pragma once

#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setFloatVec(const std::string& name, float* vec, int vec_size) const;
    void SetMatrix4F(const std::string& name, glm::mat4& m) const;
    unsigned int ID();

private:
    unsigned int programID;
    void checkCompileErrors(unsigned int shader, std::string type);
};