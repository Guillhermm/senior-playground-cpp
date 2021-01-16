#include "texture.h"
#include <iostream>

extern "C" {
#include "image.h"
}

GLuint loadTexture(const std::string& path) {
    IMAGE* img = ImageLoad(const_cast<char*>(path.c_str()));
    if (!img) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    /* The image loader pads each row to 4-byte alignment, so we must
       use GL_UNPACK_ALIGNMENT=4 (the OpenGL default) to match. */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    /* GL_RGBA8 avoids the unloadable-texture warning on macOS Metal bridge. */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                 img->sizeX, img->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, img->data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(img->data);
    free(img);
    return tex;
}
