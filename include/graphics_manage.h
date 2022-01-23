#ifndef GRAPHICS_MANAGE_H
#define GRAPHICS_MANAGE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <shader.h>
#include <cglm/cglm.h>


#define WINDOW_SCALE 2000.0
#define BASE_PATH "/home/bo3lwa/scripts/snake-game-opengl/assets/"
#define HEAD_IMAGE_PATH BASE_PATH "snake-head.png"
#define SEGMENT_IMAGE_PATH BASE_PATH "snake-segment.png"
#define TAIL_IMAGE_PATH BASE_PATH "snake-tail.png"
#define FOOD_IMAGE_PATH BASE_PATH "food.png"
#define DESERT_IMAGE_PATH BASE_PATH "desert.jpg"
#define SNAKE_PART_LENGTH 100.0f

enum game_object {
	head,
	body,
	tail,
	food,
	background
};

struct IO_handler {
	struct Shader *shader;
	GLFWwindow *win;
	unsigned int sq_VAO;
	unsigned int head_tex_id;
	unsigned int tail_tex_id;
	unsigned int segment_tex_id;
	unsigned int food_tex_id;
	unsigned int bg_tex_id;
	unsigned int cur_button;
};

struct IO_handler *IO_handler_construct();
int graphics_initialize(struct IO_handler *input_output);
int graphics_create_window(struct IO_handler *input_output, int sizex, int sizey);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void graphics_render_loop(struct IO_handler *input_output);
void processInput(struct IO_handler *input_output);
void create_test_graphics(struct IO_handler *input_output);
void graphics_draw_square(struct IO_handler *input_output, float *size, float *pos, float *rot, float *rgb, enum game_object which);
void grpahics_construct_square_buffers(struct IO_handler *input_output);
void APIENTRY gl_debug_callback(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                            GLsizei length, const char *message, const void *userParam);
void graphics_load_texture(struct IO_handler *input_output, const char *fname, unsigned int *id);
void graphics_clear_game_screen(struct IO_handler *input_output);
int get_image_format(const char *name);
void graphics_show(struct IO_handler *input_output);
void kb_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif //GRAPHICS_MANAGE_H
