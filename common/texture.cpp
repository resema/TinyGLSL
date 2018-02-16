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

    // read the header, i.e. the 54 first bytes
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    // a BMP file always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    // make sure this is a 24 bpp file
    //  check https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
    if (*(int*)&(header[0x1E]) != 0) {          // compression method
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    if (*(int*)&(header[0x1C]) != 0) {          // number of bits per pixel
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }

    // read the information about the image
    dataPos     = *(int*)&(header[0x0A]);       // offset, i.e. starting address
    imageSize   = *(int*)&(header[0x22]);       // size of the raw bitmap data
    width       = *(int*)&(header[0x12]);       // bitmap width in pixel
    height      = *(int*)&(header[0x16]);       // bitmap height in pixel

    // if some BMP files are misformatted, guess missing information
    if (imageSize == 0)
        imageSize = width * height * 3;     // 3: one byte for each Red, Green and Blue
    if (dataPos == 0)
        dataPos = 54;                       // BMP header is done that way
}