#pragma once

#include <iostream>

#include "glad/glad.h"
#include "glm/glm.hpp"

#include "vertexArray.h"
#include "buffer.h"
#include "shader.h"

#include "stb/stb_image.h"


class Skybox{
    Shader shader;
    Shader conversionShader;
    Shader irradianceShader;
    Shader prefilteredShader;
    Shader brdfShader;

    GLuint texture;
    GLuint origEquirectangularMap;
    GLuint irradianceMap;
    GLuint prefilteredMap;
    GLuint brdfLUTTex;

    VertexArray* cubeVAO = nullptr;
    
    Shader quadProgram;
    VertexArray* quadVAO = nullptr;

    void renderCube();

    void genSkybox();
public:
    Skybox(Shader _shader, const std::string& map);

    void renderQuadTexture();
    
    void setUniforms(glm::mat4 _view, glm::mat4 _projection);

    void bindEnvTextures();
    
    VertexArray* getVertexArray() {return cubeVAO;}
    Shader& getShader(){ return shader;}
    Shader& getQuadShader(){ return quadProgram;}
    GLuint getTexture(){ return texture;}
};