# Assignments

This folder contains the descriptions of assignments to be performed during this course. Assignments are _incremental_.
Each new assignment will be based on the previous ones. For this purpose, you will copy assignments to new folders. How
to do this is described in the [Preparing the assignments](#preparing-the-assignments) section below. Each assignment
will be scored. The number of points assigned to each assignment will appear in the Teams
spreadsheet [zadania](https://ujchmura.sharepoint.com/:x:/r/teams/Section_628677_1/Shared%20Documents/General/zadania.xlsx?d=w8aa96ddd9bad4e4bb8f0b9e1cb6ac4f0&csf=1&web=1&e=0I5BJW)
located in the general channel of the
[course team](https://teams.microsoft.com/l/team/19%3A-rUP1GUAhYdDATjI1djR2MqYPLl8j9igT6vpYDIsdCw1%40thread.tacv2/conversations?groupId=7f734604-dc54-4ac6-9714-3ce4f58e01ba&tenantId=eb0e26eb-bfbe-47d2-9e90-ebd2426dbceb) on Microsoft Teams.

As I am still working on the assignments, not all of them have points assigned yet; I hope to finish this by
18 October 2026. The final grade will be calculated based on the total number of points. However, to pass you have to
reach at least the [`Diffuse`](15_oa_BlinnPhongDiffuse/README.md) assignment (`15_oa_BlinnPhongDiffuse`) and get at
least 50% of the points. The exact scale is not yet set, as it may depend slightly upon your performance; more details
will be given together with the points.

Assignments will have a due date, after which they will no longer be accepted.
You can submit subsequent assignments, but all prior assignments must be included in them.
As you have to do all the assignments up to `Diffuse` anyway, you may as well try to submit them on time. The
due dates will be given in the Teams spreadsheet [zadania](https://ujchmura.sharepoint.com/:x:/r/teams/Section_628677_1/Shared%20Documents/General/zadania.xlsx?d=w8aa96ddd9bad4e4bb8f0b9e1cb6ac4f0&csf=1&web=1&e=0I5BJW) in the [course team](https://teams.microsoft.com/l/team/19%3A-rUP1GUAhYdDATjI1djR2MqYPLl8j9igT6vpYDIsdCw1%40thread.tacv2/conversations?groupId=7f734604-dc54-4ac6-9714-3ce4f58e01ba&tenantId=eb0e26eb-bfbe-47d2-9e90-ebd2426dbceb) by 18 October 2026.

In the assignment descriptions, I will often omit the arguments of various OpenGL functions. Your task will be to
complete them based on the documentation. Usually, just search for the name of the function to get a link
to the [OpenGL® 4 Reference Pages](https://registry.khronos.org/OpenGL-Refpages/gl4/).

Please follow the [guidelines](GUIDELINES.md) when working on the assignments. How to find and fix errors in your
code is described in [DEBUGGING.md](DEBUGGING.md).

## List of assignments

| Folder | Assignment | Description |
|---|---|---|
| `00_Triangle` | [Triangle](00_Triangle/README.md) | Starting point (not graded): a red triangle on a gray background. |
| `01_House` | [House](01_House/README.md) | Experiment with the vertex data and shaders, then draw a house from three triangles. |
| `02_Colors` | [Colors](02_Colors/README.md) | Add a per-vertex color attribute to the vertex buffer and pass it through the shaders. |
| `03_Indices` | [Indices](03_Indices/README.md) | Use an index buffer to remove repeated vertices. |
| `04_Uniforms` | [Uniforms](04_Uniforms/README.md) | Pass data to shaders in uniform interface blocks with `std140` layout: mix colors and move the house. |
| `05_PVM` | [Projection - View - Model](05_PVM/README.md) | Transform vertices with the model, view and projection matrices. |
| `06_Pyramid` | [Pyramid](06_Pyramid/README.md) | Build a 3D pyramid and turn on the depth buffer and back-face culling. |
| `07_Resize` | [Resize](07_Resize/README.md) | Update the viewport and the aspect ratio of the projection when the window is resized. |
| `08_Zoom` | [Zoom](08_Zoom/README.md) | Move the camera into a `Camera` class and zoom with the mouse wheel by changing the field of view. |
| `09_CameraMovement` | [Camera Movement](09_CameraMovement/README.md) | Rotate the camera around the scene center with the mouse, using a camera controller. |
| `10_Mesh` | [Mesh](10_Mesh/README.md) | Move the camera classes into the engine and replace raw buffer calls with the `Mesh` class. |
| `11_KdMaterial` | [KdMaterial](11_KdMaterial/README.md) | Add materials: a `KdMaterial` class that sets the diffuse color of submeshes. |
| `12_oa_Textures` | [Textures](12_oa_Textures/README.md) | Take the diffuse color from a texture using UV coordinates, and apply gamma correction. |
| `12_ob_ComputeShader` | [Processing textures with a compute shader](12_ob_ComputeShader/README.md) | Process a texture on the GPU with a compute shader: grayscale and negative. |
| `12_oc_PostProcessing` | [Post-processing](12_oc_PostProcessing/README.md) | Render into a texture with a framebuffer object, then post-process it: color blindness simulation and Sobel edge detection. |
| `13_OBJReader` | [Reading Wavefront OBJ files](13_OBJReader/README.md) | Load models and materials from OBJ/MTL files, with RAII ownership of OpenGL objects. |
| `15_oa_BlinnPhongDiffuse` | [Diffuse lighting](15_oa_BlinnPhongDiffuse/README.md) | Light the scene with the ambient and diffuse terms of the Blinn-Phong model and point lights. |
| `15_ob_BlinnPhongSpecular` | [Specular](15_ob_BlinnPhongSpecular/README.md) | Add the specular term of the Blinn-Phong model using the half-vector. |
| `15_oc_BlinnPhongTextures` | [Blinn-Phong textures](15_oc_BlinnPhongTextures/README.md) | Read the `Ka`, `Ks` and `Ns` material parameters from textures. |
| `15_od_BlinnPhongNormalMap` | [Normal maps](15_od_BlinnPhongNormalMap/README.md) | Add surface detail with a normal map in tangent space. |
| `15_oe_BlinnPhongEnvironment` | [Environment maps](15_oe_BlinnPhongEnvironment/README.md) | Add reflections from a cube-map environment and draw it as a skybox. |

`12_ob_ComputeShader` and `12_oc_PostProcessing` form a side branch: `12_ob_ComputeShader` starts from
`12_oa_Textures`, and `12_oc_PostProcessing` from `12_ob_ComputeShader`, but the main line continues with
`13_OBJReader`, which again starts from `12_oa_Textures`. Do not carry the compute shader and the post-processing into
the later assignments.

The `src/Assignments/Debugging` program is the example used in [DEBUGGING.md](DEBUGGING.md).

## Turning in assignments

You will keep the assignments in your repositories. Please create a **private** repository on
[GitHub](https://github.com/) and connect it as a remote repository to the local repository where you store your code.
Create it **empty**, without a README, license or `.gitignore` file, otherwise pushing your code to it will fail.
Pushing to GitHub requires authentication with an [SSH
key](https://docs.github.com/en/authentication/connecting-to-github-with-ssh) or a [personal access
token](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/managing-your-personal-access-tokens);
your GitHub password will not work.

You can connect the repositories e.g. this way. First clone my repository as described in the [README.md](../README.md)
file.

```shell
git clone https://github.com/pbialas7-Lectures/Graphics3DCode.git
```

Check if everything builds all right, then rename the remote repository

```shell
git remote rename origin origin.lecture
```

and add your GitHub repository as a remote repository

```shell
git remote add origin <your GitHub repository>
```

And finally, push the code to your repository

```shell
git push -u origin main
```

In this way, you will be able to pull my changes from my repository and push your changes to your repository. To pull my
changes, you will have to use

```shell
git pull origin.lecture main
```

After creating your repository, please give me permission to read and write to it: in the repository on GitHub go to
*Settings → Collaborators → Add people* and add the user `pbialas7`. Please add the URL to the repository to the Teams
sheet [repozytoria](https://ujchmura.sharepoint.com/:x:/r/teams/Section_628677_1/Shared%20Documents/General/repozytoria.xlsx?d=w8d21fb3ed4354a8985d2e116ea752cb8&csf=1&web=1&e=GDwpxX).

The assignments can and are even recommended to be done in pairs. You just need to report it to me in advance and keep
the code in one repository. There is space in the sheet to enter two people for one repository.

## Preparing the assignments

Before starting each assignment, you should copy the directory containing the previous assignment. Specifically, you
should not modify anything in the `src/Assignments/00_Triangle` folder, but copy it to the `src/Assignments/01_House`
folder. I have provided a Python script that does this:

```shell
python3 ./scripts/copy_assignment.py 00_Triangle 01_House
```

On Windows run it with `py` instead of `python3`. The script copies the folder and changes the project name in
`src/Assignments/01_House/CMakeLists.txt` from `00_Triangle` to `01_House`. The project name is also the name of the
program, so each program has the same name as its folder.

You can also do this by hand. On Linux copy the folder with

```shell
cp -r src/Assignments/00_Triangle src/Assignments/01_House
```

and then change the project name in `src/Assignments/01_House/CMakeLists.txt` from `00_Triangle` to `01_House`.

Use exactly the folder names given in the [list of assignments](#list-of-assignments): only the folders listed in the
`ASSIGNMENTS` variable in the top `CMakeLists.txt` are built. After copying, configure the project again (in VS Code and
CLion this usually happens automatically), so that the new assignment appears in the list of targets.
