//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

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
            -0.5f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.75f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.4f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.4f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.4f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.4f, 0.25f, 0.0f, 0.0f, 1.0f, 0.0f
    };


    std::vector<GLushort> indices = {
            0, 1, 2, 3, 4, 5, 3, 5, 6
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

    GLuint u_buffer_handle[2];
    glGenBuffers(2, u_buffer_handle);

    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle[0]);
    float strength = 0.5;
    float light[3] = {0.7, 0.2, 0.3};
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float), &strength);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), light);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_buffer_handle[0]);


    glBindBuffer(GL_UNIFORM_BUFFER, u_buffer_handle[1]);

    int w,h;
    std::tie(w,h)= frame_buffer_size();
    auto V = glm::lookAt(glm::vec3{1.0, .5, 2.0}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0, 0.0, 1.0});
    auto P = glm::perspective(glm::half_pi<float>(), (float)w/h,0.1f, 100.0f);
    auto PVM = P*V;
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_buffer_handle[1]);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}
