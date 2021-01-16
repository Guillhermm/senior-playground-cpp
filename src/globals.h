#pragma once
#include <glm/glm.hpp>

/* ── Material colors ──────────────────────────────────────────────────────── */
namespace Color {
    inline const glm::vec3 POST      = {0.0f, 0.0f, 0.7f};
    inline const glm::vec3 SUPPORT   = {0.9f, 0.8f, 0.0f};
    inline const glm::vec3 ARM       = {0.9f, 0.8f, 0.0f};
    inline const glm::vec3 HANDLE    = {0.05f, 0.05f, 0.05f};
    inline const glm::vec3 GROUND    = {0.2f, 0.1f, 0.1f};
    inline const glm::vec3 SIGN_POST = {0.6f, 0.5f, 0.0f};
    inline const glm::vec3 SIGN      = {0.0f, 0.0f, 0.7f};
    inline const glm::vec3 GRASS     = {0.15f, 0.45f, 0.1f};
    inline const glm::vec3 SAND      = {0.85f, 0.78f, 0.55f};
    inline const glm::vec3 LIGHT     = {1.0f, 1.0f, 1.0f};
}

/* ── Lighting ─────────────────────────────────────────────────────────────── */
namespace Light {
    inline const glm::vec3 POSITION = {0.0f, 30.0f, 0.0f};
    inline const glm::vec3 COLOR    = {1.0f,  1.0f, 1.0f};
    inline const glm::vec3 AMBIENT  = {0.85f, 0.75f, 0.80f};
}

/* ── Shared scale factor (matches original 0.5 used for some elements) ────── */
inline constexpr float SCALE = 0.5f;

/* ── Animation state ─────────────────────────────────────────────────────── */
struct Animation {
    bool allActive   = false;
    bool twistActive = false;
    bool walkActive  = false;
    bool wheelActive = false;

    float wheelAngle = 0.0f;
    float twistAngle = 0.0f;
    float walkAngle  = 0.0f;

    float twistDir = 1.0f;
    float walkDir  = 1.0f;

    float speed = 40.0f;  /* degrees per second */

    void update(float dt) {
        if (allActive || wheelActive) {
            wheelAngle += speed * dt;
            if (wheelAngle > 360.0f) wheelAngle -= 360.0f;
        }
        if (allActive || twistActive) {
            twistAngle += twistDir * speed * dt;
            if (twistAngle >  55.0f) { twistAngle =  55.0f; twistDir = -1.0f; }
            if (twistAngle < -55.0f) { twistAngle = -55.0f; twistDir =  1.0f; }
        }
        if (allActive || walkActive) {
            walkAngle += walkDir * speed * dt;
            if (walkAngle >  40.0f) { walkAngle =  40.0f; walkDir = -1.0f; }
            if (walkAngle < -40.0f) { walkAngle = -40.0f; walkDir =  1.0f; }
        }
    }
};

/* ── Render options ──────────────────────────────────────────────────────── */
struct RenderOptions {
    bool showGrid = false;
};
