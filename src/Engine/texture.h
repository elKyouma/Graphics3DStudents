#pragma once

#include <string>
#include "glad/gl.h"
#include "Application/gl_handle.h"

namespace xe {
    gl::Texture create_texture(const std::string& name, bool is_sRGB = true, int desired_channels = 0);
    gl::Texture create_cube_map_texture(const std::array<std::string, 6>& faces, bool is_sRGB = true);
}