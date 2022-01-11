#version 460 core
out vec4 FragColor;
in vec3 oCol;
void main()
{
    FragColor = vec4(oCol, 1.0f);
} 
