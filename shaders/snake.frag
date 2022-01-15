#version 460 core
out vec4 FragColor;
uniform vec3 spriteCol;
in vec3 oCol;
void main()
{
    FragColor = vec4(spriteCol, 1.0f);
} 
