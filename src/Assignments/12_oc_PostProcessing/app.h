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

    void create_framebuffer(int w, int h);
    void delete_framebuffer();

	void cleanup() override {
		delete_framebuffer();
		OGL_CALL(glDeleteProgram(display_program_));
		OGL_CALL(glDeleteProgram(sobel_cs_program_));
        OGL_CALL(glDeleteProgram(sobel_fs_program_));
        OGL_CALL(glDeleteProgram(vignette_cs_program_));
        OGL_CALL(glDeleteProgram(grayscale_cs_program_));
        OGL_CALL(glDeleteProgram(gaussian_cs_program_));
        OGL_CALL(glDeleteProgram(optimized_gaussian_cs_program_));
	}

    void draw_full_screen(GLuint program, GLuint texture);
    void draw_to_next_post_process(GLuint program, GLuint initTexture = 0);
    void key_callback(int key, int scancode, int action, int mods) override;


private:
    glm::mat4 M_;
    xe::Camera* camera_;
    xe::CameraController* controller_;
    xe::CameraController camera_controller_;
    double mouse_x_ = 0.0;
    double mouse_y_ = 0.0;
    GLuint u_trans_buffer_handle_;
    GLuint fbo_, scene_color_, scene_depth_, display_program_, processed_input_, processed_output_;
    GLuint sobel_cs_program_, sobel_fs_program_, vignette_cs_program_, grayscale_cs_program_, negative_cs_program_;
	GLuint optimized_gaussian_cs_program_;
    GLuint display_vao_ = 0u;
    GLuint gaussian_cs_program_;
    std::vector<xe::Mesh*> meshes_;
    bool use_compute_ = false;
    bool use_vignette_ = false;
    bool use_grayscale_ = false;
    bool use_negative_ = false;
    bool use_gaussian_ = false;
    bool use_optimized_gaussian_ = false;
};