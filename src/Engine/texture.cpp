#pragma once

#include <string>
#include "glad/gl.h"
#include "Application/gl_handle.h"
#include "../3rdParty/stb/stb_image.h"
#include "texture.h"
#include "spdlog/spdlog.h"
#include "array"

namespace xe {
gl::Texture create_texture(const std::string& name, bool is_sRGB, int desired_channels)
{
	gl::Texture tex_handle;
    GLint width, height, channels;

    auto texture_file = name;
    if (desired_channels < 3)
    {
		SPDLOG_ERROR("Texture {} must have at least 3 channels", texture_file);
        desired_channels = 0;
    }
        
    auto img = stbi_load(texture_file.c_str(), &width, &height, &channels, desired_channels);

    if (desired_channels != 0)
        channels = desired_channels;

    if (!img) {
        SPDLOG_ERROR("Could not read image from file `{}'", texture_file);
        return {};
    }
    SPDLOG_INFO("Loaded a {}x{} texture with {} channels", width, height, channels);

    OGL_CALL(glGenTextures(1, tex_handle.put()));
    OGL_CALL(glBindTexture(GL_TEXTURE_2D, tex_handle.get()));
    OGL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    OGL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLint format = 0;
    GLint internal_format = 0;
    if (is_sRGB)
    {
        if(channels == 3)
        {
            format = GL_RGB;
            internal_format = GL_SRGB8;
        }
        else if(channels == 4)
        {
            format = GL_RGBA;
            internal_format = GL_SRGB8_ALPHA8;
        }
    }
    else
    {
        if(channels == 3)
        {
            format = GL_RGB;
            internal_format = GL_RGB8;
        }
        else if(channels == 4)
        {
            format = GL_RGBA;
            internal_format = GL_RGBA8;
        }
    }
    OGL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, img));
    OGL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    OGL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    stbi_image_free(img);


    return tex_handle;
}

gl::Texture create_cube_map_texture(const std::array<std::string, 6>& faces, bool is_sRGB)
{
    gl::Texture textures;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, textures.put());
    stbi_set_flip_vertically_on_load(false);
    OGL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, textures.get()));

    int width = 0, height = 0, channels = 0;
    auto first_img = stbi_load(faces.at(0).c_str(), &width, &height, &channels, 0);
    if (!first_img) {
        SPDLOG_ERROR("Could not read image from file `{}'", faces.at(0).c_str());
        return {};
    }
    SPDLOG_INFO("Loaded a {}x{} texture with {} channels", width, height, channels);
    if (width != height) {
        SPDLOG_ERROR("Cube map face `{}' is not square", faces.at(0).c_str());
        stbi_image_free(first_img);
        return {};
    }
    OGL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    OGL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLint format = 0;
    GLint internal_format = 0;
    if (is_sRGB)
    {
        if (channels == 3)
        {
            format = GL_RGB;
            internal_format = GL_SRGB8;
        }
        else if (channels == 4)
        {
            format = GL_RGBA;
            internal_format = GL_SRGB8_ALPHA8;
        }
    }
    else
    {
        if (channels == 3)
        {
            format = GL_RGB;
            internal_format = GL_RGB8;
        }
        else if (channels == 4)
        {
            format = GL_RGBA;
            internal_format = GL_RGBA8;
        }
    }

    GLsizei mip_levels = 1;
    for (int dimension = width; dimension > 1; dimension /= 2)
        ++mip_levels;
    OGL_CALL(glTextureStorage2D(textures.get(), mip_levels, internal_format, width, height));
    // Upload first face (layer 0)
    OGL_CALL(glTextureSubImage3D(textures.get(), 0, 0, 0, 0, width, height, 1, format, GL_UNSIGNED_BYTE, first_img));
    stbi_image_free(first_img);

    // Load and upload remaining faces, ensuring size and channels match
    for (int i = 1; i < 6; ++i)
    {
        int w = 0, h = 0, ch = 0;
        auto img = stbi_load(faces.at(i).c_str(), &w, &h, &ch, 0);
        if (!img) {
            SPDLOG_ERROR("Could not read image from file `{}'", faces.at(i).c_str());
            return {};
        }
        if (w != width || h != height || ch != channels) {
            SPDLOG_ERROR("Cube map face `{}' has different size or channels", faces.at(i).c_str());
            stbi_image_free(img);
            return {};
        }
        SPDLOG_INFO("Loaded a {}x{} texture with {} channels", w, h, ch);
        OGL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        OGL_CALL(glTextureSubImage3D(textures.get(), 0, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, img));
        stbi_image_free(img);
    }

    // Generate mipmaps for the named texture
    OGL_CALL(glGenerateTextureMipmap(textures.get()));
    OGL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

    return textures;
}

}
