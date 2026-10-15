#pragma once

#include <string>
#include "glad/gl.h"
#include "Application/gl_handle.h"
#include "../3rdParty/stb/stb_image.h"
#include "texture.h"
#include "spdlog/spdlog.h"

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
}