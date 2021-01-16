#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

static constexpr float DEG = 0.0174532925f; /* π/180 */

glm::vec3 Camera::position() const {
    float p = pitch * DEG;
    float y = yaw   * DEG;
    return target + glm::vec3(
        radius * cosf(p) * sinf(y),
        radius * sinf(p),
        radius * cosf(p) * cosf(y)
    );
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position(), target, glm::vec3(0, 1, 0));
}

void Camera::onMouseMove(float dx, float dy) {
    yaw   -= dx * 0.4f;
    pitch  = std::clamp(pitch + dy * 0.4f, -89.0f, 89.0f);
}

void Camera::onScroll(float delta) {
    radius = std::clamp(radius - delta * 2.0f, 5.0f, 120.0f);
}
