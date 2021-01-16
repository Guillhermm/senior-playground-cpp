#pragma once
#include <GL/glew.h>
#include "renderer.h"
#include "globals.h"

void drawPost               (Renderer& r);
void drawSideBar            (Renderer& r);
void drawParallelBars       (Renderer& r);
void drawDoubleParallelBars (Renderer& r);
void drawWheel              (Renderer& r, const Animation& anim);
void drawDoubleWheel        (Renderer& r, const Animation& anim);
void drawSign               (Renderer& r, GLuint texSign);
void drawTwistFoot          (Renderer& r, const Animation& anim);
void drawDoubleTwister      (Renderer& r, const Animation& anim);
void drawWalkerFoot         (Renderer& r, const Animation& anim);
void drawWalkingSimulator   (Renderer& r, const Animation& anim);
