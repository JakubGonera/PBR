//
// Created by Piotr Białas on 02/11/2020.
//

#pragma once

#include "camera.h"

class CameraControler {
public:

    CameraControler(Camera *camera) : camera_(camera), LMB_pressed_(false), scale_(0.015f) {}

    CameraControler() : CameraControler(nullptr) {};

    void set_camera(Camera *camera) { camera_ = camera; }

    void rotate_camera(float dx, float dy) {
        camera_->rotate_around_center(-scale_ * dy, camera_->x());
        camera_->rotate_around_center(-scale_ * dx, camera_->y());
    }

    void update_mouse_position(float x, float y){
        x_ = x;
        y_ = y;
    }

    void mouse_moved(float x, float y) {
        if (LMB_pressed_) {
            auto dx = x - x_;
            auto dy = y - y_;
            x_ = x;
            y_ = y;

            rotate_camera(dx, dy);
        }
    };

    void LMB_pressed(float x, float y) {
        LMB_pressed_ = true;
        x_ = x;
        y_ = y;
    };

    void LMB_released(float x, float y) {
        LMB_pressed_ = false;
        auto dx = x - x_;
        auto dy = y - y_;

        rotate_camera(dx, dy);
    };

    void zoom(float y_offset) {
        auto x = camera_->fov() / glm::pi<float>();
        auto y = inverse_logistic(x);
        y += y_offset;
        auto fov = logistic(y) * glm::pi<float>();
        camera_->set_fov(fov);
    }

private:
    Camera *camera_;
    bool LMB_pressed_;
    float x_, y_;
    float scale_;
};