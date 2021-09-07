#include "model.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

Model::Model(std::vector<float>& _positions, std::vector<float>& _normals, std::vector<float>& _uvs, std::vector<unsigned int>& _indices, Material* _m)
    :material(_m)
{
    std::vector<float> vbData(_positions.size() + _normals.size() + _uvs.size());
    int stride = 3 + 3 + 3 + 2;
    for (int i = 0; i < _positions.size() / 3; i++)
    {
        for(int j = 0; j < 3; j++){
            vbData[i * stride + j] = _positions[i * 3 + j];
        }
        for(int j = 0; j < 6; j++){
            vbData[i * stride + 3 + j] = _normals[i * 6 + j];
        }
        for(int j = 0; j < 2; j++){
            vbData[i * stride + 9 + j] = _uvs[i * 2 + j];
        }
    }
    
    VertexBuffer vb(&vbData[0], vbData.size() * sizeof(float));
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    va.AddBuffer(vb, layout);

    ib.FillBuffer(&_indices[0], _indices.size());

    va.Unbind();
    vb.Unbind();
    ib.Unbind();
}

Material* Model::getMaterial() 
{
    return material;
}

void Model::setMaterial(Material* _material) 
{
    material = _material;
}

VertexArray& Model::getVertexArray() 
{
    return va;
}

IndexBuffer& Model::getIndexBuffer() 
{
    return ib;
}

Shader& Model::getShader() 
{
    return material->shader;
}

void Model::setUniforms() 
{
    assert(material != nullptr);
    material->shader.bind();

    if(material->PBR){
        material->shader.setInt("irradianceMap", 0);
        material->shader.setInt("prefilteredMap", 1);
        material->shader.setInt("brdfLUT", 2);
        material->shader.setInt("material.albedo", 3);
        material->shader.setInt("material.normal", 4);
        material->shader.setInt("material.roughness", 5);
        material->shader.setInt("material.metallic", 6);
        material->shader.setInt("material.ao", 7);
        material->shader.setInt("ambientEnabled", material->ambientEnabled);

        GLCall(glActiveTexture(GL_TEXTURE3));
        GLCall(glBindTexture(GL_TEXTURE_2D, material->albedo));
        GLCall(glActiveTexture(GL_TEXTURE4));
        GLCall(glBindTexture(GL_TEXTURE_2D, material->normalMap));
        GLCall(glActiveTexture(GL_TEXTURE5));
        GLCall(glBindTexture(GL_TEXTURE_2D, material->roughness));
        GLCall(glActiveTexture(GL_TEXTURE6));
        GLCall(glBindTexture(GL_TEXTURE_2D, material->metallic));
        GLCall(glActiveTexture(GL_TEXTURE7));
        GLCall(glBindTexture(GL_TEXTURE_2D, material->ao));
    }
    else{
        material->shader.setInt("material.diffuse", 3);
        material->shader.setInt("material.specular", 4);
        material->shader.setInt("material.normal", 5);
        material->shader.setInt("material.roughness", 6);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, material->diffuse);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, material->specular);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, material->normalMap);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, material->roughness);
    }
}

Material::Material(Shader _shader, bool _PBR, const std::string& _name) 
    : shader(_shader), PBR(_PBR), name(_name)
{
    shader.bind();
}

void Material::setPhongMaterial(const std::string& diffusePath, const std::string& specularPath, const std::string& normalMapPath, const std::string& roughnessPath) 
{
    loadTextureSRGB(diffusePath, diffuse);
    
    loadTextureSRGB(specularPath, specular);
    
    loadTexture(normalMapPath, normalMap);

    loadTexture(roughnessPath, roughness);
}

void Material::setPBRMaterial(const std::string& albedoPath, const std::string& normalMapPath, 
                              const std::string& metallicPath, const std::string& roughnessPath, const std::string& aoPath) 
{

    loadTextureSRGB(albedoPath, albedo);

    loadTexture(normalMapPath, normalMap);

    loadTexture(roughnessPath, roughness);

    loadTexture(metallicPath, metallic);

    loadTexture(aoPath, ao);
}

void Material::loadTexture(const std::string& path, GLuint& buffer) 
{
    GLCall(glGenTextures(1, &buffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, buffer));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if(data){
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else{
        std::cerr << "Cannot read image from: " << path << "\n";
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Material::loadTextureSRGB(const std::string& path, GLuint& buffer) 
{
    GLCall(glGenTextures(1, &buffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, buffer));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));	
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if(data){
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else{
        std::cerr << "Cannot read image from: " << path << "\n";
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}
