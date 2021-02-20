#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>

# define SCREEN_WIDTH 512
# define SCREEN_HEIGHT 512

void    read_shader_src(const char *fname, std::vector<char> &buffer);

GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);
GLuint load_and_compile_shader(const char *fname, GLenum shaderType);