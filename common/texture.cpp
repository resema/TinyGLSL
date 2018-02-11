#include "texture.hpp"

GLuint loadBMP(const char* imagepath)
{
    printf("Reading image %s\n", imagepath);

    // data read from the header of the BMP file
    unsigned char header[54];   // 54-bytes header of BMP file
    unsigned int dataPos;       // position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;     // = width * height * 3
    // actual RGB data
    unsigned char* data;

    // open the file
    FILE* file = fopen(imagepath, "rb");    // "rb" := read binary
    if (!file) {
        printf("%s could not opened. Are you in the right directory?\n", imagepath);
        getchar();
        return 0;
    }
}