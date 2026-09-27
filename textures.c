# 1 "textures.c"
#include "textures.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

static TextureMapEntry g_id_map[MAX_TRACKED_TEXTURES];
static int g_map_count = 0;
static GLint g_last_id = -9999999;

GLuint textures_load_texture(const char* resource_name, GLint mode) {
    (void)mode;

    for (int i = 0; i < g_map_count; i++) {
        if (strcmp(g_id_map[i].resource_name, resource_name) == 0) {
            return g_id_map[i].texture_id;
        }
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    textures_bind(texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int w, h, channels;
    unsigned char* data = stbi_load(resource_name, &w, &h, &channels, STBI_rgb_alpha);
    if (!data) {
        fprintf(stderr, "Failed to load texture file: %s\n", resource_name);
        exit(EXIT_FAILURE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    if (g_map_count < MAX_TRACKED_TEXTURES) {
        strncpy(g_id_map[g_map_count].resource_name, resource_name, 127);
        g_id_map[g_map_count].texture_id = texture_id;
        g_map_count++;
    }

    return texture_id;
}

void textures_bind(GLint id) {
    if (id != g_last_id) {
        glBindTexture(GL_TEXTURE_2D, id);
        g_last_id = id;
    }
}
