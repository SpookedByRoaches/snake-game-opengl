#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 512
struct Shader {
	unsigned int id;
};

struct Shader *shader_construct(const char *vertex_path, const char *fragment_path);
void shader_use(struct Shader *shader_obj);
void shader_set_bool(struct Shader *shader_obj, const char *name, bool val);
void shader_set_int(struct Shader *shader_obj, const char *name, int val);
void shader_set_float(struct Shader *shader_obj, const char *name, float val);
void throw_error(const char *message, int err_code);
void shader_get_uniform_err(GLenum err_code, const char *caller);
void shader_set_uniform_err(GLenum err_code, const char *caller);
unsigned int shader_compile_shader(const char *shader_path, GLenum shader_type);
unsigned int shader_link_shader(unsigned int vert_id, unsigned int frag_id);
#endif
