#include <graphics_manage.h>
#include <stdbool.h>
#include <math.h>

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

	graphics_create_window(input_output, 800, 600);
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
    glViewport(0, 0, width, height);
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
		static unsigned int VAO;
		static mat4 transform;
		static vec3 translation;
		static float test_vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
			0.0f,  0.5f, 0.0f, 	0.0f, 0.0f, 1.0f
		};

		translation[0] = 0.2;
		translation[1] = 0.2;
		translation[2] = 0.0;

		glm_translate(transform, translation);



		if (VBO == 6969){
			glGenBuffers(1, &VBO);
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO); 
			glBufferData(GL_ARRAY_BUFFER, sizeof(test_vertices), test_vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			unsigned int transformLoc = glGetUniformLocation(input_output->shader->id, "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);
		}
		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		shader_use(input_output->shader);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
		}
