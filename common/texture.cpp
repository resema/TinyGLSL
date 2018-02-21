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
        printf("Not a correct BMP file (0)\n");
        fclose(file);
        return 0;
    }
    // a BMP file always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file (1)\n");
        fclose(file);
        return 0;
    }
    // make sure this is a 24 bpp file
    //  check https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
    if (*(int*)&(header[0x1E]) != 0) {          // compression method
        printf("Not a correct BMP file (2)\n");
        fclose(file);
        return 0;
    }
    if (*(int*)&(header[0x1C]) != 24) {          // number of bits per pixel
        printf("Not a correct BMP file (3)\n");
        fclose(file);
        return 0;
    }

    // read the information about the image
    dataPos     = *(int*)&(header[0x0A]);       // offset, i.e. starting address
    imageSize   = *(int*)&(header[0x22]);       // size of the raw bitmap data
    width       = *(int*)&(header[0x12]);       // bitmap width in pixel
    height      = *(int*)&(header[0x16]);       // bitmap height in pixel

    // if some BMP files are misformatted, guess missing information
    if (imageSize == 0) {
        imageSize = width * height * 3;     // 3: one byte for each Red, Green and Blue
    }
    if (dataPos == 0) {
        dataPos = 54;                       // BMP header is done that way
    }

    // create a buffer
    data = new unsigned char[imageSize];

    // read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    // everything is in memory now, the file can be closed
    fclose(file);

    // CREATE one OpenGL texture
    GLuint textureID;
    glGenTextures(
        1,          // number of texture names to be generated
        &textureID  // array which stores the generated texture names  
        );  

    // BIND the newly created texture: all future texture functions will modify texture
    glBindTexture(
        GL_TEXTURE_2D,  // specify target
        textureID       // texture name
        );

    // FILL the image and give it to OpenGL
    glTexImage2D(
        GL_TEXTURE_2D,      // target texture
        0,                  // level of detail
        GL_RGB,             // internal format
        width, height,      // texture image
        0,                  // border
        GL_BGR,             // data format
        GL_UNSIGNED_BYTE,   // data type
        data                // ptr to image data
        );

    // OpenGL has now copied the data
    delete [] data;

    // CONFIGURE it (poor filtering)
    // glTexParameteri(
    //     GL_TEXTURE_2D,          // target texture
    //     GL_TEXTURE_MAG_FILTER,  // symbolic name
    //     GL_NEAREST              // value of name
    // );
    // glTexParameteri(
    //     GL_TEXTURE_2D,
    //     GL_TEXTURE_MIN_FILTER,
    //     GL_NEAREST
    // );

    // CONFIGURE it (trilinear filtering)
    glTexParameteri(
        GL_TEXTURE_2D,              // target texture
        GL_TEXTURE_WRAP_S,          // symbolic name
        GL_REPEAT                   // value of name
    );
    glTexParameteri(
        GL_TEXTURE_2D,              // target texture
        GL_TEXTURE_WRAP_T,          // symbolic name
        GL_REPEAT                   // value of name
    );
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );
    // ... which requires mipmaps. -> generate them automatically
    glGenerateMipmap(GL_TEXTURE_2D);

    // return the ID of the texture just created
    return textureID;
}

GLuint loadDDS(const char* imagepath)
{
    printf("Reading image %s\n", imagepath);

    constexpr unsigned int FOURCC_DXT1 = 0x31545844; // Equivalent to "DXT1" in ASCII
    constexpr unsigned int FOURCC_DXT3 = 0x33545844; // Equivalent to "DXT3" in ASCII
    constexpr unsigned int FOURCC_DXT5 = 0x35545844; // Equivalent to "DXT5" in ASCII

    unsigned char header[124];

    FILE* fp;

    // try to open the file
    fp = fopen(imagepath, "rb");
    if (fp == NULL) {
        printf("%s could not be open.\n", imagepath);
        getchar();
        return 0;
    }

    // verify the type of file
    char filecode[4];
    fread(filecode, 1, 4, fp);
    if (strncmp(filecode, "DDS ", 4) != 0) {
        printf("Unknown file type\n");
        fclose(fp);
        return 0;
    }

    // get the surface desc
    // https://msdn.microsoft.com/en-us/library/bb943982.aspx
    fread(&header, 124, 1, fp);

    unsigned int height         = *(unsigned int*)&(header[8]);     // third WORD of DDS_HEADER struct
    unsigned int width          = *(unsigned int*)&(header[12]);    // forth WORD
    unsigned int linearSize     = *(unsigned int*)&(header[16]);    // fifth WORD
    unsigned int mipMapCount    = *(unsigned int*)&(header[24]);    // seventh WORD
    unsigned int fourCC         = *(unsigned int*)&(header[80]);    // third WORD in DDS_PIXELFORMAT

    unsigned char* buffer;
    unsigned int bufsize;

    // how big is it going to be including all mipmaps?
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
    fread(buffer, 1, bufsize, fp);
    fclose(fp);

    // unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
    unsigned int format;
    switch (fourCC) {
    case FOURCC_DXT1:
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        printf("Texture with format DXT1\n");
        break;
    case FOURCC_DXT3:
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        printf("Texture with format DXT3\n");
        break;
    case FOURCC_DXT5:
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        printf("Texture with format DXT5\n");
        break;
    default:
        free(buffer);
        printf("No known format\n");
        return 0;
    }

    // CREATE one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // BIND the created texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(              // set pixel storage modes
        GL_UNPACK_ALIGNMENT,    // specifies the alignment requirements for the start of each pixel row in memory
        1                       // 1 byte-alignment
        );

    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;

    // load the mipmaps
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
        unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
        glCompressedTexImage2D(
            GL_TEXTURE_2D,      // target texture
            level,              // level-of-detail (0 is base level)
            format,             // format of image data
            width, height,
            0,                  // border
            size,               // number of unsigned bytes of image data
            buffer + offset     // ptr to the compressed image data
        );

        offset += size;
        width /= 2;
        height /= 2;

        // deal with non-power-of-two textures
        if (width < 1) width = 1;
        if (height < 1) height = 1;
    }

    free(buffer);

    return textureID;
}