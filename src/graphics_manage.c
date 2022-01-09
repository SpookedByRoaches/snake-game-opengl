#include <graphics_manage.h>
#include <stdbool.h>

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

void graphics_render_loop()
{
	while(!glfwWindowShouldClose(main_win))
	{
		processInput();
		create_test_graphics();		
    	glfwSwapBuffers(main_win);
    	glfwPollEvents();    
	}
}

void create_test_graphics()
{	
		float test_vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		glClearColor(1.0, 1.0, 1.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
}
