# Arcane-Engine
Arcane Engine is a real-time, physically based renderer built with OpenGL. A large focus of this project is to implement modern real-time graphical effects, while also making it easy to read and understand. Arcane supports forward and deferred rendering, by using the benefits of each to its advantage.

![Arcane Logo](https://user-images.githubusercontent.com/11170417/79674060-a3768400-81ad-11ea-9011-a55b27c001dd.png)

# Render Examples
[Tech Reel 2019](https://www.youtube.com/watch?v=sc02q6MXvVo)

# Features

## Rendering
- Forward + Deferred Rendering
- Physically Based (Fresnel on Everything)
- Cook Torrance Specular BRDF w/ Lambertian Diffuse
- PBR Material System
- Parallax Occlusion Mapping
- Light Probes (IBL)
- Reflection Probes (IBL)
- Linear Lighting (w/ Gamma Correction)
- HDR w/ Reinhard Tonemapper
- Screen Space Ambient Occlusion (SSAO)
- MSAA (Forward Anti-Aliasing)
- FXAA (Deferred Anti-Aliasing)
- Directional, Point, and Spot Light Sources
- Directional Light Shadowmapping
- Skybox Support
- Terrain Material Blending
- Water + Wave Simulation
- Post Processing (Film Grain, Chromatic Aberration, Vignette)

## Engine
- OpenGL 4.3+
- GLFW Input + Window
- Model Loading via ASSIMP
- ImGUI for UI
- SOIL for Image Loading

## Textbook Sources
- LearnOpenGL
- Game Engine Architecture
- Real-Time Rendering
- GPU Gems
- GPU Pro

## Development
If you want to see what I am currently working on check out Arcane's roadmap:
- [Rendering Trello](https://trello.com/b/WzD2bYCD/arcane-rendering)
- [Engine Trello](https://trello.com/b/Giv16bRX/arcane-engine)

I plan on adding many different features to the forward & deferred renderers. Here are some of the few things I am really looking forward to adding:
1. Vulkan Support
1. Decals
1. Particles
1. SSR
1. Volumetric Effects
1. Reverse Z Buffer
1. Virtual Texture System (Terrain)

## Contributing
1. [Fork it](https://github.com/Ershany/Arcane-Engine.git)
1. Create your feature branch (`git checkout -b new-feature-branch`)
1. Commit your changes (`git commit -m "A description for your feature"`)
1. Push to the branch (`git push origin new-feature-branch`)
1. Create a new Pull Request
