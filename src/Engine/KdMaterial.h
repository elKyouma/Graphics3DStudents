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
class KdMaterial : public AbstractMaterial<KdMaterial> {

public:
    KdMaterial(const glm::vec4& Kd, bool use_vertex_colors, GLuint texture) :
        Kd_(Kd), use_vertex_colors_(use_vertex_colors), texture_(texture) {
    }

    explicit KdMaterial(const glm::vec4& Kd, bool use_vertex_colors) : KdMaterial(Kd, use_vertex_colors, 0u) {}

    explicit KdMaterial(const glm::vec4& Kd) : KdMaterial(Kd, false, 0u) {}
    
	static Material* create_from_mtl(const mtl_material_t& mat, std::string mtl_dir)
	{
        glm::vec4 color = get_color(mat.diffuse);
        color = glm::vec4(xe::srgb_inverse_gamma_correction(glm::vec3(color)), color.a);
        SPDLOG_INFO("Adding KdMaterial {}", glm::to_string(color));
        auto material = new xe::KdMaterial(color);
        if (!mat.diffuse_texname.empty()) {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.diffuse_texname, true);
            SPDLOG_INFO("Adding Texture {} {:1d}", mat.diffuse_texname, texture.get());
            if (texture) {
                material->set_texture(texture.get());
                material->map_Kd_texture_ = std::move(texture);
            }
        }

        return material;
	}

	void set_texture(GLuint texture) {
		texture_ = texture;
	}

    static void init()
    {
        create_program_in_engine({ 
                                {GL_VERTEX_SHADER, "Kd_vs.glsl"},
                                {GL_FRAGMENT_SHADER, "Kd_fs.glsl"} });
        create_material_uniform_buffer(2 * sizeof(glm::vec4));


        OGL_CALL(map_Kd_location_ = glGetUniformLocation(program(), "map_Kd"));
        if (map_Kd_location_ == -1) {
            SPDLOG_WARN("Cannot find map_Kd uniform");
        }

        xe::add_mat_function("KdMaterial", KdMaterial::create_from_mtl);
    }

    void KdMaterial::bind() const {
        OGL_CALL(glUseProgram(program()));

        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, material_uniform_buffer()));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), 0, sizeof(glm::vec4), &Kd_));
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), sizeof(glm::vec4), sizeof(int), &use_vertex_colors_));

        int use_texture = texture_ != 0;
        OGL_CALL(glNamedBufferSubData(material_uniform_buffer(), sizeof(glm::vec4) + sizeof(int), sizeof(int), &use_texture));

        if (texture_)
        {
            OGL_CALL(glUniform1i(map_Kd_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, texture_));
        }
    }

    void unbind() const override
    {
		OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0));
        if (texture_)
        {
            OGL_CALL(glUniform1i(map_Kd_location_, 0));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }
    }

private:
    inline static GLint map_Kd_location_ = -1;
    int use_vertex_colors_;
	glm::vec4 Kd_;
    GLuint texture_ = 0;
    gl::Texture map_Kd_texture_;
};
}