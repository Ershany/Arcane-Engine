# Arcane-Engine
Arcane Engine is a real-time, physically based renderer built with OpenGL. A large focus of this project is to implement modern real-time graphical effects, while also making it easy to read and understand. Arcane supports forward and deferred rendering, by using the benefits of each to its advantage.

![arcaneengine](https://user-images.githubusercontent.com/11170417/27188219-75d8814e-51bb-11e7-907b-1be35da8de38.png)

# Render Samples
![sponza](https://user-images.githubusercontent.com/11170417/62843504-41fa2f00-bc88-11e9-9e9f-8b27d2873298.png)
![ArcanePBR+IBL](https://user-images.githubusercontent.com/11170417/58596408-f7007c00-8241-11e9-895c-cab452548672.png)

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
If you want to see what I am currently working on, check out Arcane's roadmap on Trello:

https://trello.com/b/WzD2bYCD/arcane-engine

I plan on adding many different features to the forward & deferred renderers. Here are some of the few things I am really looking forward to adding:
1. Decals
1. Particles
1. SSR
1. Volumetric Effects
1. Reverse Z Buffer
1. Virtual Texture System (Terrain)

## Contributing
1. Fork it (https://github.com/Ershany/Arcane-Engine.git)
1. Create your feature branch (`git checkout -b new-feature-branch`)
1. Commit your changes (`git commit -m "A description for your feature"`)
1. Push to the branch (`git push origin new-feature-branch`)
1. Create a new Pull Request
