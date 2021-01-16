#include "scene.h"
#include "equipment.h"
#include "texture.h"
#include <iostream>

/* ── Texture loading ─────────────────────────────────────────────────────── */

void Textures::load() {
    sand  = loadTexture("sand.rgb");
    grass = loadTexture("grass.rgb");
    floor = loadTexture("concrete.rgb");
    sign  = loadTexture("sign.rgb");
}

void Textures::free() {
    GLuint ids[] = {sand, grass, floor, sign};
    glDeleteTextures(4, ids);
}

/* ── Terrain ─────────────────────────────────────────────────────────────── */

static void drawTerrain(Renderer& r, const Textures& tex) {
    /* Sand background — huge textured quad */
    r.push();
        r.setColor(Color::SAND);
        r.translate(0, -0.17f, 0);
        r.push();
            r.scale(4100.0f, 1.0f, 4100.0f);
            r.drawTextured(r.meshes.quad, tex.sand, 4096, 4096);
        r.pop();
    r.pop();

    /* Grass overlay */
    r.push();
        r.setColor(Color::GRASS);
        r.translate(-20.0f + 20.0f, 0.0f, -29.0f + 29.0f); /* centred */
        r.scale(40.0f, 1.0f, 58.0f);
        r.drawTextured(r.meshes.quad, tex.grass, 2, 4);
    r.pop();

    /* 3×3 grid of concrete floor slabs */
    float varZ = -19.0f;
    for (int z = 0; z < 3; ++z) {
        float varX = -13.0f;
        for (int x = 0; x < 3; ++x) {
            r.push();
                r.setColor(Color::GROUND);
                r.translate(varX, -0.025f, varZ);
                r.scale(12.0f, 0.1f, 18.0f);
                r.drawTextured(r.meshes.box, tex.floor);
            r.pop();
            varX += 13.0f;
        }
        varZ += 19.0f;
    }
}

/* ── Grid overlay ────────────────────────────────────────────────────────── */

static void drawGrid(Renderer& r) {
    r.setColor({0.5f, 0.5f, 0.5f});
    r.push();
        r.translate(0, 0.025f, 0);
        r.draw(r.meshes.grid);
    r.pop();
}

/* ── Full scene ──────────────────────────────────────────────────────────── */

void drawScene(Renderer& r,
               const Textures& tex,
               const Animation& anim,
               const RenderOptions& opts) {

    /* Light source indicator */
    r.setColor(Color::LIGHT);
    r.push();
        r.translate(Light::POSITION.x, Light::POSITION.y, Light::POSITION.z);
        r.sphere(0.3f);
    r.pop();

    drawTerrain(r, tex);
    if (opts.showGrid) drawGrid(r);

    /* Equipment stations */
    drawDoubleParallelBars(r);
    drawSign(r, tex.sign);
    drawDoubleWheel(r, anim);
    drawWalkingSimulator(r, anim);
    drawDoubleTwister(r, anim);

    /* Mirrored parallel bars at the far end */
    r.push();
        r.rotate(180, 0, 1, 0);
        r.translate(0, 0, 30.0f);
        drawDoubleParallelBars(r);
    r.pop();
}
