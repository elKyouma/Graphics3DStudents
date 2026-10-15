#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "AbstractMaterial.h"
#include "ObjectReader/sMesh.h"
#include "Application/gl_handle.h"
#include "Engine/utils.h"
#include "Engine/mesh_loader.h"
#include "texture.h"


namespace xe {
class BlinnPhongMaterial : public AbstractMaterial<BlinnPhongMaterial> {

public:
    BlinnPhongMaterial(const glm::vec4& Kd, bool use_vertex_colors, GLuint texture) :
        Kd_(Kd), use_vertex_colors_(use_vertex_colors), texture_Kd_(texture) {
    }

    explicit BlinnPhongMaterial(const glm::vec4& Kd, bool use_vertex_colors) : BlinnPhongMaterial(Kd, use_vertex_colors, 0u) {}

    explicit BlinnPhongMaterial(const glm::vec4& Kd) : BlinnPhongMaterial(Kd, false, 0u) {}
    
	static Material* create_from_mtl(const mtl_material_t& mat, std::string mtl_dir)
	{
        glm::vec4 color = get_color(mat.diffuse);
        color = glm::vec4(xe::srgb_inverse_gamma_correction(glm::vec3(color)), color.a);
        SPDLOG_INFO("Adding BlinnPhongMaterial {}", glm::to_string(color));
        auto material = new xe::BlinnPhongMaterial(color);
		material->Kd_ = color;
		material->Ka_ = get_color(mat.ambient);
		material->Ks_ = get_color(mat.specular);
		material->Ns_ = mat.shininess;
		material->illum_ = mat.illum;
        if (!mat.diffuse_texname.empty()) {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.diffuse_texname, true, 3);
            SPDLOG_INFO("Adding Texture {} {:1d}", mat.diffuse_texname, texture.get());
            if (texture) {
                material->set_map_Kd(texture.get());
                material->owned_textures_.push_back(std::move(texture));
            }
        }
        if (!mat.ambient_texname.empty()) {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.ambient_texname, true, 3);
            SPDLOG_INFO("Adding Texture {} {:1d}", mat.ambient_texname, texture.get());
            if (texture) {
                material->set_map_Ka(texture.get());
                material->owned_textures_.push_back(std::move(texture));
            }
        }
        if (!mat.specular_texname.empty()) {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.specular_texname, true, 3);
            SPDLOG_INFO("Adding Texture {} {:1d}", mat.specular_texname, texture.get());
            if (texture) {
                material->set_map_Ks(texture.get());
                material->owned_textures_.push_back(std::move(texture));
            }
        }
        if (!mat.specular_highlight_texname.empty()) {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.specular_highlight_texname, false, 3);
            SPDLOG_INFO("Adding Texture {} {:1d}", mat.specular_highlight_texname, texture.get());
            if (texture) {
                material->set_map_Ns(texture.get());
                material->owned_textures_.push_back(std::move(texture));
            }
        }
        return material;
	}

	void set_map_Kd(GLuint texture) {
		texture_Kd_ = texture;
	}
    
    void set_map_Ka(GLuint texture) {
        texture_Ka_ = texture;
    }

    void set_map_Ks(GLuint texture) {
        texture_Ks_ = texture;
    }

    void set_map_Ns(GLuint texture) {
        texture_Ns_ = texture;
    }

    static void init()
    {
        create_program_in_engine({ 
                                {GL_VERTEX_SHADER, "BlinnPhong_vs.glsl"},
                                {GL_FRAGMENT_SHADER, "BlinnPhong_fs.glsl"} });
        create_material_uniform_buffer(5 * sizeof(glm::vec4));


        OGL_CALL(map_Kd_location_ = glGetUniformLocation(program(), "map_Kd"));
        if (map_Kd_location_ == -1) {
            SPDLOG_WARN("Cannot find map_Kd uniform");
        }

        xe::add_mat_function("BlinnPhongMaterial", BlinnPhongMaterial::create_from_mtl);
    }

    void bind() const {
        OGL_CALL(glUseProgram(program()));

        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, material_uniform_buffer()));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 0, sizeof(glm::vec4), &Ka_));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), sizeof(glm::vec4), sizeof(glm::vec4), &Kd_));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 2 * sizeof(glm::vec4), sizeof(glm::vec4), &Ks_));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4), sizeof(float), &Ns_));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4) + sizeof(float), sizeof(int), &use_vertex_colors_));

        int use_texture_Kd = texture_Kd_ != 0;
        int use_texture_Ka = texture_Ka_ != 0;
        int use_texture_Ks = texture_Ks_ != 0;
        int use_texture_Ns = texture_Ns_ != 0;
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4) + sizeof(float) + sizeof(int), sizeof(int), &use_texture_Kd));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4) + sizeof(float) + 2 * sizeof(int), sizeof(int), &use_texture_Ka));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4) + sizeof(float) + 3 * sizeof(int), sizeof(int), &use_texture_Ks));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4) + sizeof(float) + 4 * sizeof(int), sizeof(int), &use_texture_Ns));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 3 * sizeof(glm::vec4) + sizeof(float) + 5 * sizeof(int), sizeof(int), &illum_));
        if (texture_Kd_)
        {
            OGL_CALL(glUniform1i(map_Kd_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture_Kd_));
        }
        if (texture_Ka_)
        {
            OGL_CALL(glUniform1i(map_Ka_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture_Ka_));
        }
        if (texture_Ks_)
        {
            OGL_CALL(glUniform1i(map_Ks_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture_Ks_));
        }
        if (texture_Ns_)
        {
            OGL_CALL(glUniform1i(map_Ns_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture_Ns_));
        }
    }

    void unbind() const override
    {
		OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0));
        if (texture_Kd_)
        {
            OGL_CALL(glUniform1i(map_Kd_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }
        if (texture_Ka_)
        {
            OGL_CALL(glUniform1i(map_Ka_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }
        if (texture_Ks_)
        {
            OGL_CALL(glUniform1i(map_Ks_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }
        if (texture_Ns_)
        {
            OGL_CALL(glUniform1i(map_Ns_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }
    }

private:
    inline static GLint map_Kd_location_ = -1;
    inline static GLint map_Ka_location_ = -1;
    inline static GLint map_Ks_location_ = -1;
    inline static GLint map_Ns_location_ = -1;
    int use_vertex_colors_;
    glm::vec4 Kd_, Ka_, Ks_;
    float Ns_;
    int illum_;
    GLuint texture_Kd_ = 0;
    GLuint texture_Ka_ = 0;
    GLuint texture_Ks_ = 0;
    GLuint texture_Ns_ = 0;
    std::vector<gl::Texture> owned_textures_;
};
}