#version 330 core
layout (location = 0) in vec3 pos; // <vec2 pos, vec2 tex>
layout (location = 1) in vec2 tex;
out vec2 TexCoords;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(pos.xyz, 1.0);
    TexCoords = tex;
}  