#ifndef GRAPHICS_MANAGE_H
#define GRAPHICS_MANAGE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <shader.h>


static GLFWwindow *main_win;

int graphics_initialize_window();
int graphics_create_window(int sizex, int sizey);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void graphics_render_loop(struct Shader *shader_program);
void processInput();
void create_test_graphics(struct Shader *shader_program);
#endif
