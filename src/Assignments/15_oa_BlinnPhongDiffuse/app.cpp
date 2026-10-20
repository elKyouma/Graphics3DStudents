//
// Created by pbialas on 25.09.2020.
//


#include "app.h"

#include <vector>

#include "spdlog/spdlog.h"

#include "glad/gl.h"

#include "Application/utils.h"
#include "Application/uniforms.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Engine/BlinnPhongMaterial.h"
#include "Engine/texture.h"

#include "stb/stb_image.h"

void SimpleShapeApplication::init() {

    set_camera(new xe::Camera);
    set_controller(new xe::CameraController(camera()));
    xe::BlinnPhongMaterial::init();

    camera()->look_at(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	camera()->perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 20.0f);

    add_light(xe::PointLight({ 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, 1.0f, 0.1f));


    OGL_CALL(glCreateBuffers(1, u_trans_buffer_.put()));
    OGL_CALL(glNamedBufferData(u_trans_buffer_.get(), 48 * sizeof(float), 0, GL_DYNAMIC_DRAW));

    OGL_CALL(glCreateBuffers(1, u_light_buffer_handle_.put()));
    OGL_CALL(glNamedBufferData(u_light_buffer_handle_.get(), 66 * sizeof(float), 0, GL_DYNAMIC_DRAW));

	//uniform_info(xe::BlinnPhongMaterial::program(), "Lights");

    // Setting the background color of the rendering window,
    // I suggest not using white or black for better debugging.
    OGL_CALL(glClearColor(0.81f, 0.81f, 0.8f, 1.0f));

    // This set up an OpenGL viewport of the size of the whole rendering window.
    auto [w, h] = frame_buffer_size();
    OGL_CALL(glViewport(0, 0, w, h));

    //OGL_CALL(glEnable(GL_CULL_FACE));
    OGL_CALL(glEnable(GL_DEPTH_TEST));

    stbi_set_flip_vertically_on_load(true);

    auto pyramid = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj",
        std::string(ROOT_DIR) + "/Models");
    if (!pyramid) {
        SPDLOG_CRITICAL("Cannot load the pyramid model");
        exit(-1);
    }
    add_mesh(pyramid);
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    if (w <= 0 || h <= 0)
        return;
	OGL_CALL(glViewport(0, 0, w, h));
	camera()->set_aspect(static_cast<float>(w) / static_cast<float>(h));
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(-yoffset / 30.0f);
}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controller_) {
        controller_->mouse_moved(x, y);
    }
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controller_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controller_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controller_->LMB_released(x, y);
    }
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will set up all the required vertex attribute arrays.
    M_ = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 VM = camera()->view() * M_;
    glm::mat4 PVM = camera()->projection() * VM;

    auto R = glm::mat3(VM);
    auto VM_normal = glm::mat4(glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1])));

    glm::vec3 ambient = {0.25, 0.25, 0.25};

    OGL_CALL(glNamedBufferSubData(u_trans_buffer_.get(), 0, 16 * sizeof(float), &PVM));
    OGL_CALL(glNamedBufferSubData(u_trans_buffer_.get(), 16 * sizeof(float), 16 * sizeof(float), &VM));
    OGL_CALL(glNamedBufferSubData(u_trans_buffer_.get(), 32 * sizeof(float), 16 * sizeof(float), &VM_normal));
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_trans_buffer_.get()));

    int n_lights = static_cast<int>(std::min<std::size_t>(lights_.size(), xe::MAX_POINT_LIGHTS));


	uint16_t offset = 0;
    OGL_CALL(glNamedBufferSubData(u_light_buffer_handle_.get(), offset, 3 * sizeof(float), &ambient));
	offset += 3 * sizeof(float);
    OGL_CALL(glNamedBufferSubData(u_light_buffer_handle_.get(), offset, sizeof(int), &n_lights));
    offset += sizeof(int);
    for (auto l : lights_)
    {
		glm::vec3 pos = glm::vec3(camera_->view() * glm::vec4(l.position, 1.0f));
        OGL_CALL(glNamedBufferSubData(u_light_buffer_handle_.get(), offset, 3 * sizeof(float), &pos));
        offset += 3 * sizeof(float);
        OGL_CALL(glNamedBufferSubData(u_light_buffer_handle_.get(), offset, sizeof(float), &l.radius));
        offset += sizeof(float);
        OGL_CALL(glNamedBufferSubData(u_light_buffer_handle_.get(), offset, 3 * sizeof(float), &l.color));
        offset += 3 * sizeof(float);
        OGL_CALL(glNamedBufferSubData(u_light_buffer_handle_.get(), offset, sizeof(float), &l.intensity));
        offset += sizeof(float);
    }
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 2, u_light_buffer_handle_.get()));

    for (auto m : meshes_)
        m->draw();
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0));
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 2, 0));
}
