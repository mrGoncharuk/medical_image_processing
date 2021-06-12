#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cmath>
#include <climits>
#include <string.h>
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

# define SCREEN_WIDTH 1024
# define SCREEN_HEIGHT 1024

struct Property
{
    std::string                 description;
    std::vector<unsigned char>  mask;
};

void    read_shader_src(const char *fname, std::vector<char> &buffer);
GLuint  create_program(const char *path_vert_shader, const char *path_frag_shader);
GLuint  load_and_compile_shader(const char *fname, GLenum shaderType);
void	countHistogram(const unsigned short *image_data, int length, int channels, float hist[], float *maxVal);
void    equalizeHistogram(unsigned short* pdata, int width, int height, int channels, int max_val);
void    computeOtsusSegmentation(unsigned short *input, unsigned short *output, int width, int height, int overrided_threshold);
void    initSingleChannelImage(unsigned short *input, unsigned short *output, int length, int channels);
void    initMultiChannelImage(unsigned short *input, unsigned short *output, int length, int channels);
void	applyMask(unsigned short *image_data, unsigned short *mask, const int image_width, const int image_height, const int channels);
void    applyColorMask(unsigned short *image_data, unsigned short *mask, const int image_width, const int image_height, const int channels);
unsigned short *DoG(unsigned short *image_data, int width, int height, int channels, float sigma, float sigma2);
