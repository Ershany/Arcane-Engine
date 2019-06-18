# Arcane-Engine
Arcane Engine is a real-time, physically based renderer built with OpenGL. A large focus of this project is to implement modern real-time graphical effects, while also making it easy to read and understand. Arcane supports forward and deferred rendering, by using the benefits of each to its advantage.

![arcaneengine](https://user-images.githubusercontent.com/11170417/27188219-75d8814e-51bb-11e7-907b-1be35da8de38.png)

# Render Samples
![sponza](https://user-images.githubusercontent.com/11170417/44693363-60dec180-aa35-11e8-8e06-8f305222a724.png)
![ArcanePBR+IBL](https://user-images.githubusercontent.com/11170417/58596408-f7007c00-8241-11e9-895c-cab452548672.png)

# Features

## Rendering
- Forward + Deferred Rendering
- Physically Based
- Cook Torrance Specular BRDF w/ Lambertian Diffuse
- Light Probes (IBL)
- Reflection Probes (IBL)
- Linear Lighting (w/ Gamma Correction)
- HDR w/ Reinhard Tonemapper
- PBR Material System
- Multisampling Anti Aliasing (MSAA)
- Directional, Point, and Spot Light Sources
- Directional Light Shadowmapping
- Skybox Support

## Engine
- OpenGL 4.3+
- GLFW Input + Window
- Model Loading via ASSIMP
- ImGUI for UI
- SOIL for Image Loading

## Development
If you want to see what I am currently working on, check out Arcane's roadmap:

https://trello.com/b/WzD2bYCD/arcane-engine

I plan on developing adding many different features to the forward & deferred renderers. Here are some of the few things I am really looking forward to adding:
1. Decals
1. Particles
1. SSR
1. Volumetric Lighting
1. Reverse Z Buffer
1. Virtual Texture System (Terrain)

## Contributing
1. Fork it (https://github.com/Ershany/Arcane-Engine.git)
1. Create your feature branch (`git checkout -b new-feature-branch`)
1. Commit your changes (`git commit -m "A description for your feature"`)
1. Push to the branch (`git push origin new-feature-branch`)
1. Create a new Pull Request
