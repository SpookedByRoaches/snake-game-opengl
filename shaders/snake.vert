#version 460 core
layout (location = 0) in vec2 sqPos;
layout (location = 1) in vec2 texture_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 texture_coord_o;

void main()
{
    texture_coord_o = texture_coord;
    gl_Position = projection * view * model * vec4(sqPos, 0.0,  1.0);
}
