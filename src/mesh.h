#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Mesh {
    GLuint vao = 0, vbo = 0, ebo = 0;
    GLsizei count = 0;         /* index count, or vertex count if ebo==0 */
    GLenum  primitive = GL_TRIANGLES;

    void upload(const std::vector<Vertex>& verts,
                const std::vector<unsigned int>& indices = {});
    void draw() const;
    void free();
};

/* ── Geometry generators ────────────────────────────────────────────────── */

/* Lateral surface of a cone/cylinder (no caps), open at both ends.
   Like gluCylinder: z goes from 0 → 1, xy scaled by radius.
   Use scale(r, r, h) when drawing to get the desired dimensions. */
Mesh makeCylinder(int slices = 32);

/* Filled disk in XY plane, normal = +Z.  Use scale(r, r, 1) to resize.
   Like gluDisk with innerRadius = 0. */
Mesh makeDisk(int slices = 32);

/* Unit box (–0.5 → +0.5 on each axis).  Like glutSolidCube(1.0). */
Mesh makeBox();

/* Torus in XZ plane.  tubeR = tube cross-section, ringR = ring radius.
   Like glutSolidTorus(tubeR, ringR, sides, rings). */
Mesh makeTorus(float tubeR, float ringR, int sides = 32, int rings = 32);

/* UV sphere, radius = 1.  Use scale(r,r,r) to resize. */
Mesh makeSphere(int slices = 16, int stacks = 16);

/* Axis-aligned line grid in the XZ plane (for the debug overlay). */
Mesh makeGrid(float xMin, float xMax, float zMin, float zMax, float step);

/* Unit quad in the XZ plane (y = 0), facing +Y.  Use scale to tile. */
Mesh makeQuad();

/* Preloaded library — call init() once after creating the GL context. */
struct MeshLib {
    Mesh cylinder;
    Mesh disk;
    Mesh box;
    Mesh torus;   /* wheel-specific: tubeR=0.2, ringR=1 (callers scale ringR) */
    Mesh sphere;
    Mesh grid;
    Mesh quad;

    void init();
    void free();
};
