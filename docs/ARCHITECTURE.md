# Architecture — Senior Playground (C++ / Modern OpenGL port)

This document describes the architecture of the C++ modernization. For the original C/GLUT implementation see the [senior-playground](https://github.com/Guillhermm/senior-playground) repository.

---

## Overview

The application renders an interactive 3D scene using **OpenGL 3.3 Core Profile**. The fixed-function pipeline, immediate-mode geometry, and GLU quadrics from the original have been replaced with:

- A programmable GLSL pipeline (Blinn-Phong vertex + fragment shaders)
- VAO/VBO-based procedural mesh generation
- **GLFW** for window management and input
- **GLEW** for extension loading
- **GLM** for matrix/vector math
- A thin `Renderer` class that reimplements the push/pop matrix stack on top of the shader

---

## Module breakdown

### `src/main.cpp`
Entry point. Creates the GLFW window, initialises GLEW, builds the `Shader`, populates `MeshLib`, loads textures, and runs the main loop. The loop computes a wall-clock `dt` and passes it to `Animation::update(dt)` so animation speed is frame-rate independent. Input callbacks are registered here and forward to the `Camera` and `Animation` structs.

### `src/scene.cpp` / `scene.h`
Top-level scene function `drawScene(Renderer&, Textures&, Animation&, RenderOptions&)`:

- Draws the terrain: sand quad → grass quad → 3×3 grid of textured concrete slabs.
- Calls all five equipment draw functions in sequence.
- Conditionally calls `drawGrid()` based on `RenderOptions::showGrid`.

Also owns the `Textures` struct that holds the four `GLuint` texture handles loaded at startup.

### `src/equipment.cpp` / `equipment.h`
One function per logical fitness station, using the `Renderer` API instead of raw OpenGL calls. The coordinate system and scale factors mirror the original as closely as possible.

| Function | Station |
|---|---|
| `drawDoubleParallelBars(Renderer&, Animation&)` | Double parallel bars |
| `drawDoubleWheel(Renderer&, Animation&)` | Wheel spinner |
| `drawSign(Renderer&, GLuint tex)` | Information sign (texture-mapped) |
| `drawDoubleTwister(Renderer&, Animation&)` | Lateral twister |
| `drawWalkingSimulator(Renderer&, Animation&)` | Walking simulator |

### `src/camera.cpp` / `camera.h`
Orbit camera parameterised by azimuth, elevation, and radius. Produces a GLM `view` matrix each frame via `glm::lookAt`. Mouse drag updates azimuth/elevation; scroll wheel updates radius. Replaces the original's arrow-key scene rotation.

### `src/mesh.cpp` / `mesh.h`
Procedural geometry generation. Each primitive is generated once at startup, uploaded to a VAO/VBO, and stored in `MeshLib`. Primitives:

| Mesh | Description |
|---|---|
| `cylinder` | Stacked rings with computed normals; closed caps via separate disk draw |
| `disk` | Single flat ring (used for end caps) |
| `box` | Unit cube with per-face normals |
| `torus` | Parametric torus (major and minor radii) |
| `sphere` | UV sphere |
| `quad` | Unit quad in the XZ plane |
| `grid` | Flat line grid for the debug overlay |

Replaces the original's `gluCylinder`, `gluDisk`, `gluSphere`, `glutSolidCube`, and `glutSolidTorus` calls.

### `src/renderer.h`
A non-owning wrapper that holds a reference to the `Shader` and maintains a `std::stack<glm::mat4>` as the model matrix stack. Public API mirrors the original OpenGL matrix calls:

| Method | Equivalent original call |
|---|---|
| `push()` / `pop()` | `glPushMatrix()` / `glPopMatrix()` |
| `translate(x,y,z)` | `glTranslatef` |
| `rotate(deg,x,y,z)` | `glRotatef` |
| `scale(x,y,z)` | `glScalef` |
| `setColor(vec3)` | `glColor3f` + material uniforms |
| `draw(mesh)` | `glDrawElements` via VAO bind |
| `drawTextured(mesh, tex)` | texture bind + `glDrawElements` |

The renderer also uploads the normal matrix (`transpose(inverse(model))`) so lighting is computed correctly in world space.

### `src/shader.cpp` / `shader.h`
Loads, compiles, and links the GLSL shaders from `shaders/phong.vert` and `shaders/phong.frag`. Provides typed uniform setters (`setBool`, `setInt`, `setFloat`, `setVec3`, `setMat3`, `setMat4`).

### `src/texture.cpp` / `texture.h`
`loadTexture(path)` wraps the C image loader: calls `ImageLoad`, creates a `GL_TEXTURE_2D` with `GL_RGBA8` internal format, generates mipmaps, and configures repeat + linear-mipmap filtering.

> **Row alignment note:** The SGI image loader pads each scanline to a 4-byte boundary. `GL_UNPACK_ALIGNMENT` must be left at its default value of 4 (not overridden to 1) so OpenGL's pixel unpack stride matches the loader's padded layout. Mismatched alignment causes a progressive 1-byte-per-row shift that scrambles colour channels.

### `src/globals.h`
Header-only constants and structs shared across all modules:

- **`Color` namespace** — `inline const glm::vec3` material colours for each part type.
- **`Light` namespace** — world-space light position, colour, and ambient term.
- **`Animation` struct** — per-station angle accumulators, direction flags, and an `update(float dt)` method that advances angles each frame.
- **`RenderOptions` struct** — toggleable rendering options (currently: `showGrid`).

### `src/image.c` / `image.h`
Unchanged from the original: a standalone SGI RGB (`.rgb`) file loader supporting both verbatim and RLE-compressed variants. Kept as C to avoid any C++ linkage issues.

### `shaders/phong.vert`
Transforms each vertex from model space to clip space using the standard MVP chain. Passes `fragPos` (world space), `fragNormal` (world space, via normal matrix), and `fragTexCoord` to the fragment shader.

### `shaders/phong.frag`
Implements Blinn-Phong shading:

1. **Ambient** — `ambientColor × diffuseColor`
2. **Diffuse** — `max(dot(N, L), 0) × lightColor × diffuseColor`
3. **Specular** — `pow(max(dot(N, H), 0), shininess) × lightColor × 0.4`

When `useTexture` is true, the texture sample replaces `diffuseColor` in the ambient and diffuse terms; the specular highlight is still computed from `lightColor` only.

---

## Data flow

```
main()
  ├─ init: Shader, MeshLib, Textures, Camera, Animation
  └─ render loop (GLFW)
       ├─ dt = current_time - last_time
       ├─ animation.update(dt)
       ├─ camera.view()         → view matrix
       ├─ Renderer::setVP(view, proj)
       ├─ drawScene(renderer, textures, animation, options)
       │    ├─ terrain quads + concrete slabs
       │    └─ draw*()  ← equipment.cpp, reads animation angles
       ├─ glfwSwapBuffers()
       └─ glfwPollEvents()
            ├─ mouse drag   → camera azimuth / elevation
            ├─ scroll       → camera radius
            └─ key press    → animation flags / speed / options
```

---

## Rendering pipeline

1. All geometry is stored in VAOs generated at startup — no per-frame uploads.
2. A single shader program is active for the entire frame.
3. The `Renderer` uploads model, view, projection, and normal matrices as uniforms before each draw call.
4. Lighting uniforms (light position, colour, ambient) are set once per frame from `Light::*` constants.
5. Material uniforms (`diffuseColor`, `shininess`, `useTexture`) are set per draw call via `setColor` / `drawTextured`.

---

## Differences from the original C/GLUT version

| Aspect | Original (C/GLUT) | This port (C++/GLFW) |
|---|---|---|
| OpenGL API | 1.x fixed-function | 3.3 Core Profile |
| Shading | Gouraud (driver-computed) | Blinn-Phong GLSL |
| Geometry | Immediate mode + GLU quadrics | VAO/VBO + procedural generation |
| Window / event loop | GLUT | GLFW |
| Camera | Arrow-key scene rotation | Orbit camera (mouse drag + scroll) |
| Animation timing | Key-hold only, no dt | Frame-rate independent (wall-clock dt) |
| Shared state | Global variables in `globals.c` | Typed structs passed by reference |
| Texture loading | `gluBuild2DMipmaps` | `glTexImage2D` + `glGenerateMipmap` |

---

## Asset pipeline

Textures (`.rgb`) and shaders (`.vert`, `.frag`) live in `assets/` and `shaders/` in the source tree. A CMake post-build step copies them into the build directory alongside the binary so the application can load them with relative paths at runtime. This means **rebuilding after editing only a shader file requires a forced rebuild** (`cmake --build build -- -B`) to trigger the copy step.
