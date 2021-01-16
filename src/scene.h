#pragma once
#include <GL/glew.h>
#include "renderer.h"
#include "globals.h"

struct Textures {
    GLuint sand  = 0;
    GLuint grass = 0;
    GLuint floor = 0;
    GLuint sign  = 0;

    void load();
    void free();
};

void drawScene(Renderer& r,
               const Textures& tex,
               const Animation& anim,
               const RenderOptions& opts);
