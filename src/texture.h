#pragma once
#include <string>
#include <GL/glew.h>

/* Load an SGI .rgb file and upload it as a GL_TEXTURE_2D.
   Returns the texture handle, or 0 on failure. */
GLuint loadTexture(const std::string& path);
