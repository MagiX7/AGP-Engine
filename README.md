# AGP-Engine

## Overview
This project is a Graphics Engine made in OpenGL and C++ from scratch. It explores different graphics techniques such as Deferred Rendering, and PBR and SSAO in a near future.

## Controls
The camera can be moved with WASDEQ and rotated moving the mouse. All of this while holding the right mouse button.
 
 ## UI
 The UI consists in:
  - Main Menu Bar: From here a brief OpenGL info panel can be seen alongside lights creation.
  - Viewport Panel: The scene is rendered to a texture, later passed to ImGui to have it on a window
  - Hierarchy Panel: Here all the entities in the scene can be seen, selected and manipulated (transform) in the bottom-right panel named _Model_
  - Renderer Panel: Can choose the render path as well as the current render target (albedo, normals, position, depth)
  - Lighting Panel: This panel shows all the lights present in the scene. Also allows editing.

Link to the repository [here](https://github.com/MagiX7/AGP-Engine).
