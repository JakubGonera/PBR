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


class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor), camera_(nullptr), controler_(nullptr) {}

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

    ~SimpleShapeApplication() {
        delete camera_;
        delete controler_;
    }

private:
    GLuint vao_;
    Camera *camera_;
    CameraControler *controler_;
    GLuint u_buffer_handle_[2];
};