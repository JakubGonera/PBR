#pragma once

#include <string>
#include <map>
#include <iostream>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "Application/utils.h"

class Shader{
    unsigned int id;
    std::map<std::string, int> locations;
public:
    Shader(const std::string& vs, const std::string& fs){
        id = xe::create_program(std::string(PROJECT_DIR) + vs,
                                      std::string(PROJECT_DIR) + fs);
        if (!id) {
            std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + vs << " and ";
            std::cerr << std::string(PROJECT_DIR) + fs << " shader files" << std::endl;
        }
    }
    void bind(){
        GLCall(glUseProgram(id));
    }
    void setInt(const std::string& name, int val){
        if(locations[name] == 0){
            GLCall(locations[name] = glGetUniformLocation(id, name.c_str()) + 1);
        }
        GLCall(glUniform1i(locations[name] - 1, val));
    }
    void setFloat(const std::string& name, float val){
        if(locations[name] == 0){
            GLCall(locations[name] = glGetUniformLocation(id, name.c_str()) + 1);
        }
        GLCall(glUniform1f(locations[name] - 1, val));
    }
    void setMat4(const std::string& name, glm::mat4 val){
        if(locations[name] == 0){
            GLCall(locations[name] = glGetUniformLocation(id, name.c_str()) + 1);
        }
        GLCall(glUniformMatrix4fv(locations[name] - 1, 1, GL_FALSE, &val[0][0]));
    }
    unsigned int getID(){
        return id;
    }
};