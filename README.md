# Arcane-Engine
Arcane Engine is a real-time game engine developed and maintained by [Tyrannical Games](https://ershany.github.io/). A large focus of this project is to develop an in-house engine catered to building indie games. Arcane's development roadmap will be game development driven, meaning it will be influenced by actually making games and prioritizing features needed to ship great indie games at [Tyrannical Games](https://ershany.github.io/)

![Arcane Logo](https://user-images.githubusercontent.com/11170417/79673959-8a210800-81ac-11ea-9474-3a44ee664152.png)

# Engine Showcases
[Tech Reel 2019](https://www.youtube.com/watch?v=sc02q6MXvVo)

## Engine Features
- Forward + Deferred Rendering (3D API)
- Debug Rendering API
- Physically Based Rendering (Fresnel on Everything)
- Cook Torrance Specular BRDF w/ Lambertian Diffuse
- PBR Material System
- Parallax Occlusion Mapping
- Light Probes (IBL)
- Reflection Probes (IBL)
- Linear Lighting (w/ Gamma Correction)
- HDR w/ Reinhard Tonemapper
- Screen Space Ambient Occlusion (SSAO)
- MSAA (Forward Path Anti-Aliasing)
- FXAA (Deferred Path Anti-Aliasing)
- Directional, Point, and Spot Light Sources
- Shadows for directional, point, and spot lights
- Skybox Support
- Terrain Material Blending
- Water + Wave Simulation
- Post Processing (Bloom, Film Grain, Chromatic Aberration, Vignette)
- Entity Component System
- Basic Animation Clip Player
- Emission Maps
- Displacement Maps

## Engine Libraries
- OpenGL 4.3+
- GLFW Input + Window
- Model Loading via ASSIMP (with bone data)
- ImGUI for UI
- STB for Image Loading
- EnTT for Component Storage and Management

## Development
If you want to see what I am currently working on check out Arcane's roadmap:
- [Rendering Trello](https://trello.com/b/WzD2bYCD/arcane-rendering)
- [Engine Trello](https://trello.com/b/Giv16bRX/arcane-engine)

## Roadmap for Arcane's First Official Release (v0.1)
1. ~~Debug Rendering API~~
1. 2D Batch Rendering w/ 2D Rendering API (separate from 3D API)
1. Proper Signed Distance Field Text Rendering
1. Physics w/ Rigid Bodies and Collider Shapes
1. Add in Editor Support for Skybox Customization and support HDR
1. Basic Sound Engine w/ Editor Support
1. Animation Blending w/ Editor Support
1. Improved Continuous LOD Terrain w/ Editor Support
1. Camera Component and a Editor/Scene camera seperation
1. Being able to package a game executable that runs without the editor
1. Virtual Filesystem for Game Assets

## Contributing
1. [Fork it](https://github.com/Ershany/Arcane-Engine.git)
1. Create your feature branch (`git checkout -b new-feature-branch`)
1. Commit your changes (`git commit -m "A description for your feature"`)
1. Push to the branch (`git push origin new-feature-branch`)
1. Create a new Pull Request

## Sources
- LearnOpenGL
- Game Engine Architecture
- Real-Time Rendering
- GPU Gems
- GPU Pro
- Hazel Series (YouTube)
