//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "camera.h"
#include "Application/application.h"


class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug, int swap_interval = 1) :
        Application(width, height, title, debug, swap_interval), camera_(nullptr) {
    }

    ~SimpleShapeApplication() override {
        delete camera_;
    }

    void framebuffer_resize_callback(int w, int h) override;

    void init() override;

    void frame() override;

    void set_camera(Camera* camera) { camera_ = camera; }

    void scroll_callback(double xoffset, double yoffset) override;


    Camera* camera() const {
        assert(camera_);
        return camera_;
    }

private:
    glm::mat4 M_;
    GLuint vao_;
    Camera* camera_;
    GLuint u_trans_buffer_handle_;
};