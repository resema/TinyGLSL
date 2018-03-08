#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <stdio.h>
#include <stdlib.h>         // used for malloc()
#include <string.h>         // used for strncmp()

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// load a .bmp file using this custom loader
GLuint loadBMP(const char* imagepath);

GLuint loadDDS(const char* imagepath);

#endif  // TEXTURE_HPP