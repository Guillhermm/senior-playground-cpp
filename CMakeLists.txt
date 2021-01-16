cmake_minimum_required(VERSION 3.17)
project(senior-playground CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD   11)

if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

# ── Dependencies ──────────────────────────────────────────────────────────────

find_package(OpenGL REQUIRED)
find_package(GLEW   REQUIRED)
find_package(glfw3  REQUIRED)

include(FetchContent)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)

# ── Sources ───────────────────────────────────────────────────────────────────

set(SOURCES
    src/main.cpp
    src/shader.cpp
    src/mesh.cpp
    src/camera.cpp
    src/texture.cpp
    src/scene.cpp
    src/equipment.cpp
    src/image.c        # C source compiled as C
)

add_executable(senior-playground ${SOURCES})

target_include_directories(senior-playground PRIVATE src)

target_link_libraries(senior-playground PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm::glm
)

# ── Copy assets to build dir ─────────────────────────────────────────────────

add_custom_command(TARGET senior-playground POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:senior-playground>
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:senior-playground>/shaders
    COMMENT "Copying assets and shaders to build directory"
)
