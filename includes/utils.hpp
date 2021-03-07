#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string.h>
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

# define SCREEN_WIDTH 1024
# define SCREEN_HEIGHT 1024

void    read_shader_src(const char *fname, std::vector<char> &buffer);

GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);
GLuint load_and_compile_shader(const char *fname, GLenum shaderType);


void	        applyLUTRedChannel(unsigned char *image_data, unsigned char *LUT, const int image_width, const int image_height, const int channels);
unsigned char	*generateLUT(bool isStandart);
void	        applyMask(unsigned char *image_data, unsigned char *mask, const int image_width, const int image_height, const int channels);
unsigned char	*generateMask(const int image_width, const int image_height, int type);

void	countHistogram(const unsigned char *image_data, int length, int channels, float hist[], float *maxVal);
unsigned char	*histogramEqualisation(unsigned char* image_data, int cols, int rows, int channels);
void	        peakNormalization(unsigned char* image_data, int width, int heigth, int channels, int newMin, int newMax, int peakRange[2]);
