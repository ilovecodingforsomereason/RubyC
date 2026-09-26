#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/gl.h>
#include <GL/glu.h>

#define MAX_TRACKED_TEXTURES 64

typedef struct {
    char resource_name[128];
    GLuint texture_id;
} TextureMapEntry;

GLuint textures_load_texture(const char* resource_name, GLint mode);
void textures_bind(GLint id);

#endif
