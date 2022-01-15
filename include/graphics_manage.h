#ifndef GRAPHICS_MANAGE_H
#define GRAPHICS_MANAGE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <shader.h>
#include <cglm/cglm.h>


struct IO_handler {
	struct Shader *shader;
	GLFWwindow *win;
	unsigned int sq_VBO;
	unsigned int sq_EBO;
	unsigned int sq_VAO;
};

struct IO_handler *IO_handler_construct();
int __graphics_initialize(struct IO_handler *input_output);
int graphics_create_window(struct IO_handler *input_output, int sizex, int sizey);
void __framebuffer_size_callback(GLFWwindow *window, int width, int height);
void graphics_render_loop(struct IO_handler *input_output);
void processInput(struct IO_handler *input_output);
void create_test_graphics(struct IO_handler *input_output);
void graphics_draw_square(struct IO_handler *input_output);
void __grpahics_construct_square_buffers(struct IO_handler *input_output);
#endif //GRAPHICS_MANAGE_H
