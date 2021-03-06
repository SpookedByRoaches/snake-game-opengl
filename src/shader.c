#include <shader.h>

struct Shader *shader_construct(const char *vertex_path, const char *fragment_path)
{
	unsigned int vert_id, frag_id, program_id;
	struct Shader *shader_obj;
	shader_obj = (struct Shader *)malloc(sizeof(struct Shader));
	vert_id = shader_compile_shader(vertex_path, GL_VERTEX_SHADER);
	frag_id = shader_compile_shader(fragment_path, GL_FRAGMENT_SHADER);
	program_id = shader_link_shader(vert_id, frag_id);
	shader_obj->id = program_id;
	
	return shader_obj;
}

unsigned int shader_link_shader(unsigned int vert_id, unsigned int frag_id)
{
	unsigned int program_id;
	int success;
	char link_message[MAX_MESSAGE_SIZE];
	program_id = glCreateProgram();
	glAttachShader(program_id, vert_id);
	glAttachShader(program_id, frag_id);
	glLinkProgram(program_id);
	glGetProgramiv(program_id, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(program_id, MAX_MESSAGE_SIZE, NULL, link_message);
		fprintf(stderr, "%s", link_message);
		throw_error("Error: shader linkage error. See message above", 0);
	}
	return program_id;
}

unsigned int shader_compile_shader(const char *shader_path, GLenum shader_type)
{
	FILE *shader_f;
	long unsigned int fsize;
	unsigned int shader_id;
	int success, fread_status;
	char *shader_buf, compile_message[MAX_MESSAGE_SIZE], error_message[MAX_MESSAGE_SIZE];
	shader_f = fopen(shader_path, "r");
	if (shader_f == NULL){
		snprintf(error_message, MAX_MESSAGE_SIZE, "Cannot open shader file %s", shader_path);
		throw_error(error_message, errno);
	}
	fseek(shader_f, 0, SEEK_END);
	fsize = ftell(shader_f);
	rewind(shader_f);
	shader_buf = (char *)malloc(fsize + 1);
	if (shader_buf == NULL){
		snprintf(error_message, MAX_MESSAGE_SIZE, "Cannot allocate memory for file buffer %s", shader_path);
		throw_error(error_message, errno);
	}
	
	if (fread(shader_buf, sizeof(char), fsize + 1, shader_f) != fsize)
		throw_error("Fread has read wrong number of characters", errno);
	fclose(shader_f);
	shader_buf[fsize] = '\0';
	shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, (const char **)&shader_buf, NULL);
	glCompileShader(shader_id);
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	free(shader_buf);
	return shader_id;
}

void throw_error(const char *message, int err_code)
{
	char *err_message[512];
	fprintf(stderr, "Error: %s\n", message);
	if (err_code == 0)
		fprintf(stderr, "Error cause unknown or cause is not a system call\n");
	else
		fprintf(stderr, "Cause: %s\n", strerror(errno));
	exit(EXIT_FAILURE);
}

void shader_use(struct Shader *shader_obj)
{
	glUseProgram(shader_obj->id);
}

void shader_set_bool(struct Shader *shader_obj, const char *name, bool val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_bool");
	glUniform1i(loc, (int)val);
	//shader_set_uniform_err(glGetError(), "shader_set_bool");
}

void shader_set_int(struct Shader *shader_obj, const char *name, int val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_int");
	glUniform1i(loc, val);
	//shader_set_uniform_err(glGetError(), "shader_set_int");
}

void shader_set_vec2i(struct Shader *shader_obj, const char *name, int *val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_vec2i");
	glUniform2i(loc, val[0], val[1]);
	//shader_set_uniform_err(glGetError(), "shader_set_vec2i");
}

void shader_set_vec3i(struct Shader *shader_obj, const char *name, int *val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_vec3i");
	glUniform3i(loc, val[0], val[1], val[2]);
	//shader_set_uniform_err(glGetError(), "shader_set_vec3i");
}

void shader_set_vec4i(struct Shader *shader_obj, const char *name, int *val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_vec4i");
	glUniform4i(loc, val[0], val[1], val[2], val[3]);
	//shader_set_uniform_err(glGetError(), "shader_set_vec4i");
}

void shader_set_float(struct Shader *shader_obj, const char *name, float val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_float");
	glUniform1f(loc, val);
	//shader_set_uniform_err(glGetError(), "shader_set_float");
}

void shader_set_vec2f(struct Shader *shader_obj, const char *name, float *val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_vec2f");
	glUniform2f(loc, val[0], val[1]);
	//shader_set_uniform_err(glGetError(), "shader_set_vec2f");
}

void shader_set_vec3f(struct Shader *shader_obj, const char *name, float *val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_vec3f");
	glUniform3f(loc, val[0], val[1], val[2]);
	//shader_set_uniform_err(glGetError(), "shader_set_vec3f");
}

void shader_set_vec4f(struct Shader *shader_obj, const char *name, float *val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_vec4f");
	glUniform4f(loc, val[0], val[1], val[2], val[3]);
	//shader_set_uniform_err(glGetError(), "shader_set_vec4f");
}

void shader_set_mat4(struct Shader *shader_obj, const char *name, float **val)
{
	int loc;
	loc = glGetUniformLocation(shader_obj->id, name);
	//shader_get_uniform_err(glGetError(), "shader_set_mat4");
	glUniformMatrix4fv(loc, 1, GL_FALSE, val);
	//shader_set_uniform_err(glGetError(), "shader_set_mat4");
}

void shader_get_uniform_err(GLenum err_code, const char *caller)
{
	char message[MAX_MESSAGE_SIZE];
	switch (err_code){
	case GL_NO_ERROR:
		break;
	case GL_INVALID_VALUE:
		snprintf(message, MAX_MESSAGE_SIZE, "%s: Program number not generated by OpenGL", caller);
		throw_error(message, 0);
		break;
	case GL_INVALID_OPERATION:
		snprintf(message, MAX_MESSAGE_SIZE, "%s: Program supplied is not a program object "
				"or could not be made part of current state", caller);
		throw_error(message, 0);
		break;
	}
}

void shader_set_uniform_err(GLenum err_code, const char *caller)
{
	char message[MAX_MESSAGE_SIZE];
	switch (glGetError()){
	case GL_NO_ERROR:
		break;
	case GL_INVALID_OPERATION:
		snprintf(message, MAX_MESSAGE_SIZE, "%s: Uniform found but could not be set");
		throw_error(message, 0);
		break;
	}
}
