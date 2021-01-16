#pragma once
#include <stack>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "mesh.h"

/*
 * Renderer wraps the shader + a local matrix stack, providing an API that
 * closely mirrors the old glPushMatrix / glTranslatef / glMaterialfv style
 * so equipment draw functions are easy to translate.
 */
class Renderer {
public:
    Shader&  shader;
    MeshLib& meshes;

    glm::vec3 currentColor = {1, 1, 1};
    float     shininess    = 32.0f;

    explicit Renderer(Shader& s, MeshLib& m) : shader(s), meshes(m) {
        stack_.push(glm::mat4(1.0f));
    }

    /* Matrix stack */
    void push()  { stack_.push(stack_.top()); }
    void pop()   { stack_.pop(); }

    void translate(float x, float y, float z) {
        top() = glm::translate(top(), {x, y, z});
    }
    void rotate(float deg, float x, float y, float z) {
        top() = glm::rotate(top(), glm::radians(deg), {x, y, z});
    }
    void scale(float x, float y, float z) {
        top() = glm::scale(top(), {x, y, z});
    }

    /* Material */
    void setColor(glm::vec3 c, float s = 32.0f) {
        currentColor = c;
        shininess    = s;
    }

    /* Draw current mesh with current model matrix and material. */
    void draw(const Mesh& mesh) {
        glm::mat4 model = top();
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));

        shader.setMat4("model",        model);
        shader.setMat3("normalMatrix", normalMat);
        shader.setVec3("diffuseColor", currentColor);
        shader.setFloat("shininess",   shininess);
        shader.setBool("useTexture",   false);
        mesh.draw();
    }

    /* Draw with a bound texture. */
    void drawTextured(const Mesh& mesh, GLuint texID, float tileX = 1, float tileY = 1) {
        glm::mat4 model = top();
        glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));

        shader.setMat4("model",        model);
        shader.setMat3("normalMatrix", normalMat);
        shader.setVec3("diffuseColor", currentColor);
        shader.setFloat("shininess",   shininess);
        shader.setBool("useTexture",   true);
        shader.setInt("diffuseTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        mesh.draw();
        glBindTexture(GL_TEXTURE_2D, 0);

        shader.setBool("useTexture", false);
    }

    /* Convenience: draw a cylinder (r=rxy, h=h) matching gluCylinder(r, r, h). */
    void cylinder(float r, float h) {
        push(); scale(r, r, h); draw(meshes.cylinder); pop();
    }

    /* Convenience: draw a filled disk facing +Z (match gluDisk(0, r)). */
    void disk(float r) {
        push(); scale(r, r, 1); draw(meshes.disk); pop();
    }

    /* Convenience: draw a filled disk facing -Z (bottom cap). */
    void diskBottom(float r) {
        push();
        rotate(180, 0, 1, 0);
        scale(r, r, 1);
        draw(meshes.disk);
        pop();
    }

    /* Convenience: draw a box scaled to (w, h, d). */
    void box(float w, float h, float d) {
        push(); scale(w, h, d); draw(meshes.box); pop();
    }

    /* Convenience: draw the torus scaled so ring radius = ringR.
       The stored torus has ringR=1, so we uniformly scale xy.
       tubeR stays proportional (stored as 0.08 * ringR). */
    void torus(float ringR) {
        push(); scale(ringR, ringR, ringR); draw(meshes.torus); pop();
    }

    /* Convenience: draw a sphere of radius r. */
    void sphere(float r) {
        push(); scale(r, r, r); draw(meshes.sphere); pop();
    }

private:
    std::stack<glm::mat4> stack_;
    glm::mat4& top() { return stack_.top(); }
};
