#version 460 core
out vec4 FragColor;

uniform vec3 sprite_col;
uniform sampler2D texture;

in vec2 texture_coord;

void main()
{
    FragColor = texture(texture, texture_coord) * vec4(spriteCol, 1.0f);
} 
