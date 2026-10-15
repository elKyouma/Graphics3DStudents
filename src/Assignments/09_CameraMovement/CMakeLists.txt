cmake_minimum_required(VERSION 3.15)
project(09_CameraMovement)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}" PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${PROJECT_NAME}
        main.cpp
        app.h
        camera_controller.h
        app.cpp
        )
        
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)


