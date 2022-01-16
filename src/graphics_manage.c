#include <graphics_manage.h>
#include <stdbool.h>

struct IO_handler *IO_handler_construct()
{
	struct IO_handler *input_output;
	input_output = (struct IO_handler *)malloc(sizeof(struct IO_handler));
	input_output->sq_VAO = 0;
	graphics_initialize(input_output);
	grpahics_construct_square_buffers(input_output);
	return input_output;
}

void graphics_draw_square(struct IO_handler *input_output, float *size, float *pos, float *rot, float *rgb)
{
	shader_use(input_output->shader);
	mat4 model, projection, view;
	vec3 trans = {pos[0], pos[1], 0};
	glm_mat4_identity(projection);
	glm_mat4_identity(model);
	glm_mat4_identity(view);

	shader_set_mat4(input_output->shader, "view", view);
	
	glm_ortho(-WINDOW_SCALE, WINDOW_SCALE, -WINDOW_SCALE, WINDOW_SCALE, -0.1, 100, projection);
	glm_translate(model, trans);
	glm_rotate(model, *rot, (vec3){0.0f, 0.0f, 1.0f});
	glm_scale(model, (vec3){size[0], size[1], 1.0f});
	

	shader_set_mat4(input_output->shader, "projection", projection);
	shader_set_mat4(input_output->shader, "model", model);

	shader_set_vec3f(input_output->shader, "sprite_col", rgb);
	glBindVertexArray(input_output->sq_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void grpahics_construct_square_buffers(struct IO_handler *input_output)
{
	unsigned int e, VBO, EBO;
	float vertices[] = {
		-50.0f, -50.0f, 1.0f, 1.0f,
         50.0f, -50.0f, 1.0f, 0.0f,
        -50.0f,  50.0f, 0.0f, 0.0f,
         50.0f,  50.0f, 0.0f, 1.0f
	};

	static unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	glGenBuffers(1, &(VBO));
	glGenBuffers(1, &(EBO));
	glGenVertexArrays(1, &(input_output->sq_VAO));

	glBindVertexArray(input_output->sq_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
}


int graphics_initialize(struct IO_handler *input_output)
{
	int flags;
	if (!glfwInit())
		return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	
	graphics_create_window(input_output, 800, 800);
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
		glDebugMessageCallback(gl_debug_callback, (void *)NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, (void *)NULL, GL_TRUE);
	}
	input_output->shader = shader_construct("shaders/snake.vert", "shaders/snake.frag");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

int graphics_create_window(struct IO_handler *input_output, int sizex, int sizey)
{
	input_output->win = glfwCreateWindow(sizex, sizey, "SNAAAKE", NULL, NULL);
	if (input_output->win == NULL)
		goto create_win_error;
	
	glfwMakeContextCurrent(input_output->win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		goto create_win_error;
	
	glViewport(0, 0, sizex, sizey);
	glfwSetFramebufferSizeCallback(input_output->win, framebuffer_size_callback);
	return 1;
create_win_error:
    glfwTerminate();
    return -1;
}

void processInput(struct IO_handler *input_output)
{
    if(glfwGetKey(input_output->win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(input_output->win, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	float cur_ratio = (float)width/(float)height;
	float left, bottom;
	if (width >= height){
		left = (((float)width - (float)height)/2);
		bottom = 0;
	}
	else{
		left = 0;
		bottom = (((float)height - (float)width)/2);
	}
	glViewport(left, bottom, height, height);
	glScissor(left,bottom,width,height);
}

void graphics_render_loop(struct IO_handler *input_output)
{
	float size[2] = {1.0f, 1.0f};
	float col[3] = {1.0f, 0.0f, 0.0f};
	float pos[2] = {500, 300};
	float rot = GLM_PI_4f;
	float rot2 = 0;
	while(!glfwWindowShouldClose(input_output->win))
	{
		processInput(input_output);
		//create_test_graphics(input_output);
		graphics_draw_square(input_output, size, pos, &rot, col);
    	glfwSwapBuffers(input_output->win);
    	glfwPollEvents();
		glClearColor(0, 0.3, 0.3, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void create_test_graphics(struct IO_handler *input_output)
{
	static unsigned int VBO = 6969;
	static unsigned int VAO, e, EBO, u_model, u_view, u_projection, width, height;
	static mat4 view_mat, model_mat, projection_mat;
	glfwGetWindowSize(input_output->win, &width, &height);
	float test_vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f
	};
	static unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	glm_mat4_identity(view_mat);

	glm_mat4_identity(projection_mat);

	glm_translate(view_mat, (vec3){0.0, 0.0, -10.0});

	glm_mat4_identity(model_mat);

	glm_translate(model_mat, (vec3){0.0, 0.0, 0});

	glm_ortho(-1000.0, 1000.0, -1000.0, 1000.0, 0.1, 100, projection_mat);

	//glm_perspective(GLM_PI_4, (float)width/(float)height, 0.1, 100, projection_mat);

	if (VBO == 6969){
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 3; j++)
				test_vertices[i*6 + j] *= 500.0;
		
		glGenBuffers(1, &EBO);
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(test_vertices), test_vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		shader_use(input_output->shader);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	shader_use(input_output->shader);
	u_model = glGetUniformLocation(input_output->shader->id, "model");
	u_projection = glGetUniformLocation(input_output->shader->id, "projection");
	u_view = glGetUniformLocation(input_output->shader->id, "view");
	glUniformMatrix4fv(u_model, 1, GL_FALSE, model_mat);
	glUniformMatrix4fv(u_projection, 1, GL_FALSE, projection_mat);
	glUniformMatrix4fv(u_view, 1, GL_FALSE, view_mat);
	e = glGetError();
	glClearColor(0, 0.3, 0.3, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void APIENTRY gl_debug_callback(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    printf("---------------");
    printf("Debug message (%d): %s\n", id, message);

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             printf("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     printf("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           printf("Source: Other"); break;
    } 
	printf("\n");

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               printf("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  printf("Type: Undefined Behaviour"); break; 
        case GL_DEBUG_TYPE_PORTABILITY:         printf("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         printf("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              printf("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          printf("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           printf("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               printf("Type: Other"); break;
    } 
	printf("\n");
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         printf("Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       printf("Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          printf("Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Severity: notification"); break;
    }
	printf("\n");

}