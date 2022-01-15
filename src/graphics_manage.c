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
	glm_mat4_identity(projection);
	glm_mat4_identity(model);
	glm_mat4_identity(view);

	shader_set_mat4(input_output->shader, "view", view);
	//shader_set_mat4(input_output->shader, "translation", translation);
	//shader_set_mat4(input_output->shader, "rotation", rotation);
	glm_ortho(-1000.0, 1000.0, -1000.0, 1000.0, 0.1, 100, projection);
	shader_set_mat4(input_output->shader, "projection", projection);
	shader_set_mat4(input_output->shader, "model", model);

	float col[3] = {1.0f, 1.0f, 1.0f};

	shader_set_vec3f(input_output->shader, "spriteCol", col);
	glBindVertexArray(input_output->sq_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void grpahics_construct_square_buffers(struct IO_handler *input_output)
{
	unsigned int e, VBO, EBO;
	float vertices[] = {
		-500.0f, -500.0f,
         500.0f, -500.0f,
         -500.0f,  500.0f,
         500.0f,  500.0f,
	};

	static unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	glGenBuffers(1, &(VBO));
	glGenBuffers(1, &(EBO));
	glGenVertexArrays(1, &(EBO));
	glBindVertexArray(&input_output->sq_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, &VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(2 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	e = glGetError();
}


int graphics_initialize(struct IO_handler *input_output)
{
	if (!glfwInit())
		return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	graphics_create_window(input_output, 800, 800);
	input_output->shader = shader_construct("shaders/snake.vert", "shaders/snake.frag");
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
	while(!glfwWindowShouldClose(input_output->win))
	{
		processInput(input_output);
		//create_test_graphics(input_output);
		graphics_draw_square(input_output, NULL, NULL, NULL, NULL);
    	glfwSwapBuffers(input_output->win);
    	glfwPollEvents();
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