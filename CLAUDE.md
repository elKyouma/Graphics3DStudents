# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this repository is

Student starter code for a university course on 3D graphics with OpenGL 4.5/4.6 in C++17. Students clone it, then
complete a sequence of incremental assignments. The assignment descriptions live in `Assignments/<name>/README.md`
(rendered images next to them); the code lives in `src/Assignments/<name>/`. Only `00_Triangle` (starting point) and
`Debugging` (example for `Assignments/DEBUGGING.md`) exist in `src/Assignments` initially. Documentation is written for
students, so keep added prose at that level. macOS is not supported (OpenGL max 4.1 there).

## Build and run

```shell
mkdir build && cd build
cmake ..                 # first configure downloads GLFW, GLM, spdlog, cxxopts, ImGui via FetchContent (needs network)
cmake --build . -j 4     # or: cmake --build . --target 00_Triangle
./src/Assignments/00_Triangle/00_Triangle
```

CLion uses `cmake-build-debug/` (gitignored). There is no test suite or linter; `src/tests` is added only if it exists.

CMake knobs in the top `CMakeLists.txt`:
- `set(MINOR 6)` → `5` for GPUs/drivers without OpenGL 4.6. Selects the GLAD loader in `src/3rdParty/glad/glad_<MAJOR>_<MINOR>`.
- `-DGLAD_DEBUG=ON` uses the `_debug` GLAD variant that checks errors after every GL call.
- `WINDOW_WIDTH`/`WINDOW_HEIGHT`, `MAJOR`/`MINOR`, `ROOT_DIR` (repo root, for locating `Models/`) are passed as compile definitions.
- Dependency versions are pinned by `GIT_TAG`; changing one requires reconfiguring from scratch.

Runtime shortcuts in every program: Ctrl-Q quit, Ctrl-S screenshot (`screenshot_<n>.png` in cwd), Ctrl-F RenderDoc capture (Linux).

## Adding an assignment

```shell
python3 ./scripts/copy_assignment.py 00_Triangle 01_House   # copies the folder and renames the CMake project
```

A folder under `src/Assignments` is built **only if its name is listed in the `ASSIGNMENTS` variable** of the top
`CMakeLists.txt`; use the exact names from `Assignments/README.md`. The executable name equals the folder name.
Assignments are chained: each starts as a copy of the previous one. Note the side branch: `12_ob_ComputeShader` and
`12_oc_PostProcessing` branch off `12_oa_Textures`, and `13_OBJReader` continues from `12_oa_Textures`, not from them.

## Architecture

Libraries (all in namespace `xe`), from bottom to top:

- `src/Application` → `application` library. `xe::Application` owns the GLFW window, GL context, ImGui and the main
  loop (`init()` once, then per frame `frame()`, `imgui_info()`, `imgui()`; `cleanup()` at exit; virtual `*_callback`
  methods for input/resize). Also: `xe::utils::create_program` (compiles shaders from files), the `OGL_CALL` macro and
  error reporting (`utils.h`), `xe::gl::Handle` RAII wrappers for GL objects (`gl_handle.h`), uniform-block helpers,
  and `RegisteredObject` — a global registry whose instances (meshes, materials) must be heap-allocated with `new`
  and are deleted by `Application::cleanup()` while the context is still alive. Every assignment links this library
  automatically via `link_libraries(application)`.
- `src/ObjectReader` → `objreader`: OBJ/MTL loading via tinyobjloader into `sMesh`, tangents via MikkTSpace.
- `src/OGL` → `OGL`: small OpenGL helper functions.
- `src/Engine` → `Engine`: `Mesh` (buffers + submeshes with materials), `Material`/`AbstractMaterial<D>` (CRTP base
  with per-material-type shared resources such as the shader program), `PointLight`, `mesh_loader`. From `10_Mesh`
  onward an assignment links it with `target_link_libraries(${PROJECT_NAME} PUBLIC Engine spdlog::spdlog)`.
- `src/3rdParty`: vendored GLAD, stb, tinyobjloader, MikkTSpace, RenderDoc header.

Each assignment is `main.cpp` (constructs `SimpleShapeApplication` and calls `run`), `app.h/app.cpp` (a subclass of
`xe::Application`), and `shaders/*.glsl`. Shaders are loaded at runtime from `PROJECT_DIR "/shaders/..."` (absolute
source path baked in by the assignment's CMakeLists), so they are not copied to the build directory. Models and
textures are in the top-level `Models/` folder.

## Code conventions (from `Assignments/GUIDELINES.md`)

- Wrap **every** OpenGL call in `OGL_CALL(...)`. It is a single statement, so declare result variables outside:
  `GLint loc; OGL_CALL(loc = glGetUniformLocation(p, "x"));`. Define `DEBUG_NO_ABORT` to log instead of abort, or
  `NO_OGL_CALL` to disable checks.
- Uniform blocks use the `std140` layout (see `Assignments/04_Uniforms/STD140.md`).
- Do not leave commented-out code.
