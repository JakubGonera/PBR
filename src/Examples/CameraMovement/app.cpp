//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Application/utils.h"

void SimpleShapeApplication::init() {

    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");

    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

    std::vector<GLfloat> vertices = {
            //Base
            -0.5, -0.5, 0.0, 0.5, 0.5, 0.5, //0
            -0.5, 0.5, 0.0, 0.5, 0.5, 0.5,  //1
            0.5, -0.5, 0.0, 0.5, 0.5, 0.5, //2
            0.5, 0.5, 0.0, 0.5, 0.5, 0.5,  //3

            0.5, 0.5, 0.0, 1.0, 0.0, 0.0, //3
            0.0, 0.0, 1.0, 1.0, 0.0, 0.0,
            0.5, -0.5, 0.0, 1.0, 0.0, 0.0, //2

            -0.5, -0.5, 0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 1.0, 0.0, 1.0, 0.0,
            0.5, -0.5, 0.0, 0.0, 1.0, 0.0,

            -0.5, -0.5, 0.0, 0.0, 0.0, 1.0,
            0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
            -0.5, 0.5, 0.0, 0.0, 0.0, 1.0,

            -0.5, 0.5, 0.0, 1.0, 0.0, 1.0, //1
            0.0, 0.0, 1.0, 1.0, 0.0, 1.0,
            0.5, 0.5, 0.0, 1.0, 0.0, 1.0, //3

    };


    std::vector<GLushort> indices = {
            0, 1, 2, 1, 3, 2,
            4, 5, 6,
            7, 8, 9,
            10, 11, 12,
            13, 14, 15
    };

    GLuint v_buffer_handle[2];
    glGenBuffers(2, v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_handle[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_buffer_handle[1]);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    auto u_modifiers_index = glGetUniformBlockIndex(program, "Modifiers");
    if (u_modifiers_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Modifiers uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program, u_modifiers_index, 0);
    }
    auto u_transformations_index = glGetUniformBlockIndex(program, "Transformations");
    if (u_transformations_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Transformations uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program, u_transformations_index, 1);
    }


    glGenBuffers(2, u_buffer_handle_);

    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle_[0]);
    float strength = 1.0;
    float light[3] = {1.0, 1.0, 1.0};
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_buffer_handle_[0]);


    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle_[1]);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);


    set_camera(new Camera);
    camera()->look_at(glm::vec3{0.5, 1.0, 4.0}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0, 0.0, 1.0});
    set_controler(new CameraControler(camera()));

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    auto aspect = (float) w / h;
    camera()->perspective(glm::pi<float>() / 4.0, aspect, 0.1f, 100.0f);


    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_buffer_handle_[1]);
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    auto PVM = camera()->projection() * camera()->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle_[1]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
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

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}