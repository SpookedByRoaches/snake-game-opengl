#version 460 core
layout (location = 0) in vec2 sqPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main()
{
    gl_Position = projection * view * model * vec4(sqPos, 0.0,  1.0);
}
