#include <graphics_manage.h>
#include <stdbool.h>
#include <math.h>
int graphics_initialize_window()
{
	if (!glfwInit())
		return -1;
	
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

int graphics_create_window(int sizex, int sizey)
{
	main_win = glfwCreateWindow(sizex, sizey, "SNAAAKE", NULL, NULL);
	if (main_win == NULL)
		goto create_win_error;
	
	glfwMakeContextCurrent(main_win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		goto create_win_error;
	
	glViewport(0, 0, sizex, sizey);
	glfwSetFramebufferSizeCallback(main_win, framebuffer_size_callback);
	return 1;
create_win_error:
    glfwTerminate();
    return -1;
}

void processInput()
{
    if(glfwGetKey(main_win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(main_win, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void graphics_render_loop(struct Shader *shader_program)
{
	while(!glfwWindowShouldClose(main_win))
	{
		processInput();
		create_test_graphics(shader_program);
    	glfwSwapBuffers(main_win);
    	glfwPollEvents();    
	}
}

void create_test_graphics(struct Shader *shader_program)
{	
		static unsigned int VBO = 6969;
		static unsigned int VAO;
		float test_vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		if (VBO == 6969){
			fflush(stdout);
			shader_use(shader_program);
			glGenBuffers(1, &VBO);
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO); 
			glBufferData(GL_ARRAY_BUFFER, sizeof(test_vertices), test_vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0); 
			glBindVertexArray(VAO);
		}
		glClearColor(1.0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		shader_use(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
		}
