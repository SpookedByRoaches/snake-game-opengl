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
		static unsigned int VBO = 6969;
		static unsigned int VAO;
		static unsigned int shaderProgram;
		float test_vertices[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f,  0.5f, 0.0f
		};
		if (VBO == 6969){
			printf("OK");
			const char *vertexShaderSource = "#version 330 core\n"
				"layout (location = 0) in vec3 aPos;\n"
				"void main()\n"
				"{\n"
				"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
				"}\0";
			const char *fragmentShaderSource = " #version 330 core\n"
				"out vec4 FragColor;\n"
				"void main(){\n"
	    		"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
				"}\0";
			unsigned int vertexShader;
			vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
			glCompileShader(vertexShader);
			unsigned int fragmentShader;
			fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
			glCompileShader(fragmentShader);
			int success;
			char infoLog[512];
    		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
					glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
					fprintf(stderr, "Failed vertex shader compilation \n%s\n", infoLog);
			}
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
					glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
					fprintf(stderr, "Failed fragment shader compilation \n%s\n", infoLog);
			}
			shaderProgram = glCreateProgram();
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);
			glUseProgram(shaderProgram);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			glGenBuffers(1, &VBO);  
			glGenVertexArrays(1, &VAO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO); 
			glBufferData(GL_ARRAY_BUFFER, sizeof(test_vertices), test_vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0); 
			glBindVertexArray(VAO);
			glUseProgram(shaderProgram);
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				fprintf(stderr, "Failed program making \n%s\n", infoLog);
			}
			fflush(stderr);
		}
		glClearColor(1.0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
		}
