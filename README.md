# AGP-Engine

## Overview
This project is a Graphics Engine made in OpenGL and C++ from scratch. It explores different graphics techniques such as Deferred Rendering, PBR and SSAO.

## Controls
The camera can be moved with WASDEQ and rotated moving the mouse. All of this while holding the right mouse button.
Orbitting around the (0,0,0) can be achieved by pressing the LEFT ALT button and moving the mouse holding the left button.
 
## UI
The UI consists in:
  - Main Menu Bar: From here a brief OpenGL info panel can be seen alongside lights creation.
  - Viewport Panel: The scene is rendered to a texture, later passed to ImGui to have it on a window
  - Hierarchy Panel: Here all the entities in the scene can be seen, selected and manipulated (transform) in the bottom-right panel named _Model_
  - Renderer Panel: Can choose the render path as well as the current render target (albedo, normals, position, depth)
  - Lighting Panel: This panel shows all the lights present in the scene. Also allows editing.
  - SSAO Panel: Availability to tweak SSAO parameters, such as radius or intensity, among other ones.

## Techniques
### Deferred Rendering
The rendering path can be switched from forward (classic) to deferred, storing the relevant data into a framebuffer and applying a single pass where all the lighting gets calculated together.

### Physically-Based Renderer (PBR)
Another technique that has been implemented is PBR, which is a shading model that provides a more accurate representation of how light interacts with material depending on its properties.
By default each objects follows the PBR pipeline, which allows to select the different textures (Albedo, Normals, Metallic, Roughness and Ambient Occlusion) from the inspector panel.

With the PBR shader selected and all textures applied:
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/pbr.png">

The options related to PBR to edit is the material itself:  
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/pbr_options.png">

- All textures can be changed with the button "Change" or removed with the button "Remove". If "Change" button is clicked, a window with all the textures will be shown in order to pick one.

The shaders used for PBR are:
- If in deferred: [Deferred Pass Shader](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/deferred_pass.glsl)
- If in forward: [Forward Pass Shader](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/forward_pass.glsl)

### Image-Based Lighting (IBL)
In addition to PBR, IBL (Image-Based Lighting) has been implemented. This technique aims to light objects with the surrounding environment, which is known as Skybox.

The skybox drawn looks like this:
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/skybox_no_reflections.png">

With reflections on the objects:
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/skybox_with_reflections.png">

And disabled like this:
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/no_skybox.png">


The options for handling the skybox are the following:  
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/skybox_options.png">  
  
- Render Skybox: Defines wether to render the skybox or not. In deferred path saves an extra pass.
- Enable Reflections: Defines wether the objects get affected by the skybox light.

The shaders used are:
- [Converting HDR to Cubemap](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/Skybox/hdr_to_cubemap.glsl)
- [Calculate the Irradiance Map](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/Skybox/irradiance_skybox.glsl)
- [Generate the prefiltered map](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/Skybox/prefilter.glsl)
- [BRDF calculation](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/Skybox/brdf.glsl)
- [Skybox Rendering](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/Skybox/skybox.glsl)

### Screen-Space Ambient Occlusion (SSAO)
The last developed technique is SSAO, a post-processing effect that, essentially, defines the points on the geometry that are less prone to receive light.

The effect enabled looks like this:
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/ssao.png">

And disabled like this:
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/no_ssao.png">

The options for tweaking this effect are:  
<img src="https://github.com/MagiX7/AGP-Engine/blob/main/docs/Screenshots/ssao_options.png">  

- Enabled: Defines if the effect is enabled or not.
- Radius: The zone covered by the occlusion. Higher values will make the effect expand.
- Strength: The intensity of the effect. Higher values will boost SSAO effect.
- Bias: The threshold to apply the occlusion. Higher values will lead to less SSAO.
- Noise Size: Changes the noise size, which allows to randomize a little the effect.

The shaders used are:
- [A specific Geometry Pass](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/ssao_geometry_pass.glsl)
- [The SSAO Algorithm](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/ssao.glsl)
- [Blur SSAO result](https://github.com/MagiX7/AGP-Engine/blob/main/Engine/Assets/Shaders/ssao_blur.glsl)


## Team Members
[David González López](https://github.com/MagiX7)  

___

Link to the repository [here](https://github.com/MagiX7/AGP-Engine).
