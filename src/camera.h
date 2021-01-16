#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    float yaw    =   0.0f;   /* horizontal angle around scene, degrees */
    float pitch  =  20.0f;   /* vertical angle, degrees */
    float radius =  30.0f;   /* distance from target */
    glm::vec3 target = {0.0f, 0.0f, 0.0f};

    glm::mat4 view()     const;
    glm::vec3 position() const;

    /* Called by GLFW callbacks */
    void onMouseMove(float dx, float dy);
    void onScroll(float delta);
};
