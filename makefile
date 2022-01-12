LINKED_LIBS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lncurses -lm -lcglm
INCLUDE = -Iinclude
FLAGS = -g
SRC_FILES = src/snake_opengl.c
HEADERS = include/snake_opengl.h include/graphics_manage.h
OBJ = glad.o graphics_manage.o shader.o

snake_opengl: $(OBJ) $(SRC_FILES) $(HEADERS)
	gcc -o snake_opengl $(OBJ) $(SRC_FILES) $(LINKED_LIBS) $(INCLUDE) $(FLAGS)

glad.o:
	gcc src/glad.c -c $(INCLUDE) -g

shader.o: src/shader.c $(HEADERS)
	gcc src/shader.c -c $(INCLUDE) -g

graphics_manage.o: src/graphics_manage.c $(HEADERS)
	gcc src/graphics_manage.c -c $(INCLUDE) -g

clean: $(OBJ)
	rm $(OBJ)

.PHONY: snake_opengl graphics_manage.o shader.o
