cmake_minimum_required(VERSION 3.16)

project(Graphics3DStudents CXX C)

add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}")

include(FetchContent)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(3rdParty_DIR ${SOURCE_DIR}/3rdParty)
include_directories(${3rdParty_DIR})
include_directories(${SOURCE_DIR})

add_compile_definitions(XE_DEBUG)
if (MSVC)
    add_compile_options(/utf-8)
endif ()

# GLAD
set(MAJOR 4)
set(MINOR 6)
if (MAJOR LESS 4 OR (MAJOR EQUAL 4 AND MINOR LESS 5))
    message(FATAL_ERROR "OpenGL ${MAJOR}.${MINOR} is not supported, the minimum version is 4.5.")
endif ()
option(GLAD_DEBUG "Use the GLAD debug loader, which checks for OpenGL errors after every call" OFF)

add_compile_definitions(MAJOR=${MAJOR})
add_compile_definitions(MINOR=${MINOR})

# Size of the application windows in pixels, passed to the SimpleShapeApplication constructor in each main.cpp.
set(WINDOW_WIDTH 1280)
set(WINDOW_HEIGHT 720)
add_compile_definitions(WINDOW_WIDTH=${WINDOW_WIDTH})
add_compile_definitions(WINDOW_HEIGHT=${WINDOW_HEIGHT})
set(GLAD_DIR ${3rdParty_DIR}/glad/glad_${MAJOR}_${MINOR})
if (${GLAD_DEBUG})
    set(GLAD_DIR ${GLAD_DIR}_debug)
endif ()
if (NOT EXISTS ${GLAD_DIR})
    message(FATAL_ERROR "No GLAD loader for OpenGL ${MAJOR}.${MINOR} in ${GLAD_DIR}.")
endif ()


file(GLOB_RECURSE glad_SOURCES "${GLAD_DIR}/glad/src/*.c")
# A static library, so that there is a single table of OpenGL function pointers, filled by gladLoadGL.
add_library(glad STATIC ${glad_SOURCES})
target_include_directories(glad PUBLIC ${GLAD_DIR}/glad/include)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Dependencies are downloaded as release archives of tagged versions, so everyone builds against the same versions.
# To update a library, change the tag in its URL and configure the project from scratch.

# Give extracted files the extraction time, so changing a URL triggers a rebuild.
if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif ()

# GLFW
FetchContent_Declare(
        glfw
        URL https://github.com/glfw/glfw/archive/refs/tags/3.5.1.tar.gz
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
        glm
        URL https://github.com/g-truc/glm/archive/refs/tags/1.0.3.tar.gz
)
FetchContent_MakeAvailable(glm)

# SPDLOG
FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.tar.gz
)
FetchContent_MakeAvailable(spdlog)

# CXXOPTS
FetchContent_Declare(
        cxxopt
        URL https://github.com/jarro2783/cxxopts/archive/refs/tags/v3.3.1.tar.gz
)
FetchContent_MakeAvailable(cxxopt)

# IMGUI
FetchContent_Declare(
        imgui
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.9b.tar.gz
)
FetchContent_MakeAvailable(imgui)

SET(IMGUI_DIR ${imgui_SOURCE_DIR})

find_package(OpenGL REQUIRED)

# My part

add_subdirectory(src/Application)

if (EXISTS ${SOURCE_DIR}/ObjectReader)
    add_subdirectory(src/ObjectReader)
else ()
    message("Warning: directory ObjectReader does not exists.")
endif ()

if (EXISTS ${SOURCE_DIR}/Engine)
    add_subdirectory(src/Engine)
else ()
    message("Warning: directory Engine does not exists.")
endif ()


if (EXISTS ${SOURCE_DIR}/OGL)
    add_subdirectory(src/OGL)
endif ()

if (EXISTS ${SOURCE_DIR}/3rdParty/MIKKTSpace)
    add_subdirectory(src/3rdParty/MIKKTSpace)
endif ()

if (EXISTS ${SOURCE_DIR}/tests)
    add_subdirectory(src/tests)
endif ()

#Assignments
set(ASSIGNMENTS
        Debugging
        00_Triangle 01_House 02_Colors 03_Indices 04_Uniforms 05_PVM 06_Pyramid 07_Resize 08_Zoom
        09_CameraMovement 10_Mesh 11_KdMaterial 12_oa_Textures 12_ob_ComputeShader 12_oc_PostProcessing 13_OBJReader
        15_oa_BlinnPhongDiffuse 15_ob_BlinnPhongSpecular 15_oc_BlinnPhongTextures 15_od_BlinnPhongNormalMap
        15_oe_BlinnPhongEnvironment)

set(ASSIGNMENTS_DIR ${SOURCE_DIR}/Assignments)
# The assignments are linked with the application library, and through it with GLAD, GLFW, GLM, ImGui and OpenGL.
link_libraries(application)

foreach (assignment ${ASSIGNMENTS})
    if (EXISTS ${ASSIGNMENTS_DIR}/${assignment})
        add_subdirectory(${ASSIGNMENTS_DIR}/${assignment})
    endif ()
endforeach ()
