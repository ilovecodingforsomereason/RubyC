#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "timer.h"
#include "player.h"
#include "hit_result.h"
#include "textures.h"

typedef struct LevelRenderer LevelRenderer;
extern Level* level_create(int w, int h, int d);
extern void level_save(Level* level);
extern void level_set_tile(Level* level, int x, int y, int z, int tile_id);
extern LevelRenderer* level_renderer_create(Level* lvl);
extern void level_renderer_pick(LevelRenderer* lr, Player* p);
extern void level_renderer_render(LevelRenderer* lr, Player* p, int layer);
extern void level_renderer_render_hit(LevelRenderer* lr, HitResult* hr);

static int g_width = 1024;
static int g_height = 768;

static Timer g_timer;
static Level* g_level = NULL;
static LevelRenderer* g_level_renderer = NULL;
static Player g_player;
static HitResult* g_hit_result = NULL;
static double g_last_mouse_x = 0, g_last_mouse_y = 0;

static GLuint g_select_buffer[2000];

void init_game(GLFWwindow* window) {
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    
    
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    g_level = level_create(256, 256, 64);
    g_level_renderer = level_renderer_create(g_level);
    player_init(&g_player, g_level);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &g_last_mouse_x, &g_last_mouse_y);
}

void move_camera_to_player(float a) {
    glTranslatef(0.0f, 0.0f, -0.3f);
    glRotatef(g_player.xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(g_player.yRot, 0.0f, 1.0f, 0.0f);

    float x = g_player.xo + (g_player.x - g_player.xo) * a;
    float y = g_player.yo + (g_player.y - g_player.yo) * a;
    float z = g_player.zo + (g_player.z - g_player.zo) * a;
    glTranslatef(-x, -y, -z);
}

void setup_camera(float a) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, (float)g_width / (float)g_height, 0.05f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    move_camera_to_player(a);
}

void setup_pick_camera(float a, int x, int y) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix((GLdouble)x, (GLdouble)y, 5.0, 5.0, viewport);
    gluPerspective(70.0f, (float)g_width / (float)g_height, 0.05f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    move_camera_to_player(a);
}

void pick(float a) {
    glSelectBuffer(2000, g_select_buffer);
    glRenderMode(GL_SELECT);
    
    setup_pick_camera(a, g_width / 2, g_height / 2);
    level_renderer_pick(g_level_renderer, &g_player);
    
    GLint hits = glRenderMode(GL_RENDER);
    if (g_hit_result) { free(g_hit_result); g_hit_result = NULL; }

    if (hits > 0) {
        GLuint* ptr = g_select_buffer;
        GLuint closest_min_z = 0xFFFFFFFF;
        GLuint* closest_names = NULL;
        GLuint closest_count = 0;

        for (int i = 0; i < hits; i++) {
            GLuint names_count = *ptr++;
            GLuint min_z = *ptr++;
            ptr++; 
            
            if (min_z < closest_min_z || i == 0) {
                closest_min_z = min_z;
                closest_count = names_count;
                closest_names = ptr;
            }
            ptr += names_count;
        }

        if (closest_count >= 5) {
            int bx = (int)closest_names[0];
            int by = (int)closest_names[1];
            int bz = (int)closest_names[2];
            int bo = (int)closest_names[3];
            int bf = (int)closest_names[4];
            g_hit_result = hit_result_create(bx, by, bz, bo, bf);
        }
    }
}

void handle_mouse_clicks(int button, int action) {
    if (action != GLFW_PRESS || !g_hit_result) return;

    if (button == GLFW_MOUSE_BUTTON_RIGHT) { 
        level_set_tile(g_level, g_hit_result->x, g_hit_result->y, g_hit_result->z, 0);
    } 
    else if (button == GLFW_MOUSE_BUTTON_LEFT) { 
        int x = g_hit_result->x;
        int y = g_hit_result->y;
        int z = g_hit_result->z;

        if (g_hit_result->f == 0) y--;
        if (g_hit_result->f == 1) y++;
        if (g_hit_result->f == 2) z--;
        if (g_hit_result->f == 3) z++;
        if (g_hit_result->f == 4) x--;
        if (g_hit_result->f == 5) x++;

        level_set_tile(g_level, x, y, z, 1);
    }
}

void render(float a) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setup_camera(a);

    glEnable(GL_CULL_FACE);
    
    
    level_renderer_render(g_level_renderer, &g_player, 0);
    level_renderer_render(g_level_renderer, &g_player, 1);
    glEnable(GL_TEXTURE_2D);

    if (g_hit_result) {
        level_renderer_render_hit(g_level_renderer, g_hit_result);
    }
}

int main(void) {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(1024, 768, "RubyC - rd-132211 Port", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    init_game(window);
    timer_init(&g_timer, 60.0f);

    double last_fps_time = glfwGetTime();
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

        double mx, my;
        glfwGetCursorPos(window, &mx, &my);
        
        float dx = (float)(mx - g_last_mouse_x);
        float dy = (float)(my - g_last_mouse_y);
        player_turn(&g_player, dx, -dy);
        
        g_last_mouse_x = mx; g_last_mouse_y = my;

        timer_advance_time(&g_timer);
        for (int i = 0; i < g_timer.ticks; i++) {
            player_tick(&g_player, window);
        }

        pick(g_timer.a);

        static int l_state = GLFW_RELEASE, r_state = GLFW_RELEASE;
        int cl = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int cr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        if (cl != l_state) { handle_mouse_clicks(GLFW_MOUSE_BUTTON_LEFT, cl); l_state = cl; }
        if (cr != r_state) { handle_mouse_clicks(GLFW_MOUSE_BUTTON_RIGHT, cr); r_state = cr; }

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
            level_save(g_level);
        }

        render(g_timer.a);
        frames++;

        if (glfwGetTime() - last_fps_time >= 1.0) {
            printf("%d fps\n", frames);
            frames = 0;
            last_fps_time += 1.0;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (g_level) level_save(g_level);
    glfwTerminate();
    return 0;
}
