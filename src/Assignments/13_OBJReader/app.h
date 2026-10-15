//
// Created by pbialas on 05.08.2020.
//

#pragma once

#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
#include "Engine/camera.h"
#include "Engine/camera_controller.h"
#include "Engine/Mesh.h"
#include "Application/application.h"
#include "Application/gl_handle.h"

class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, bool debug, int swap_interval = 1) :
        Application(width, height, title, debug, swap_interval), camera_(nullptr) {
    }

    ~SimpleShapeApplication() override {
        delete controller_;
        delete camera_;
    }

    void framebuffer_resize_callback(int w, int h) override;

    void init() override;

    void frame() override;

    void set_camera(xe::Camera* camera) {
        camera_ = camera;
        camera_controller_.set_camera(camera_);
    }

    void add_mesh(xe::Mesh* mesh) {
        meshes_.push_back(mesh);
    }

    void scroll_callback(double xoffset, double yoffset) override;
    void cursor_position_callback(double x, double y) override;
    void mouse_button_callback(int button, int action, int mods) override;
    void set_controller(xe::CameraController* controller) { controller_ = controller; }

    xe::Camera* camera() const {
        assert(camera_);
        return camera_;
    }

private:
    glm::mat4 M_;
    xe::Camera* camera_;
    xe::CameraController* controller_;
    xe::CameraController camera_controller_;
    double mouse_x_ = 0.0;
    double mouse_y_ = 0.0;
	xe::gl::Buffer u_trans_buffer_;
    std::vector<xe::Mesh*> meshes_;
};