# Resize

In this exercise we will change behavior of the application when resizing the window. But first let's gain some more 3D
perspective on the pyramid.

Start by copying the `06_Pyramid` assignment to a new `07_Resize` directory, as described in the
[Preparing the assignments](../README.md#preparing-the-assignments) section.

Please change the location of the camera in the `init` method of the `SimpleShapeApplication` class to (2.0,1.0,2.0)
looking at (0,0,0) with up vector (0,0,1).
Now the pyramid should be visible from the side.

Try to resize the display window. The `Application` class keeps the OpenGL viewport equal to the size of the
framebuffer (the part of the window we draw to), so the pyramid scales together with the window. However, its
proportions are not preserved. That is because the aspect ratio of the viewport changes with the window, while the
aspect ratio of the perspective projection set up in the call to the `glm::perspective` function in the `init` method
stays the same. We will change this so that the projection follows the size of the window.

For this purpose, we will use the virtual method
`Application::framebuffer_resize_callback(int w, int h)`, which is called during each frame buffer size change with
the `w` and `h` parameters defining the new buffer sizes. The `Application` class has already updated the viewport
when it is called.
This method can be overridden in classes derived from the `Application` class, such as
`SimpleShapeApplication`, which you use in your exercises.

1. First add
   ```c++
   void framebuffer_resize_callback(int w, int h) override;
   ```
   to the `public` part of the `SimpleShapeApplication` class declaration in the `app.h` file indicating that we will
   substitute our own implementation of this method.
   Then add the definition in the  `app.cpp` file:
   ```c++
   void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
       Application::framebuffer_resize_callback(w, h);
   }
   ```
   When the window is minimized, the framebuffer can have zero size (this happens e.g. on Windows). Such a size is not
   useful for rendering, and later on it would make the aspect ratio `w/h` undefined, so ignore it by returning early
   at the beginning of the method, after calling the base class method:
   ```c++
   if (w <= 0 || h <= 0)
       return;
   ```

2. To preserve the proportions of the pyramid, we need to change the aspect ratio of the perspective projection each time the
   framebuffer is resized. Because that means changing the perspective matrix from within
   the `framebuffer_resize_callback` method, this matrix must be accessible there, so it cannot be defined in the `init`
   method anymore. Another issue is that once we change the perspective matrix, we need to send the new PVM matrix to
   the shader.

   To solve this problem, we will define several new fields in the `SimpleShapeApplication` class in the `app.h` file
   (you will need to include the `glm/glm.hpp` header file):

   ```c++
   float fov_;
   float aspect_;
   float near_;
   float far_;

   glm::mat4 P_;
   glm::mat4 V_;
   glm::mat4 M_;

   ```

   In the `init` method of the `app.cpp` please assign the same values to them as before (remember that the field of view is stored in
   radians, e.g. `fov_ = glm::radians(45.0f)`) and change the code accordingly to use those new fields.
   Please delete the unnecessary code from the `init` method i.e., the `M`, `V` and `P` variables.

   At this point we still create the PVM matrix in the  `init` method and send it to the uniform buffer there.
   We will change this in the next step.

3. The PVM matrix needs to be recalculated and loaded to uniform buffer every time the framebuffer is resized. We could
   do this in the `framebuffer_resize_callback` method, but in anticipation of further use, we will move this code to
   the `frame` method. For that to be successful we need a way to access the uniform buffer handle in this method, so
   again we will create a new field in the `SimpleShapeApplication` class in the `app.h` file:
   ```c++
   GLuint u_trans_buffer_handle_;
   ```
   Now in the `init` method use this variable to store the handle of the uniform buffer. Change the code accordingly,
   and remove the old handle variable from the `init` method. Check if everything works as before.

   The buffer will now be overwritten in every frame, so change the last argument of its `glNamedBufferData` call, the
   _usage hint_, to `GL_DYNAMIC_DRAW`. The hint does not change what the buffer can do, it only tells the driver how
   the buffer will be used, so that it can place it in the most suitable memory. `GL_STATIC_DRAW` means "written once,
   used many times", `GL_DYNAMIC_DRAW` "written repeatedly, used many times".

   So please move the `P_` and `PVM` calculation to the `frame` method and remove the old code from the `init` method
   and load the matrix to the transformations uniform buffer using the `glNamedBufferSubData` function.

4. Then, in the `frame` method, bind the uniform buffer to the `Transformations` interface block using
   the `glBindBufferBase` function. Remove this code from the `init` method. After executing the draw call unbind the buffer by
   binding buffer `0` to the same binding point: `glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0)`.

5. Finally, in the `framebuffer_resize_callback` add the code that changes the `aspect_`. This will be used to calculate
   the new perspective matrix in the `frame` method. The `fov_`, `near_` and `far_` values should remain unchanged.

   Now try to resize the window.
   The pyramid should scale with the window and its proportions should be preserved.
   To be more precise, the pyramid will scale only with the vertical size of the window, because the `fov` parameter
   of `glm::perspective` is the vertical field of view. Changing the horizontal size will only change how much of the
   scene is visible on the sides.
