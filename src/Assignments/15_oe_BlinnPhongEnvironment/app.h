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
#include "Engine/light.h"

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

    void cleanup() override {
        empty_vao_.reset();
        skybox_program_.reset();
        environment_map_.reset();
        Application::cleanup();
    }

    void set_camera(xe::Camera* camera) {
        camera_ = camera;
        camera_controller_.set_camera(camera_);
    }

    void add_mesh(xe::Mesh* mesh) {
        meshes_.push_back(mesh);
    }

    void add_light(xe::PointLight light) { 
        lights_.push_back(light); 
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
    xe::Camera* camera_;
    xe::CameraController* controller_;
    xe::CameraController camera_controller_;
    double mouse_x_ = 0.0;
    double mouse_y_ = 0.0;
	xe::gl::Buffer u_trans_buffer_;
    xe::gl::Buffer u_light_buffer_handle_;
    xe::gl::Texture environment_map_;
    std::vector<xe::PointLight> lights_;
    std::vector<xe::Mesh*> meshes_;

	xe::gl::Program skybox_program_;
	xe::gl::VertexArray empty_vao_;

    inline static GLint skybox_location_ = -1;
    inline static GLint inverse_PV_rotation_location_ = -1;

};