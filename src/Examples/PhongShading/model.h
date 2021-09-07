#pragma once

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "Application/utils.h"

#include "vertexArray.h"
#include "shader.h"

#include <vector>
#include <string>


struct Material{
    std::string name;

    GLuint diffuse;
    GLuint specular;
    GLuint normalMap;

    Shader shader;

    GLuint roughness;
    GLuint albedo;
    GLuint metallic;
    GLuint ao;
    int ambientEnabled = 1.f;

    bool PBR;

    Material(Shader _shader, bool PBR, const std::string& _name);
    void setPhongMaterial(const std::string& diffusePath, const std::string& specularPath, const std::string& normalMapPath, const std::string& roughness);
    void setPBRMaterial(const std::string& albedoPath, const std::string& normalMapPath, const std::string& metallicPath, 
                        const std::string& roughnessPath, const std::string& aoPath);
    void loadTexture(const std::string& path, GLuint& buffer);
    void loadTextureSRGB(const std::string& path, GLuint& buffer);
};
class Model{
private:
    Material* material = nullptr;
    VertexArray va;
    IndexBuffer ib;
public:
    Model(std::vector<float>& _positions, std::vector<float>& _normals, std::vector<float>& _uvs, std::vector<unsigned int>& _indices, Material* _m);
    Material* getMaterial();
    void setMaterial(Material* material);
    VertexArray& getVertexArray();
    IndexBuffer& getIndexBuffer();
    Shader& getShader();
    void setUniforms();
};