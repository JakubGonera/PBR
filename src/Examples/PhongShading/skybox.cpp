#include "skybox.h"
#include "glm/gtc/matrix_transform.hpp"

Skybox::Skybox(Shader _shader, const std::string& map)
        :shader(_shader), conversionShader("/shaders/cubemap_vs.glsl", "/shaders/cubemap_fs.glsl"), 
        irradianceShader("/shaders/cubemap_vs.glsl", "/shaders/convolution_fs.glsl"),
        prefilteredShader("/shaders/cubemap_vs.glsl", "/shaders/prefiltered_fs.glsl"),
        brdfShader("/shaders/brdfConvolutedTex_vs.glsl", "/shaders/brdfConvolutedTex_fs.glsl"),
        quadProgram("/shaders/quad_vs.glsl", "/shaders/quad_fs.glsl")
    {
        glGenTextures(1, &origEquirectangularMap);
        glBindTexture(GL_TEXTURE_2D, origEquirectangularMap);

        stbi_set_flip_vertically_on_load(true);
        int width, height, nrChannels;
        float *data;  
        data = stbi_loadf(map.c_str(), &width, &height, &nrChannels, 0);
        if(data){
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data));
        }
        else{
            std::cout << "Failed to load texture at: " << map << '\n';
        }
        stbi_image_free(data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        genSkybox();
    }

void Skybox::renderCube(){
    // initialize (if necessary)
    if (cubeVAO == nullptr)
    {
        cubeVAO = new VertexArray();
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,
            1.0f,  1.0f, -1.0f,  
            1.0f, -1.0f, -1.0f,           
            1.0f,  1.0f, -1.0f,  
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            // front face
            -1.0f, -1.0f,  1.0f, 
            1.0f, -1.0f,  1.0f, 
            1.0f,  1.0f,  1.0f, 
            1.0f,  1.0f,  1.0f, 
            -1.0f,  1.0f,  1.0f, 
            -1.0f, -1.0f,  1.0f, 
            // left face
            -1.0f,  1.0f,  1.0f, 
            -1.0f,  1.0f, -1.0f, 
            -1.0f, -1.0f, -1.0f, 
            -1.0f, -1.0f, -1.0f, 
            -1.0f, -1.0f,  1.0f, 
            -1.0f,  1.0f,  1.0f, 
            // right face
            1.0f,  1.0f,  1.0f,  
            1.0f, -1.0f, -1.0f,  
            1.0f,  1.0f, -1.0f, 
            1.0f, -1.0f, -1.0f,  
            1.0f,  1.0f,  1.0f,  
            1.0f, -1.0f,  1.0f,      
            // bottom face
            -1.0f, -1.0f, -1.0f, 
            1.0f, -1.0f, -1.0f, 
            1.0f, -1.0f,  1.0f, 
            1.0f, -1.0f,  1.0f, 
            -1.0f, -1.0f,  1.0f, 
            -1.0f, -1.0f, -1.0f, 
            // top face
            -1.0f,  1.0f, -1.0f, 
            1.0f,  1.0f , 1.0f, 
            1.0f,  1.0f, -1.0f, 
            1.0f,  1.0f,  1.0f, 
            -1.0f,  1.0f, -1.0f, 
            -1.0f,  1.0f,  1.0f
        };
        VertexBuffer vb(&vertices[0], sizeof(vertices));
        VertexBufferLayout layout;
        layout.Push<float>(3);

        cubeVAO->AddBuffer(vb, layout);
        vb.Unbind();
        cubeVAO->Unbind();
    }
    // render Cube
    cubeVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeVAO->Unbind();
}

void Skybox::genSkybox(){
    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
                    1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = 
    {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    // convert HDR equirectangular environment map to cubemap equivalent
    conversionShader.bind();
    conversionShader.setInt("rectMap", 0);
    conversionShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, origEquirectangularMap);

    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        conversionShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube(); 
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Generate irridance map
    GLCall(glGenTextures(1, &irradianceMap));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap));
    for (int i = 0; i < 6; i++)
    {
        GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr));
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

    irradianceShader.bind();
    irradianceShader.setInt("environmentMap", 0);
    irradianceShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        irradianceShader.setMat4("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Generate prefiltered environment map
    GLCall(glGenTextures(1, &prefilteredMap));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap));

    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    prefilteredShader.bind();
    prefilteredShader.setInt("enviromentMap", 0);
    prefilteredShader.setMat4("projection", captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        unsigned int mipWidth  = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilteredShader.setFloat("roughness", roughness);
        for (unsigned int i = 0; i < 6; ++i)
        {
            prefilteredShader.setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilteredMap, mip);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLCall(glGenTextures(1, &brdfLUTTex));
    GLCall(glBindTexture(GL_TEXTURE_2D, brdfLUTTex));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, captureFBO));
    GLCall(glBindRenderbuffer(GL_RENDERBUFFER, captureRBO));
    GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512));
    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTex, 0));
    GLCall(glViewport(0, 0, 512, 512));
    brdfShader.bind();
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    renderQuadTexture();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Skybox::renderQuadTexture(){
    if(quadVAO == nullptr){
        quadVAO = new VertexArray();
        std::vector<float> quadVertices = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        VertexBuffer vb(&quadVertices[0], quadVertices.size() * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);

        quadVAO->AddBuffer(vb, layout);

        quadVAO->Unbind();
        vb.Unbind();
    }
    quadVAO->Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    quadVAO->Unbind();
}

void Skybox::setUniforms(glm::mat4 _view, glm::mat4 _projection){
    shader.setInt("hdrBuffer", 0);
    shader.setMat4("view", _view);
    shader.setMat4("projection", _projection);
}

void Skybox::bindEnvTextures(){
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap));
    GLCall(glActiveTexture(GL_TEXTURE1));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, prefilteredMap));
    GLCall(glActiveTexture(GL_TEXTURE2));
    GLCall(glBindTexture(GL_TEXTURE_2D, brdfLUTTex));
}