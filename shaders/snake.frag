#version 460 core
out vec4 FragColor;

uniform vec3 sprite_col;
uniform sampler2D text;

in vec2 tex_coord;

void main()
{
    vec4 tex_color = texture(text, tex_coord);
    if(tex_color.a < 0)
        discard;
    FragColor = tex_color * vec4(sprite_col, 1.0f);
} 
