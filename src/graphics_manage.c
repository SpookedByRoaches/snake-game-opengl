#include <graphics_manage.h>
#include <stdbool.h>

struct IO_handler *IO_handler_construct()
{
	struct IO_handler *input_output;
	input_output = (struct IO_handler *)malloc(sizeof(struct IO_handler));
	
	return input_output;
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
	if (width >= height)
    	glViewport((((float)width - (float)height)/2), 0, height, height);
	else
		glViewport(0, (((float)height - (float)width)/2), width, width);
}

void graphics_render_loop(struct IO_handler *input_output)
{
	while(!glfwWindowShouldClose(input_output->win))
	{
		processInput(input_output);
		create_test_graphics(input_output);
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
	static float test_vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
         -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, 0.0f, 0.0f, 0.5f, 0.5f
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
				test_vertices[i*6 + j] *= 1000.0;
		
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
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
