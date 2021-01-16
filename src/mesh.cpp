#include "mesh.h"
#include <cmath>
#include <stdexcept>

static constexpr float TAU = 6.28318530f;

/* ── Mesh upload / draw / free ──────────────────────────────────────────── */

void Mesh::upload(const std::vector<Vertex>& verts,
                  const std::vector<unsigned int>& indices) {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr)(verts.size() * sizeof(Vertex)),
                 verts.data(), GL_STATIC_DRAW);

    if (!indices.empty()) {
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (GLsizeiptr)(indices.size() * sizeof(unsigned int)),
                     indices.data(), GL_STATIC_DRAW);
        count = (GLsizei)indices.size();
    } else {
        count = (GLsizei)verts.size();
    }

    /* layout(location=0) = pos */
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, pos));
    /* layout(location=1) = normal */
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));
    /* layout(location=2) = uv */
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, uv));

    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(vao);
    if (ebo)
        glDrawElements(primitive, count, GL_UNSIGNED_INT, nullptr);
    else
        glDrawArrays(primitive, 0, count);
    glBindVertexArray(0);
}

void Mesh::free() {
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    vao = vbo = ebo = 0;
}

/* ── Cylinder ────────────────────────────────────────────────────────────── */

Mesh makeCylinder(int slices) {
    /* Unit cylinder: r=1, z from 0→1, no caps.
       Normals point radially outward (perpendicular to side wall). */
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    for (int i = 0; i <= slices; ++i) {
        float t  = (float)i / slices;
        float a  = t * TAU;
        float ca = cosf(a), sa = sinf(a);

        glm::vec3 n = {ca, sa, 0.0f};

        /* bottom ring */
        verts.push_back({{ca, sa, 0.0f}, n, {t, 0.0f}});
        /* top ring */
        verts.push_back({{ca, sa, 1.0f}, n, {t, 1.0f}});
    }

    for (int i = 0; i < slices; ++i) {
        unsigned int b = (unsigned int)i * 2;
        /* two triangles per quad */
        idx.insert(idx.end(), {b, b+1, b+2, b+1, b+3, b+2});
    }

    Mesh m;
    m.upload(verts, idx);
    return m;
}

/* ── Disk ────────────────────────────────────────────────────────────────── */

Mesh makeDisk(int slices) {
    /* Unit filled disk in XY plane, normal = +Z, outer radius = 1. */
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    /* Centre vertex */
    verts.push_back({{0, 0, 0}, {0, 0, 1}, {0.5f, 0.5f}});

    for (int i = 0; i <= slices; ++i) {
        float a  = (float)i / slices * TAU;
        float ca = cosf(a), sa = sinf(a);
        verts.push_back({{ca, sa, 0.0f}, {0, 0, 1},
                         {0.5f + 0.5f * ca, 0.5f + 0.5f * sa}});
    }

    for (int i = 1; i <= slices; ++i)
        idx.insert(idx.end(), {0u, (unsigned)i, (unsigned)(i + 1)});

    Mesh m;
    m.upload(verts, idx);
    return m;
}

/* ── Box ─────────────────────────────────────────────────────────────────── */

Mesh makeBox() {
    /* Unit box centred at origin, one quad per face with correct normals. */
    static const glm::vec3 faceNormals[6] = {
        { 0, 0, 1}, { 0, 0,-1},
        { 1, 0, 0}, {-1, 0, 0},
        { 0, 1, 0}, { 0,-1, 0}
    };
    /* Per-face: 4 corners in CCW order */
    static const glm::vec3 faceVerts[6][4] = {
        /* +Z */ {{-.5,-.5,.5},{.5,-.5,.5},{.5,.5,.5},{-.5,.5,.5}},
        /* -Z */ {{.5,-.5,-.5},{-.5,-.5,-.5},{-.5,.5,-.5},{.5,.5,-.5}},
        /* +X */ {{.5,-.5,.5},{.5,-.5,-.5},{.5,.5,-.5},{.5,.5,.5}},
        /* -X */ {{-.5,-.5,-.5},{-.5,-.5,.5},{-.5,.5,.5},{-.5,.5,-.5}},
        /* +Y */ {{-.5,.5,.5},{.5,.5,.5},{.5,.5,-.5},{-.5,.5,-.5}},
        /* -Y */ {{-.5,-.5,-.5},{.5,-.5,-.5},{.5,-.5,.5},{-.5,-.5,.5}},
    };
    static const glm::vec2 uvs[4] = {{0,0},{1,0},{1,1},{0,1}};

    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    for (int f = 0; f < 6; ++f) {
        unsigned int base = (unsigned int)verts.size();
        for (int v = 0; v < 4; ++v)
            verts.push_back({faceVerts[f][v], faceNormals[f], uvs[v]});
        idx.insert(idx.end(), {base,base+1,base+2, base,base+2,base+3});
    }

    Mesh m;
    m.upload(verts, idx);
    return m;
}

/* ── Torus ───────────────────────────────────────────────────────────────── */

Mesh makeTorus(float tubeR, float ringR, int sides, int rings) {
    /* Torus in the XY plane (hole faces +Z), matching glutSolidTorus convention.
       drawWheel applies glRotatef(90, 0,1,0) to turn the hole to face +X. */
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    for (int i = 0; i <= rings; ++i) {
        float u  = (float)i / rings * TAU;
        float cu = cosf(u), su = sinf(u);
        /* ring centre in XY plane */
        glm::vec3 centre = {ringR * cu, ringR * su, 0.0f};

        for (int j = 0; j <= sides; ++j) {
            float v  = (float)j / sides * TAU;
            float cv = cosf(v), sv = sinf(v);

            /* tube cross-section normal and position */
            glm::vec3 n   = {cv * cu, cv * su, sv};
            glm::vec3 pos = {(ringR + tubeR * cv) * cu,
                             (ringR + tubeR * cv) * su,
                             tubeR * sv};

            verts.push_back({pos, n, {(float)i / rings, (float)j / sides}});
        }
    }

    int sideCount = sides + 1;
    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < sides; ++j) {
            unsigned int a = (unsigned)(i * sideCount + j);
            unsigned int b = a + 1;
            unsigned int c = a + (unsigned)sideCount;
            unsigned int d = c + 1;
            idx.insert(idx.end(), {a, c, b, b, c, d});
        }
    }

    Mesh m;
    m.upload(verts, idx);
    return m;
}

/* ── Sphere ──────────────────────────────────────────────────────────────── */

Mesh makeSphere(int slices, int stacks) {
    std::vector<Vertex>       verts;
    std::vector<unsigned int> idx;

    for (int i = 0; i <= stacks; ++i) {
        float phi = (float)i / stacks * (float)M_PI;
        float cp  = cosf(phi), sp = sinf(phi);

        for (int j = 0; j <= slices; ++j) {
            float theta = (float)j / slices * TAU;
            float ct = cosf(theta), st = sinf(theta);

            glm::vec3 n = {sp * ct, cp, sp * st};
            verts.push_back({n, n, {(float)j / slices, (float)i / stacks}});
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            unsigned int a = (unsigned)(i * (slices + 1) + j);
            unsigned int b = a + 1;
            unsigned int c = a + (unsigned)(slices + 1);
            unsigned int d = c + 1;
            idx.insert(idx.end(), {a, c, b, b, c, d});
        }
    }

    Mesh m;
    m.upload(verts, idx);
    return m;
}

/* ── Grid (GL_LINES) ─────────────────────────────────────────────────────── */

Mesh makeGrid(float xMin, float xMax, float zMin, float zMax, float step) {
    std::vector<Vertex> verts;

    glm::vec3 n = {0, 1, 0};
    for (float x = xMin; x <= xMax + 0.001f; x += step) {
        verts.push_back({{x, 0, zMin}, n, {}});
        verts.push_back({{x, 0, zMax}, n, {}});
    }
    for (float z = zMin; z <= zMax + 0.001f; z += step) {
        verts.push_back({{xMin, 0, z}, n, {}});
        verts.push_back({{xMax, 0, z}, n, {}});
    }

    Mesh m;
    m.primitive = GL_LINES;
    m.upload(verts);
    return m;
}

/* ── Quad ────────────────────────────────────────────────────────────────── */

Mesh makeQuad() {
    /* Unit quad in XZ plane (y=0), facing +Y, corners at ±0.5. */
    std::vector<Vertex> verts = {
        {{-0.5f, 0, -0.5f}, {0,1,0}, {0, 0}},
        {{ 0.5f, 0, -0.5f}, {0,1,0}, {1, 0}},
        {{ 0.5f, 0,  0.5f}, {0,1,0}, {1, 1}},
        {{-0.5f, 0,  0.5f}, {0,1,0}, {0, 1}},
    };
    std::vector<unsigned int> idx = {0, 2, 1, 0, 3, 2};
    Mesh m;
    m.upload(verts, idx);
    return m;
}

/* ── MeshLib ─────────────────────────────────────────────────────────────── */

void MeshLib::init() {
    cylinder = makeCylinder(32);
    disk     = makeDisk(32);
    box      = makeBox();
    /* Wheel torus: tube radius 0.2 relative to ring radius 1.
       drawWheel scales ringR to 2.5 via scale(). */
    torus    = makeTorus(0.08f, 1.0f, 32, 32);
    sphere   = makeSphere(16, 16);
    grid     = makeGrid(-20, 20, -29, 29, 1.0f);
    quad     = makeQuad();
}

void MeshLib::free() {
    cylinder.free(); disk.free(); box.free();
    torus.free(); sphere.free(); grid.free(); quad.free();
}
