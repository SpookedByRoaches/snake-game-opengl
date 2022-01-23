#version 460 core
layout (location = 0) in vec2 sqPos;
layout (location = 1) in vec2 texture_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 tex_coord;

void main()
{
    tex_coord = texture_coord;
    gl_Position = projection * view * model * vec4(sqPos, 0.0,  1.0);
}
