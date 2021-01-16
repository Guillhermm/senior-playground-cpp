#include "equipment.h"
#include <cmath>

/* ── Shared primitive ────────────────────────────────────────────────────── */

void drawPost(Renderer& r) {
    const float h = 8.25f;
    r.setColor(Color::POST);
    r.push();
        r.rotate(-90, 1, 0, 0);
        r.scale(SCALE, SCALE, 1);
        r.cylinder(0.9f, h);
        r.disk(2.5f);               /* base flange disk, matches original gluDisk(0, 2.5) */
        r.push();
            r.translate(0, 0, h);
            r.disk(0.9f);           /* top cap, facing +Z */
        r.pop();
    r.pop();
}

/* ── Parallel bars ───────────────────────────────────────────────────────── */

void drawSideBar(Renderer& r) {
    r.setColor(Color::ARM);

    r.push();
        r.rotate(90, 0, 1, 0);
        r.translate(0, 1.25f, -2.0f);
        r.cylinder(0.2f, 2.0f);
        r.diskBottom(0.2f);
    r.pop();

    r.push();
        r.rotate(90, 0, 1, 0);
        r.translate(0, 8.0f, -4.0f);
        r.cylinder(0.2f, 4.0f);
        r.diskBottom(0.2f);
    r.pop();

    r.push();
        r.rotate(90, 1, 0, 0);
        r.translate(-3.8f, 0, -8.0f);
        r.rotate(15, 0, 1, 0);
        r.cylinder(0.2f, 7.0f);
    r.pop();
}

void drawParallelBars(Renderer& r) {
    r.translate(0, 0, 0.4f);

    r.push();
        r.translate(0, 1.0f, 0);
        drawSideBar(r);
    r.pop();

    r.push();
        r.rotate(180, 0, 1, 0);
        drawSideBar(r);
    r.pop();
}

void drawDoubleParallelBars(Renderer& r) {
    r.push();
    r.translate(12.0f, 0.1f, -15.0f);

        r.push();
            r.scale(1.4f, 1.25f, 1.4f);
            drawPost(r);
        r.pop();

        r.setColor(Color::SUPPORT);
        r.push();
            r.rotate(-90, 1, 0, 0);
            r.translate(0, 0, 7.5f);
            r.cylinder(0.65f, 2.5f);
        r.pop();

        r.push();
            r.rotate(-90, 1, 0, 0);
            r.translate(0, 0, 0.75f);
            r.cylinder(0.65f, 2.5f);
        r.pop();

        r.push(); drawParallelBars(r); r.pop();

        r.push();
            r.translate(0, 0.4f, -0.8f);
            drawParallelBars(r);
        r.pop();

    r.pop();
}

/* ── Wheel spinner ───────────────────────────────────────────────────────── */

void drawWheel(Renderer& r, const Animation& anim) {
    r.setColor(Color::ARM);
    r.rotate(anim.wheelAngle, 1, 0, 0);

    r.push();
        r.rotate(90, 0, 1, 0);
        r.torus(2.5f);
    r.pop();

    /* Cross-spokes */
    r.push();
        r.translate(0, 0, -2.5f);
        r.cylinder(0.1f, 5.0f);
    r.pop();

    r.push();
        r.translate(0, 2.5f, 0);
        r.rotate(90, 1, 0, 0);
        r.cylinder(0.1f, 5.0f);
    r.pop();

    /* Handle grips at diagonal positions */
    r.setColor(Color::HANDLE);

    auto grip = [&](float ty, float tz) {
        r.push();
            r.translate(-1.0f, ty, tz);
            r.rotate(90, 0, 1, 0);
            r.disk(0.2f);
            r.cylinder(0.2f, 1.0f); /* slightly tapered in original, kept uniform */
        r.pop();
    };
    const float d = (float)(sqrt(2.0) / 2.0);
    grip( 2.5f - d,  2.5f - d);
    grip(-2.5f + d, -2.5f + d);
}

void drawDoubleWheel(Renderer& r, const Animation& anim) {
    r.push();
    r.translate(13.0f, 0.1f, 0);
    r.rotate(45, 0, 1, 0);

        r.push(); drawPost(r); r.pop();

        r.setColor(Color::SUPPORT);
        r.push();
            r.rotate(-90, 1, 0, 0);
            r.translate(0, 0, 6.0f);
            r.cylinder(0.5f, 2.0f);
        r.pop();

        r.setColor(Color::ARM);
        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 6.5f, -1.5f);
            r.cylinder(0.125f, 1.0f);
        r.pop();

        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 7.5f, 0.5f);
            r.cylinder(0.125f, 1.0f);
        r.pop();

        r.push();
            r.translate(-1.5f, 6.5f, 0);
            drawWheel(r, anim);
        r.pop();

        r.push();
            r.translate(1.5f, 7.5f, 0);
            r.rotate(180, 0, 1, 0);
            drawWheel(r, anim);
        r.pop();

    r.pop();
}

/* ── Sign ────────────────────────────────────────────────────────────────── */

void drawSign(Renderer& r, GLuint texSign) {
    const float h = 8.25f;

    auto post = [&](float x) {
        r.push();
            r.translate(x, 0, -24.0f);
            r.scale(1.0f, 1.75f, 1.0f);
            r.rotate(-90, 1, 0, 0);
            r.scale(SCALE, SCALE, 1.0f);
            r.setColor(Color::SIGN_POST);
            r.cylinder(0.9f, h);
            r.push();
                r.translate(0, 0, h);
                r.disk(0.9f);
            r.pop();
        r.pop();
    };

    r.push();
        post(-8.0f);
        post( 8.0f);

        /* Board */
        r.push();
            r.setColor(Color::SIGN);
            r.rotate(90, 1, 0, 0);
            r.translate(0, -23.5f, -14.0f);

            r.push();
                r.box(20.0f, 0.1f, 12.0f);
            r.pop();

            /* Textured face — centered on board, Z flipped to match original UV orientation */
            r.push();
                r.translate(0, 0.1f, 0);
                r.scale(19.0f, 1.0f, -11.0f);
                r.drawTextured(r.meshes.quad, texSign);
            r.pop();
        r.pop();
    r.pop();
}

/* ── Twister ─────────────────────────────────────────────────────────────── */

void drawTwistFoot(Renderer& r, const Animation& anim) {
    /* Arm-support joint */
    r.setColor(Color::SUPPORT);
    r.push();
        r.translate(0, 7.25f, 0.75f);
        r.scale(SCALE, SCALE, 1);
        r.cylinder(0.4f, 0.5f);
        r.push();
            r.translate(0, 0, 0.5f);
            r.disk(0.4f);
        r.pop();
    r.pop();

    /* Animated upper arm + foot platform */
    r.push();
        r.rotate(90, 1, 0, 0);
        r.translate(0, 1.0f, -7.25f);
        r.rotate(anim.twistAngle, 0, 1, 0);
        r.scale(SCALE, SCALE, 1);
        r.cylinder(0.4f, 2.75f);
        r.translate(0, 0, 2.55f);
        r.rotate(-30, 1, 0, 0);
        r.cylinder(0.4f, 3.25f);

        /* Platform */
        r.setColor(Color::SUPPORT);
        r.push();
            r.rotate(30, 1, 0, 0);
            r.translate(0, 2.0f, 2.75f);
            r.box(1.0f, 2.0f, 0.75f);
        r.pop();

        /* Foot pads */
        r.push();
            r.setColor(Color::POST);
            r.push();
                r.rotate(30, 1, 0, 0);
                r.translate(0, 3.5f, 3.0f);
                r.box(2.75f, 4.0f, 0.1f);

                r.setColor(Color::SUPPORT);
                /* Three vertical side rails — positions derived from original's nested scale */
                r.push(); r.translate(    0, 0, -0.1f); r.box(0.275f, 4.0f, 0.2f); r.pop();
                r.push(); r.translate( 1.375f, 0, -0.1f); r.box(0.275f, 4.0f, 0.2f); r.pop();
                r.push(); r.translate(-1.375f, 0, -0.1f); r.box(0.275f, 4.0f, 0.2f); r.pop();
                /* Top and bottom horizontal rails */
                r.push(); r.translate(0,  2.0f, -0.1f); r.box(2.75f, 0.2f, 0.2f); r.pop();
                r.push(); r.translate(0, -2.0f, -0.1f); r.box(2.75f, 0.2f, 0.2f); r.pop();
            r.pop();
        r.pop();
    r.pop();
}

void drawDoubleTwister(Renderer& r, const Animation& anim) {
    r.push();
    r.translate(-13.0f, 0.1f, 0);
    r.rotate(90, 0, 1, 0);

        r.push(); drawPost(r); r.pop();

        r.rotate(90, 0, 1, 0);

        r.setColor(Color::POST);
        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 7.75f, -2.0f);
            r.scale(SCALE, SCALE, 1);
            r.cylinder(0.25f, 4.0f);
        r.pop();

        r.push();
            r.translate(0, 7.25f, -0.75f);
            r.scale(SCALE, SCALE, 1);
            r.cylinder(0.25f, 1.5f);
        r.pop();

        r.push();
            r.translate(-2.0f, 7.75f, -1.0f);
            r.scale(SCALE, SCALE, 1);
            r.cylinder(0.25f, 2.0f);
        r.pop();

        r.push();
            r.translate(2.0f, 7.75f, -1.0f);
            r.scale(SCALE, SCALE, 1);
            r.cylinder(0.25f, 2.0f);
        r.pop();

        r.setColor(Color::HANDLE);
        for (int flip = 0; flip < 2; ++flip) {
            for (float sx : {-2.0f, 2.0f}) {
                r.push();
                    if (flip) r.rotate(180, 0, 1, 0);
                    r.translate(sx, 7.75f, -2.25f);
                    r.disk(0.175f);
                    r.cylinder(0.175f, 2.0f);
                r.pop();
            }
        }

        r.push(); drawTwistFoot(r, anim); r.pop();
        r.rotate(180, 0, 1, 0);
        r.push(); drawTwistFoot(r, anim); r.pop();

    r.pop();
}

/* ── Walking simulator ───────────────────────────────────────────────────── */

void drawWalkerFoot(Renderer& r, const Animation& anim) {
    r.setColor(Color::SUPPORT);
    r.push();
        r.translate(0, 7.25f, 0.75f);
        r.scale(SCALE, SCALE, 1);
        r.cylinder(0.4f, 0.5f);
        r.push();
            r.translate(0, 0, 0.5f);
            r.disk(0.4f);
        r.pop();
    r.pop();

    r.push();
        r.rotate(90, 1, 0, 0);
        r.translate(0, 1.0f, -7.25f);
        r.rotate(anim.walkAngle, 0, 1, 0);
        r.scale(SCALE, SCALE, 1);
        r.cylinder(0.4f, 2.75f);
        r.translate(0, 0, 2.55f);
        r.rotate(-30, 1, 0, 0);
        r.cylinder(0.4f, 3.25f);

        r.setColor(Color::SUPPORT);
        r.push();
            r.rotate(30, 1, 0, 0);
            r.translate(0, 2.0f, 2.75f);
            r.box(2.0f, 0.75f, 0.75f);
        r.pop();

        r.push();
            r.setColor(Color::POST);
            r.push();
                r.rotate(30, 1, 0, 0);
                r.translate(0, 3.0f, 3.0f);
                r.box(4.0f, 2.0f, 0.1f);

                r.setColor(Color::SUPPORT);
                /* Two vertical side rails — positions derived from original's nested scale */
                r.push(); r.translate( 2.0f, 0, -0.1f); r.box(0.4f, 2.0f, 0.2f); r.pop();
                r.push(); r.translate(-2.0f, 0, -0.1f); r.box(0.4f, 2.0f, 0.2f); r.pop();
                /* Top and bottom horizontal rails */
                r.push(); r.translate(0,  1.0f, -0.1f); r.box(4.0f, 0.2f, 0.2f); r.pop();
                r.push(); r.translate(0, -1.0f, -0.1f); r.box(4.0f, 0.2f, 0.2f); r.pop();
            r.pop();
        r.pop();
    r.pop();
}

void drawWalkingSimulator(Renderer& r, const Animation& anim) {
    r.push();
    r.translate(0, 0.1f, 0);

        r.push();
            r.scale(1.0f, 0.2f, 1.0f);
            drawPost(r);
        r.pop();

        r.setColor(Color::POST);
        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 1.25f, -3.0f);
            r.cylinder(0.2f, 6.0f);
        r.pop();

        r.push();
            r.rotate(-90, 1, 0, 0);
            r.translate(-3.0f, 0, 1.25f);
            r.cylinder(0.2f, 7.0f);
        r.pop();

        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 7.75f, -3.0f);
            r.cylinder(0.175f, 1.0f);
        r.pop();

        r.push();
            r.rotate(-90, 1, 0, 0);
            r.translate(3.0f, 0, 1.25f);
            r.cylinder(0.2f, 7.0f);
        r.pop();

        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 7.75f, 2.0f);
            r.cylinder(0.175f, 1.0f);
        r.pop();

        /* Hand rails */
        r.setColor(Color::ARM);
        r.push();
            r.rotate(90, 1, 0, 0);
            r.translate(3.0f, 0, -8.5f);
            r.scale(SCALE, SCALE, 1);
            r.cylinder(0.5f, 0.5f);
            r.push();
                r.translate(-12.0f, 0, 0);
                r.cylinder(0.5f, 0.5f);
            r.pop();
        r.pop();

        r.push();
            r.rotate(15, 1, 0, 0);
            r.translate(3.0f, 8.0f, -4.25f);
            r.cylinder(0.25f, 2.0f);
            r.push();
                r.translate(-6.0f, 0, 0);
                r.cylinder(0.25f, 2.0f);
            r.pop();
        r.pop();

        r.setColor(Color::HANDLE);
        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(1.75f, 8.75f, -3.0f);
            r.cylinder(0.2f, 6.0f);
        r.pop();

        /* Two mirrored foot pedal assemblies */
        r.push();
            r.rotate(90, 0, 1, 0);
            r.translate(0, 0.5f, -3.25f);
            r.push(); drawWalkerFoot(r, anim); r.pop();
            r.rotate(180, 0, 1, 0);
            r.translate(0, 0, -6.5f);
            r.push(); drawWalkerFoot(r, anim); r.pop();
        r.pop();

    r.pop();
}
