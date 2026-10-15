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
#include "Engine/KdMaterial.h"

#include "stb/stb_image.h"

void SimpleShapeApplication::init() {
    /*
     * A utility function that reads the shaders' source files, compiles them and creates the program object,
     * as everything in OpenGL we reference the program by an integer "handle".
     * The input to this function is a map that associates a shader type (GL_VERTEX_SHADER and GL_FRAGMENT_SHADER) with
     * its source file. The source files are located in the PROJECT_DIR/shaders directory, where  PROJECT_DIR is the
     * current assignment directory (e.g., src/Assignments/00_Triangle).
     */
    set_camera(new xe::Camera);
    set_controller(new xe::CameraController(camera()));

    // A vector containing the x,y,z vertex coordinates for the triangle.
    std::vector<GLfloat> vertices = {
        -0.5f, -0.5f, 0.0f, 0.5f,   0.809f, // 0
        -0.5f,  0.5f, 0.0f, 0.191f, 0.5f,   // 1
         0.5f, -0.5f, 0.0f, 0.809f, 0.5f,   // 2
         0.5f,  0.5f, 0.0f, 0.5f,   0.191f,// 3

          0.0f, 0.0f, 1.0f, 1.0f, 0.0f,      // 4
          0.0f, 0.0f, 1.0f, 1.0f, 1.0f,      // 5
          0.0f, 0.0f, 1.0f, 0.0f, 1.0f,      // 6
          0.0f, 0.0f, 1.0f, 0.0f, 0.0f       // 7
    };

    std::vector<GLubyte> indices = {
        0, 1, 2,
        2, 1, 3,

        2, 3, 4,
        0, 2, 5,
        1, 0, 6,
        3, 1, 7
    };

    xe::KdMaterial::init();

    camera()->look_at(glm::vec3(2.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera()->perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f, 20.0f);
    /*
     * All the calls to the OpenGL API are "encapsulated" in the OGL_CALL macro for debugging purposes as explained in
     * Assignments/DEBUGGING.md. The macro is defined in src/Application/utils.h. If the call to the OpenGL API returns an
     * error code, the macro will print the name of the function that failed, the file and line number where the error
     * occurred.
     */

    OGL_CALL(glCreateBuffers(1, &u_trans_buffer_handle_));
    OGL_CALL(glNamedBufferData(u_trans_buffer_handle_, 16 * sizeof(float), 0, GL_DYNAMIC_DRAW));

    // Setting the background color of the rendering window,
    // I suggest not using white or black for better debugging.
    OGL_CALL(glClearColor(0.81f, 0.81f, 0.8f, 1.0f));

    // This set up an OpenGL viewport of the size of the whole rendering window.
    auto [w, h] = frame_buffer_size();
    OGL_CALL(glViewport(0, 0, w, h));

    OGL_CALL(glEnable(GL_CULL_FACE));
    OGL_CALL(glEnable(GL_DEPTH_TEST));

    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;

    auto texture_file = std::string(ROOT_DIR) + "/Models/multicolor.png";
    auto img = stbi_load(texture_file.c_str(), &width, &height, &channels, 0);
    if (!img) {
        SPDLOG_ERROR("Could not read image from file `{}'", texture_file);
    }
    else {
        SPDLOG_INFO("Loaded a {}x{} texture with {} channels", width, height, channels);
    }

    GLuint tex_handle = 0;
	OGL_CALL(glGenTextures(1, &tex_handle));
	OGL_CALL(glBindTexture(GL_TEXTURE_2D, tex_handle));
    OGL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	OGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img));
    OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    stbi_image_free(img);

    auto compute_program = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/example_cs.glsl"} });
    if (!compute_program) {
        SPDLOG_CRITICAL("Invalid compute program");
        exit(-1);
    }

    GLuint processed_tex_handle;
    OGL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &processed_tex_handle));
    OGL_CALL(glTextureStorage2D(processed_tex_handle, 1, GL_RGBA16F, width, height));
    OGL_CALL(glTextureParameteri(processed_tex_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    OGL_CALL(glTextureParameteri(processed_tex_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	OGL_CALL(glUseProgram(compute_program));
    OGL_CALL(glBindTextureUnit(0, tex_handle));
    OGL_CALL(glBindImageTexture(0, processed_tex_handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F));
	OGL_CALL(glDispatchCompute((GLuint)width / 16, (GLuint)height / 16, 1));
    OGL_CALL(glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT));


    OGL_CALL(glBindTextureUnit(0, 0));
    OGL_CALL(glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F));
	OGL_CALL(glDeleteProgram(compute_program));
	OGL_CALL(glDeleteTextures(1, &tex_handle));


    auto pyramid = new xe::Mesh(5 * sizeof(float), vertices.size() * sizeof(GLfloat), GL_STATIC_DRAW,
        indices.size() * sizeof(GLubyte), GL_UNSIGNED_BYTE, GL_STATIC_DRAW);

    pyramid->add_attribute(xe::AttributeType::POSITION, 3, GL_FLOAT, 0);
    pyramid->add_attribute(xe::AttributeType::TEXCOORD_0, 2, GL_FLOAT, 3 * sizeof(GLfloat));
    pyramid->load_vertices(0, vertices.size() * sizeof(GLfloat), vertices.data());
    pyramid->load_indices(0, indices.size() * sizeof(GLubyte), indices.data());
    pyramid->add_submesh(0, 18, new xe::KdMaterial({ 1.f, 1.f, 1.0f, 1.0f }, false, processed_tex_handle));

    add_mesh(pyramid);

	auto[sizeX, sizeY] = frame_buffer_size();
    create_framebuffer(sizeX, sizeY);
}

void SimpleShapeApplication::create_framebuffer(int w, int h)
{
    OGL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &processed_input_));
    OGL_CALL(glTextureStorage2D(processed_input_, 1, GL_RGBA16F, w, h));
    OGL_CALL(glTextureParameteri(processed_input_, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    OGL_CALL(glTextureParameteri(processed_input_, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    OGL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &processed_output_));
    OGL_CALL(glTextureStorage2D(processed_output_, 1, GL_RGBA16F, w, h));
    OGL_CALL(glTextureParameteri(processed_output_, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    OGL_CALL(glTextureParameteri(processed_output_, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    OGL_CALL(glCreateTextures(GL_TEXTURE_2D, 1, &scene_color_));
    OGL_CALL(glTextureStorage2D(scene_color_, 1, GL_SRGB8_ALPHA8, w, h));
    OGL_CALL(glTextureParameteri(scene_color_, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    OGL_CALL(glTextureParameteri(scene_color_, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    OGL_CALL(glCreateRenderbuffers(1, &scene_depth_));
    OGL_CALL(glNamedRenderbufferStorage(scene_depth_, GL_DEPTH_COMPONENT24, w, h));

    OGL_CALL(glCreateFramebuffers(1, &fbo_));
    OGL_CALL(glNamedFramebufferTexture(fbo_, GL_COLOR_ATTACHMENT0, scene_color_, 0));
    OGL_CALL(glNamedFramebufferRenderbuffer(fbo_, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, scene_depth_));

    GLenum status;
    OGL_CALL(status = glCheckNamedFramebufferStatus(fbo_, GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_CRITICAL("Framebuffer is not complete: {:#x}", status);
        exit(-1);
    }

	display_program_ = xe::utils::create_program({
		{GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/fullscreen_vs.glsl"},
		{GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/display_fs.glsl"}
		});

    if (!display_program_) {
        SPDLOG_CRITICAL("Invalid Display fragment program");
        exit(-1);
    }

    sobel_fs_program_ = xe::utils::create_program({
    {GL_VERTEX_SHADER, std::string(PROJECT_DIR) + "/shaders/fullscreen_vs.glsl"},
    {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/sobel_fs.glsl"}
        });

    if (!sobel_fs_program_) {
        SPDLOG_CRITICAL("Invalid Sobel fragment program");
        exit(-1);
    }

    OGL_CALL(glCreateVertexArrays(1, &display_vao_));


    sobel_cs_program_ = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/sobel_cs.glsl"} });
    if (!sobel_cs_program_) {
        SPDLOG_CRITICAL("Invalid Sobel compute program");
        exit(-1);
    }

    vignette_cs_program_ = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/vignette_cs.glsl"} });
    if (!vignette_cs_program_) {
        SPDLOG_CRITICAL("Invalid Vignette compute program");
        exit(-1);
    }

    grayscale_cs_program_ = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/grayscale_cs.glsl"} });
    if (!grayscale_cs_program_) {
        SPDLOG_CRITICAL("Invalid Grayscale compute program");
        exit(-1);
    }

    negative_cs_program_ = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/negative_cs.glsl"} });
    if (!negative_cs_program_) {
        SPDLOG_CRITICAL("Invalid Negative compute program");
        exit(-1);
    }

    gaussian_cs_program_ = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/gaussian_cs.glsl"} });
    if (!gaussian_cs_program_) {
        SPDLOG_CRITICAL("Invalid Gaussian compute program");
        exit(-1);
    }
    
    optimized_gaussian_cs_program_ = xe::utils::create_program(
        { {GL_COMPUTE_SHADER, std::string(PROJECT_DIR) + "/shaders/gaussian_optimized_cs.glsl"} });
    if (!optimized_gaussian_cs_program_) {
        SPDLOG_CRITICAL("Invalid Optimized Gaussian compute program");
        exit(-1);
    }
}

void SimpleShapeApplication::delete_framebuffer() {
    OGL_CALL(glDeleteFramebuffers(1, &fbo_));
    OGL_CALL(glDeleteTextures(1, &scene_color_));
    OGL_CALL(glDeleteRenderbuffers(1, &scene_depth_));
    OGL_CALL(glDeleteTextures(1, &processed_input_));
    OGL_CALL(glDeleteTextures(1, &processed_output_));
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    if (w <= 0 || h <= 0)
        return;
	OGL_CALL(glViewport(0, 0, w, h));
	delete_framebuffer();
	create_framebuffer(w, h);
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

void SimpleShapeApplication::draw_full_screen(GLuint program, GLuint texture) {
    OGL_CALL(glUseProgram(program));
    OGL_CALL(glBindTextureUnit(0, texture));
    OGL_CALL(glBindVertexArray(display_vao_));
    OGL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
    OGL_CALL(glBindVertexArray(0));
    OGL_CALL(glBindTextureUnit(0, 0));
    OGL_CALL(glUseProgram(0));
}

void SimpleShapeApplication::draw_to_next_post_process(GLuint program, GLuint initTexture) {
    auto [w, h] = frame_buffer_size();

	std::swap(processed_input_, processed_output_);
    OGL_CALL(glUseProgram(program));
    OGL_CALL(glBindTextureUnit(0, initTexture ? initTexture : processed_input_));
    OGL_CALL(glBindImageTexture(0, processed_output_, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F));
    OGL_CALL(glDispatchCompute((w + 15) / 16, (h + 15) / 16, 1));
    OGL_CALL(glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT));
    OGL_CALL(glBindVertexArray(0));
    OGL_CALL(glBindTextureUnit(0, 0));
    OGL_CALL(glUseProgram(0));
}


void SimpleShapeApplication::key_callback(int key, int scancode, int action, int mods) {
    Application::key_callback(key, scancode, action, mods);
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        use_compute_ = !use_compute_;
        SPDLOG_INFO("Edge detection in the {} shader", use_compute_ ? "compute" : "fragment");
    }
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        use_vignette_ = !use_vignette_;
        SPDLOG_INFO("Vignette effect {}", use_vignette_ ? "enabled" : "disabled");
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        use_grayscale_ = !use_grayscale_;
        SPDLOG_INFO("Grayscale effect {}", use_grayscale_ ? "enabled" : "disabled");
    }
    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        use_negative_ = !use_negative_;
        SPDLOG_INFO("Negative effect {}", use_negative_ ? "enabled" : "disabled");
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        use_gaussian_ = !use_gaussian_;
        SPDLOG_INFO("Gaussian effect {}", use_gaussian_ ? "enabled" : "disabled");
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        use_optimized_gaussian_ = !use_optimized_gaussian_;
        SPDLOG_INFO("Optimized Gaussian effect {}", use_optimized_gaussian_ ? "enabled" : "disabled");
    }
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    // Binding the VAO will set up all the required vertex attribute arrays.
    M_ = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 PVM = camera()->projection() * camera()->view() * M_;
    OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, fbo_));
    OGL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    OGL_CALL(glNamedBufferSubData(u_trans_buffer_handle_, 0, 16 * sizeof(float), &PVM));
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_trans_buffer_handle_));
    for (auto m : meshes_)
        m->draw();
    OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0));
    OGL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));


    if (use_compute_) {
        draw_to_next_post_process(sobel_cs_program_, scene_color_);
        if (use_gaussian_)
			draw_to_next_post_process(gaussian_cs_program_);
        if (use_grayscale_)
            draw_to_next_post_process(grayscale_cs_program_);
        if(use_vignette_)
            draw_to_next_post_process(vignette_cs_program_);
		if (use_negative_)
			draw_to_next_post_process(negative_cs_program_);
		if (use_optimized_gaussian_)
			draw_to_next_post_process(optimized_gaussian_cs_program_);
        draw_full_screen(display_program_, processed_output_);
    }
    else {
        draw_full_screen(sobel_fs_program_, scene_color_);
    }
}
