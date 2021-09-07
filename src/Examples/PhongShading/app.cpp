//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Application/utils.h"

void SimpleShapeApplication::init() {

    Shader program("/shaders/base_vs.glsl", "/shaders/base_fs.glsl");
    Shader pbrProgram("/shaders/base_vs.glsl", "/shaders/pbr_fs.glsl");
    Shader skyboxProgram("/shaders/skybox_vs.glsl", "/shaders/skybox_fs.glsl");

    std::vector<std::string> skyboxTex = {
        "../../../../src/Examples/PhongShading/textures/skybox/right.jpg",
        "../../../../src/Examples/PhongShading/textures/skybox/left.jpg",
        "../../../../src/Examples/PhongShading/textures/skybox/top.jpg",
        "../../../../src/Examples/PhongShading/textures/skybox/bottom.jpg",
        "../../../../src/Examples/PhongShading/textures/skybox/front.jpg",
        "../../../../src/Examples/PhongShading/textures/skybox/back.jpg"
    };

    skybox = new Skybox(skyboxProgram, 
                        "../../../../src/Examples/PhongShading/textures/skybox/Brooklyn_Bridge_Planks_2k.hdr");

    std::ifstream sphere("../../../../src/Examples/PhongShading/objects/sphere.in");
    int vertCount;
    sphere >> vertCount;
    std::vector<float> vertices(vertCount * 3);
    for (int i = 0; i < vertCount * 3; i++)
    {
        sphere >> vertices[i];
    }
    std::vector<float> uvs(vertCount * 2);
    for (int i = 0; i < vertCount * 2; i++)
    {
        sphere >> uvs[i];
    }
    int triangleCount;
    sphere >> triangleCount;
    std::vector<unsigned int> indices(triangleCount * 3);
    for (int i = 0; i < triangleCount * 3; i++)
    {
        sphere >> indices[i];
    }
    
    std::vector<float> normals = calculate_normals(vertices, indices, uvs);

    materials.push_back(new Material(pbrProgram, true, "Metal Scratched"));
    materials[0]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Metal Scratched/Metal_scratched_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Metal Scratched/Metal_scratched_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal Scratched/Metal_scratched_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal Scratched/Metal_scratched_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal Scratched/Metal_scratched_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Metal Plate"));
    materials[1]->setPBRMaterial("../../../../src/Examples/PhongShading/textures/Metal Plate/Metal_plate_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Metal Plate/Metal_plate_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal Plate/Metal_plate_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal Plate/Metal_plate_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal Plate/Metal_plate_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Leather"));
    materials[2]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Leather/Leather_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Leather/Leather_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Leather/Leather_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Leather/Leather_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Leather/Leather_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Bricks"));
    materials[3]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Bricks/Bricks_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Bricks/Bricks_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Bricks/Bricks_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Bricks/Bricks_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Bricks/Bricks_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Rusted Iron"));
    materials[4]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Tiles"));
    materials[5]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Tiles/Tiles_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Tiles/Tiles_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Tiles/Tiles_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Tiles/Tiles_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Tiles/Tiles_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Concrete"));
    materials[6]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Concrete/Concrete_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Concrete/Concrete_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Concrete/Concrete_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Concrete/Concrete_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Concrete/Concrete_ambient.jpg"
    );

    materials.push_back(new Material(pbrProgram, true, "Metal"));
    materials[7]->setPBRMaterial(
    "../../../../src/Examples/PhongShading/textures/Metal/Metal_basecolor.jpg", 
    "../../../../src/Examples/PhongShading/textures/Metal/Metal_normal.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal/Metal_metallic.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal/Metal_roughness.jpg",
    "../../../../src/Examples/PhongShading/textures/Metal/Metal_ambient.jpg"
    );

    materials.push_back(new Material(program, false, "Phong"));
    materials[8]->setPhongMaterial(
        "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_basecolor.jpg", 
        "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_basecolor.jpg", 
        "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_normal.jpg",
        "../../../../src/Examples/PhongShading/textures/Rusted Iron/Rustediron_roughness.jpg"
    );

    models.push_back(new Model(vertices, normals, uvs, indices, materials[0]));

    genUBO(pbrProgram);
    genUBO(program);

    set_camera(new Camera);
    camera()->look_at(glm::vec3{0.5, 1.0, 4.0}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0, 0.0, 1.0});
    set_controler(new CameraControler(camera()));

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    auto aspect = (float) w / h;
    camera()->perspective(glm::pi<float>() / 4.0, aspect, 0.1f, 100.0f);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //HDR setup
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenSize.first, screenSize.second, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenSize.first, screenSize.second);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Dual rendering
    glGenFramebuffers(1, &phongFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, phongFbo);
    glGenTextures(1, &phongColorBuffer);
    glBindTexture(GL_TEXTURE_2D, phongColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenSize.first, screenSize.second, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenRenderbuffers(1, &phongRboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, phongRboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenSize.first, screenSize.second);
    // attach buffers
    glBindFramebuffer(GL_FRAMEBUFFER, phongFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, phongColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, phongRboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SimpleShapeApplication::frame() {
    ImGui::Begin("Material");
    ImGui::Checkbox("Blinn-Phong - PBR Comparison", &comparison);
    if(comparison){
        ImGui::SliderFloat("PBR - Blinn-Phong (0 - PBR, 1 - Blinn-Phong)", &phongMixRatio, 0.0f, 1.0f, "ratio = %.3f");
    }
    else{
        const char* items[] = { "Metal Scratched", "Metal Plate", "Leather", "Bricks", "Rusted Iron", "Tiles", "Concrete", "Metal", "Phong" };
        static int item_current = 0;
        ImGui::Combo("Pick material", &item_current, items, IM_ARRAYSIZE(items));
        if(models[0]->getMaterial()->name != items[item_current]){
            models[0]->setMaterial(materials[item_current]);
        }
    }
    overImGuiWindow = ImGui::IsWindowHovered();
    ImGui::End();


    if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);


    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float frameTime = glfwGetTime();
    float deltaTime = frameTime - lastFrameTime;
    lastFrameTime = frameTime;
    lightPos[0] = glm::vec4(glm::cos(frameTime) * 2, glm::sin(frameTime) * 2, 0.5f, 0.f);

    glm::vec3 viewPos = camera()->position();

    glm::mat4 projection = camera()->projection();
    glm::mat4 view = camera()->view();
    glm::mat4 model = glm::mat4(1.0);

    for (int i = 0; i < u_buffer_handles.size(); i++)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handles[i][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4 * sizeof(float), &viewPos[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 72 * sizeof(float), 32 * sizeof(float), &lightPos[0][0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        
        glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handles[i][1]);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &projection[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &view[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), &model[0]);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    if(!comparison){
        for (Model* m : models)
        {
            m->getShader().bind();
            m->setUniforms();
            skybox->bindEnvTextures();
            m->getVertexArray().Bind();
            GLCall(glDrawElements(GL_TRIANGLES, m->getIndexBuffer().GetCount(), GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0)));
            m->getVertexArray().Unbind();
        }
    }
    else{
        models[0]->setMaterial(materials[4]);
        models[0]->getShader().bind();
        models[0]->getMaterial()->ambientEnabled = 0;
        models[0]->setUniforms();
        models[0]->getVertexArray().Bind();
        GLCall(glDrawElements(GL_TRIANGLES, models[0]->getIndexBuffer().GetCount(), GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0)));
        models[0]->getVertexArray().Unbind();
        models[0]->getMaterial()->ambientEnabled = 1;
    }
    //Render skybox
    glDepthFunc(GL_LEQUAL);
    skybox->getShader().bind();
    skybox->setUniforms(glm::mat4(glm::mat3(view)), projection);
    skybox->getVertexArray()->Bind();
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTexture()));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
    glDepthFunc(GL_LESS);
    if(comparison){
        glBindFramebuffer(GL_FRAMEBUFFER, phongFbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        models[0]->setMaterial(materials[8]);
        models[0]->getShader().bind();
        models[0]->setUniforms();
        models[0]->getVertexArray().Bind();
        GLCall(glDrawElements(GL_TRIANGLES, models[0]->getIndexBuffer().GetCount(), GL_UNSIGNED_INT, reinterpret_cast<GLvoid *>(0)));
        models[0]->getVertexArray().Unbind();
        glDepthFunc(GL_LEQUAL);
        
        //render skybox on the other texture as well
        skybox->getShader().bind();
        skybox->setUniforms(glm::mat4(glm::mat3(view)), projection);
        skybox->getVertexArray()->Bind();
        GLCall(glActiveTexture(GL_TEXTURE0));
        GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->getTexture()));
        GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
        glDepthFunc(GL_LESS);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Render hdr texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Shader& quadShader = skybox->getQuadShader();
    quadShader.bind();
    quadShader.setInt("hdrBuffer", 0);
    quadShader.setInt("phongBuffer", 1);
    quadShader.setFloat("mixRatio", phongMixRatio);
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindTexture(GL_TEXTURE_2D, colorBuffer));
    GLCall(glActiveTexture(GL_TEXTURE1));
    GLCall(glBindTexture(GL_TEXTURE_2D, phongColorBuffer));
    skybox->renderQuadTexture();
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera()->set_aspect((float) w / h);
}


void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
            if(!overImGuiWindow)
                controler_->LMB_pressed(x, y);
            else
                mousePressedOverWindow = true;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
            if(!mousePressedOverWindow)
                controler_->LMB_released(x, y);
            else
                mousePressedOverWindow = false;
        }
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        if(!overImGuiWindow){
            controler_->mouse_moved(x, y);
        }
    }
}

std::vector<float> SimpleShapeApplication::calculate_normals(const std::vector<GLfloat>& vertexData, 
    const std::vector<unsigned int>& indexData, std::vector<float>& uvs) 
{
    std::vector<float> output(vertexData.size() * 2);
    for (size_t i = 0; i < indexData.size(); i++)
    {
        if(i % 3 == 0){
            glm::vec3 v1 = {vertexData[indexData[i] * 3], vertexData[indexData[i] * 3 + 1], vertexData[indexData[i] * 3 + 2]};
            glm::vec3 v2 = {vertexData[indexData[i + 1] * 3], vertexData[indexData[i + 1] * 3 + 1], vertexData[indexData[i + 1] * 3 + 2]};
            glm::vec3 v3 = {vertexData[indexData[i + 2] * 3], vertexData[indexData[i + 2] * 3 + 1], vertexData[indexData[i + 2] * 3 + 2]};
            glm::vec3 edge1 = v2 - v1;
            glm::vec3 edge2 = v3 - v1;
            glm::vec2 deltaUV1 = glm::vec2(uvs[indexData[i + 1] * 2], uvs[indexData[i + 1] * 2 + 1]) - glm::vec2(uvs[indexData[i] * 2], uvs[indexData[i] * 2 + 1]);
            glm::vec2 deltaUV2 = glm::vec2(uvs[indexData[i + 2] * 2], uvs[indexData[i + 2] * 2 + 1]) - glm::vec2(uvs[indexData[i] * 2], uvs[indexData[i] * 2 + 1]);
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
            
            glm::vec3 tangent;
            float f = 1.f/(deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);            
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);            
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);            
            tangent = glm::normalize(tangent);

            for (size_t j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    output[indexData[i + j] * 6 + k] += normal[k];
                    output[indexData[i + j] * 6 + 3 + k] += tangent[k];
                }
            }
        }
    }
    for (int i = 0; i < vertexData.size()/3; i++)
    {
        glm::vec3 normalizedNormal = {output[i * 6], output[i * 6 + 1], output[i * 6 + 2]};
        glm::vec3 normalizedTangent = {output[i * 6 + 3], output[i * 6 + 4], output[i * 6 + 5]};
        normalizedNormal = glm::normalize(normalizedNormal);
        normalizedTangent = glm::normalize(normalizedTangent);
        for (int k = 0; k < 3; k++)
        {
            output[i * 6 + k] = normalizedNormal[k];
            output[i * 6 + k + 3] = normalizedTangent[k];
        }
    }
    
    return output;
}

void SimpleShapeApplication::genUBO(Shader& program) 
{
    auto u_modifiers_index = glGetUniformBlockIndex(program.getID(), "Modifiers");
    if (u_modifiers_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Modifiers uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program.getID(), u_modifiers_index, 0);
    }
    auto u_transformations_index = glGetUniformBlockIndex(program.getID(), "Transformations");
    if (u_transformations_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Transformations uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program.getID(), u_transformations_index, 1);
    }

    int index = u_buffer_handles.size();
    u_buffer_handles.push_back(std::vector<GLuint>(2));
    glGenBuffers(2, &u_buffer_handles[index][0]);

    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handles[index][0]);
    glm::vec4 strength[2] = {glm::vec4(3.f), glm::vec4(10.f)};
    lightPos[1] = glm::vec4(0.f, 0.f, 3.f, 0.0);
    glm::vec4 light[2] = {glm::vec4(1.0, 0.0, 0.0, 1.0), glm::vec4(1.f,1.f,1.f, 1.0)};
    glBufferData(GL_UNIFORM_BUFFER,  104 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 4 * sizeof(int), &lightCount);
    glBufferSubData(GL_UNIFORM_BUFFER, 8 * sizeof(float), 32 * sizeof(float), &strength[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 40 * sizeof(float), 32 * sizeof(float), &light[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 72 * sizeof(float), 32 * sizeof(float), &lightPos[0][0]);
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_buffer_handles[index][0]);


    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handles[index][1]);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_buffer_handles[index][1]);
}
