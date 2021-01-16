#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "renderer.h"
#include "scene.h"
#include "globals.h"

/* ── Global state ────────────────────────────────────────────────────────── */

static Camera       g_camera;
static Animation    g_anim;
static RenderOptions g_opts;

static bool   g_mouseDown = false;
static double g_lastX = 0, g_lastY = 0;
static double g_prevTime = 0;

/* ── GLFW callbacks ──────────────────────────────────────────────────────── */

static void onKey(GLFWwindow* win, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_RELEASE) return;

    /* Arrow keys: orbit camera (fire on hold via GLFW_REPEAT too) */
    switch (key) {
        case GLFW_KEY_LEFT:  g_camera.yaw   += 2.0f; return;
        case GLFW_KEY_RIGHT: g_camera.yaw   -= 2.0f; return;
        case GLFW_KEY_UP:    g_camera.pitch  = std::clamp(g_camera.pitch + 2.0f, -89.0f, 89.0f); return;
        case GLFW_KEY_DOWN:  g_camera.pitch  = std::clamp(g_camera.pitch - 2.0f, -89.0f, 89.0f); return;
        default: break;
    }

    /* Toggle keys: fire only on initial press */
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_A:     g_anim.allActive   = !g_anim.allActive;   break;
        case GLFW_KEY_1:     g_anim.twistActive = !g_anim.twistActive; break;
        case GLFW_KEY_2:     g_anim.walkActive  = !g_anim.walkActive;  break;
        case GLFW_KEY_3:     g_anim.wheelActive = !g_anim.wheelActive; break;
        case GLFW_KEY_ENTER: g_opts.showGrid    = !g_opts.showGrid;    break;
        case GLFW_KEY_EQUAL: g_anim.speed      += 10.0f;               break;
        case GLFW_KEY_MINUS: g_anim.speed       = std::max(5.0f, g_anim.speed - 10.0f); break;
        case GLFW_KEY_ESCAPE:
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(win, GLFW_TRUE);
            break;
        default: break;
    }
}

static void onMouseButton(GLFWwindow* win, int button, int action, int /*mods*/) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    g_mouseDown = (action == GLFW_PRESS);
    glfwGetCursorPos(win, &g_lastX, &g_lastY);
}

static void onMouseMove(GLFWwindow* /*win*/, double x, double y) {
    if (!g_mouseDown) return;
    g_camera.onMouseMove((float)(x - g_lastX), (float)(y - g_lastY));
    g_lastX = x;
    g_lastY = y;
}

static void onScroll(GLFWwindow* /*win*/, double /*dx*/, double dy) {
    g_camera.onScroll((float)dy);
}

static void onResize(GLFWwindow* /*win*/, int w, int h) {
    glViewport(0, 0, w, h);
}

/* ── Main ────────────────────────────────────────────────────────────────── */

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* win = glfwCreateWindow(1024, 768, "Senior Playground", nullptr, nullptr);
    if (!win) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(win);
    glfwSwapInterval(1); /* vsync */

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        return 1;
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";

    /* Register callbacks */
    glfwSetKeyCallback(win,         onKey);
    glfwSetMouseButtonCallback(win, onMouseButton);
    glfwSetCursorPosCallback(win,   onMouseMove);
    glfwSetScrollCallback(win,      onScroll);
    glfwSetFramebufferSizeCallback(win, onResize);

    /* OpenGL state */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); /* sky blue */

    /* Load resources */
    Shader  shader("shaders/phong.vert", "shaders/phong.frag");
    MeshLib meshes;
    meshes.init();

    Textures textures;
    textures.load();

    /* Bind a 1×1 white fallback to unit 0 so the sampler is always valid.
       The Metal bridge on macOS validates all samplers even when useTexture=false. */
    GLuint fallbackTex;
    {
        glGenTextures(1, &fallbackTex);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fallbackTex);
        const unsigned char white[4] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    /* Set light uniforms once — they don't change */
    shader.use();
    shader.setVec3("lightPos",    Light::POSITION);
    shader.setVec3("lightColor",  Light::COLOR);
    shader.setVec3("ambientColor", Light::AMBIENT);

    /* Projection — fixed aspect for now, updated on resize */
    int fbW, fbH;
    glfwGetFramebufferSize(win, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);

    g_prevTime = glfwGetTime();

    /* ── Main loop ─────────────────────────────────────────────────────── */
    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime();
        float  dt  = (float)(now - g_prevTime);
        g_prevTime = now;

        g_anim.update(dt);

        glfwGetFramebufferSize(win, &fbW, &fbH);
        float aspect = fbH > 0 ? (float)fbW / fbH : 1.0f;

        glm::mat4 proj = glm::perspective(glm::radians(65.0f), aspect, 0.5f, 500.0f);
        glm::mat4 view = g_camera.view();

        shader.use();
        shader.setMat4("projection", proj);
        shader.setMat4("view",       view);
        shader.setVec3("viewPos",    g_camera.position());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Renderer renderer(shader, meshes);
        drawScene(renderer, textures, g_anim, g_opts);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    /* Cleanup */
    textures.free();
    meshes.free();
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
