//
// Created by pbialas on 05.08.2020.
//


#pragma once

#include <vector>

#include "glad/glad.h"

#include "glm/glm.hpp"

#include "Application/application.h"
#include "Application/utils.h"
#include "camera.h"
#include "camera_controler.h"
#include "model.h"
#include "skybox.h"
#include "shader.h"


class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor), camera_(nullptr), controler_(nullptr), screenSize(width, height) {}

    void init() override;;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override {
        Application::scroll_callback(xoffset, yoffset);
        if (controler_)
            controler_->zoom(yoffset / 30.0);
    }

    void mouse_button_callback(int button, int action, int mods) override;

    void cursor_position_callback(double x, double y) override;

    void set_camera(Camera *camera) { camera_ = camera; }

    void set_controler(CameraControler *controler) { controler_ = controler; }

    Camera *camera() { return camera_; }

    std::vector<float> calculate_normals(const std::vector<GLfloat>& vertexData, const std::vector<unsigned int>& indexData, std::vector<float>& uvs);

    ~SimpleShapeApplication() {
        delete camera_;
        delete controler_;
    }

private:
    void genUBO(Shader& program);
    std::vector<std::vector<GLuint>> u_buffer_handles;

    std::pair<int,int> screenSize;
    unsigned int colorBuffer;
    unsigned int fbo;
    unsigned int rboDepth;
    
    unsigned int phongColorBuffer;
    unsigned int phongFbo;
    unsigned int phongRboDepth;

    float phongMixRatio = 0;

    Camera *camera_;
    CameraControler *controler_;

    GLuint u_buffer_handle_[2];
    float lastFrameTime = 0;
    
    std::vector<Model*> models;
    std::vector<Material*> materials;
    
    int lightCount = 2;
    glm::vec4 lightPos[2] = {glm::vec4(0.5, 1.0, 1.0, 0.f), glm::vec4(0,0,0,0)};
    
    bool show_demo_window = false;
    bool comparison = false;
    bool overImGuiWindow = false;
    bool mousePressedOverWindow = false;

    Skybox* skybox = nullptr;

};