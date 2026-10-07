# Setting up VS Code

This page describes how to set up [Visual Studio Code](https://code.visualstudio.com/) for working on the
assignments. Install the compiler, CMake and Git first, as described in the [main README](./README.md#building).

## 1. Open the project

Use **File | Open Folder...** and choose the folder with the cloned repository (the one containing the top
`CMakeLists.txt`). Do not open a subfolder such as `src/Assignments/00_Triangle`: CMake Tools would not find the
project.

## 2. Install the extensions

After opening the folder VS Code should offer to install the recommended extensions. Accept. If it does not,
open the Extensions panel (`Ctrl+Shift+X`) and install:

- **CMake Tools** (`ms-vscode.cmake-tools`), which configures, builds and runs the project,
- **C/C++** (`ms-vscode.cpptools`), which provides code completion, error highlighting and the debugger,
- **Markdown All in One** (`yzhang.markdown-all-in-one`), which helps with editing Markdown files such as your
  notes. The assignment descriptions are Markdown files too; press `Ctrl+Shift+V` to see one rendered.

## 3. Choose a kit and configure

The repository contains a `.vscode/settings.json` file, so the project starts configuring itself when you open the
folder. The first configuration downloads the libraries used by the project, so you need an internet connection and
some patience.

When asked to choose a **kit** (the compiler), pick:

- on Linux: `GCC` or `Clang` (a C++17 compiler is required),
- on Windows: the Visual Studio kit ending in `amd64`, e.g. *Visual Studio Community 2022 Release - amd64*, not
  the `x86` or `arm64` ones.

If you are asked whether CMake Tools may configure IntelliSense, choose **Allow**. Otherwise the editor marks the
includes of the downloaded libraries as errors, although the project builds fine.

If nothing happens, or you want to configure again, press `Ctrl+Shift+P` and run **CMake: Configure**.
To start from scratch, run **CMake: Delete Cache and Reconfigure**.

## 4. The status bar

The bar at the bottom of the window contains the CMake controls:

| Item                  | What it does                                                          |
|-----------------------|-----------------------------------------------------------------------|
| Kit, e.g. `GCC 13`    | Choose the compiler.                                                  |
| `Debug` / `Release`   | Choose the build type. Use `Debug` while developing and debugging.    |
| `[all]`               | Choose which assignment to build, e.g. `00_Triangle`.                 |
| Build button          | Build the selected target.                                            |
| Launch target (▷ row) | Choose which program the ▷ and bug buttons start.                     |
| ▷ button              | Run the launch target.                                                |
| Bug button            | Run the launch target in the debugger.                                |

Select the same assignment as both the build target and the launch target. Building `[all]` compiles every
assignment, which takes much longer.

Which items are shown is controlled by the `cmake.options` entries in `.vscode/settings.json`. You can also hide or
show items by right-clicking the status bar. The Settings UI (`Ctrl+,`, then search for `cmake status bar`) offers the
same choice for all items at once.

## 5. Build, run and debug

1. Select the assignment as build and launch target in the status bar (or in the CMake panel, the CMake icon in the
   left bar).
2. Press the Build button (or `F7`).
3. Press ▷ to run, or the bug icon to debug. In the debugger you can set breakpoints by clicking to the left of the
   line numbers.

The same commands are in the Command Palette (`Ctrl+Shift+P`): **CMake: Build**, **CMake: Run Without Debugging**
and **CMake: Debug**.

The program runs in the build folder of the assignment, e.g. `build/src/Assignments/00_Triangle/`, so screenshots
(`Ctrl-S` in the program) are saved there.

## 6. Adding a new assignment

A new assignment folder appears in the list of targets only after you add its name to the `ASSIGNMENTS` variable in
the top `CMakeLists.txt` (see [Assignments/README.md](./Assignments/README.md)) and configure the project again with
**CMake: Configure**.

## Troubleshooting

- **No CMake items in the status bar.** The project is not configured yet, or you opened the wrong folder. See
  steps 1 and 3.
- **The list of targets is empty or does not contain your assignment.** Check that its name is listed in
  `ASSIGNMENTS` and run **CMake: Configure**.
- **Red squiggles under `#include` of library headers.** Allow CMake Tools to configure IntelliSense. If you
  declined, run **C/C++: Select a Configuration...** from the Command Palette and choose **CMake Tools**.
- **Configuration fails while downloading.** Check your internet connection and run
  **CMake: Delete Cache and Reconfigure**.
- **The program does not start because OpenGL 4.6 is not available.** See
  [OpenGL version](./README.md#opengl-version) in the main README.
