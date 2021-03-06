// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/text2D.hpp>
#include <common/tangentspace.hpp>

int main( void )
{
	// Initialise GLFW
	if (!glfwInit()) 
    {
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
	window = glfwCreateWindow( 640, 480, "TinyGLSL", NULL, NULL);
	if (window == NULL) 
    {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

    // Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) 
    {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

    // Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);    // disable in case of opacity
    // enable depth test
    glEnable(GL_DEPTH_TEST);
    // accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("shaders/NormalMapping.vs", "shaders/NormalMapping.fs");

    // get a handle for our "MVP" uniform 
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    GLuint ModelView3x3MatrixID = glGetUniformLocation(programID, "MV3x3");

    // load the texture
    GLuint DiffuseTexture = loadDDS("textures/diffuse.DDS");
    GLuint NormalTexture = loadBMP("textures/normal.bmp");
    GLuint SpecularTexture = loadDDS("textures/specular.DDS");

    // get a handle for "myTextureSampler" uniform
    GLuint DiffuseTextureID = glGetUniformLocation(
        programID,                  // program object
        "DiffuseTextureSampler"     // name of uniform variable
    );
    GLuint NormalTextureID = glGetUniformLocation(
        programID,                  // program object
        "NormalTextureSampler"      // name of uniform variable
    );
    GLuint SpecularTextureID = glGetUniformLocation(
        programID,                  // program object
        "SpecularTextureSampler"    // name of uniform variable
    );

    // read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("models/cylinder.obj", vertices, uvs, normals);
    if (!res)
    {
        fprintf(stderr, "Failed to load .OBJ model\n");
		getchar();
		glfwTerminate();
		return -1;
    }

    // calculate tangent basis
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    computeTangentBasis(
        vertices, uvs, normals, // inputs
        tangents, bitangents    // outputs
    );

    // index VBO
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    std::vector<glm::vec3> indexed_tangents;
    std::vector<glm::vec3> indexed_bitangents;
    indexVBO_TBN(
        vertices, uvs, normals, tangents, bitangents,
        indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents
        );

    //
    // load it into a VBO

    // identify our vertex buffer
    GLuint vertexbuffer;
    // generate one buffer, put identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // bind buffer object to specified buffer binding point
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // give our vertices to OpenGL
    glBufferData(
        GL_ARRAY_BUFFER, 
        indexed_vertices.size() * sizeof(glm::vec3), 
        &indexed_vertices[0], 
        GL_STATIC_DRAW
        );

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        indexed_uvs.size() * sizeof(glm::vec2), 
        &indexed_uvs[0], 
        GL_STATIC_DRAW
        );

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, 
        indexed_normals.size() * sizeof(glm::vec3), 
        &indexed_normals[0], 
        GL_STATIC_DRAW
        );

    GLuint tangentbuffer;
    glGenBuffers(1, &tangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        indexed_tangents.size() * sizeof(glm::vec3),
        &indexed_tangents[0],
        GL_STATIC_DRAW
    );

    GLuint bitangentbuffer;
    glGenBuffers(1, &bitangentbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        indexed_bitangents.size() * sizeof(glm::vec3),
        &indexed_bitangents[0],
        GL_STATIC_DRAW
    );

    // generate a buffer for the indices as well
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        indices.size() * sizeof(unsigned short), 
        &indices[0], 
        GL_STATIC_DRAW
        );

    // get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    // initialize our little text library with the Holstein font
    initText2D("textures/Holstein.DDS");     // contains hardcoded shaders

    // for speed computation
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    do {
        // measure speed
        double currentTime = glfwGetTime();
        nbFrames++;
        if (currentTime - lastTime >= 1.0)  // if last printf() was more then 1sec ago
        {
            // printf and reset
            printf("%f ms/frame\n", 1000.0 / double(nbFrames));
            nbFrames = 0;
            lastTime += 1.0;    // deltaT is 1sec
        }

        // clear the screen.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // use our shader
        glUseProgram(programID);

        // compute the mvp matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
        glm::mat3 MV3x3Matrix = glm::mat3(ModelViewMatrix);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // send our transformation to the currently bound shader 
        //  in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        glUniformMatrix3fv(ModelView3x3MatrixID, 1, GL_FALSE, &MV3x3Matrix[0][0]);

        glm::vec3 lightPos = glm::vec3(4,4,4);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

        // bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, DiffuseTexture);
        // set "DiffuseTextureSampler" sampler to use Texture Unit 0
        glUniform1i(DiffuseTextureID, 0);

        // bind our normal texture in Texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, NormalTexture);
        // set "NormalTextureSampler" sampler to use Texture Unit 1
        glUniform1i(NormalTextureID, 1);

        // bind our specular texture in Texture unit 2
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, SpecularTexture);
        // set "NormalTextureSampler" sampler to use Texture Unit 2
        glUniform1i(SpecularTextureID, 2);

        // 1st attribute buffer: vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,          // attribute 0, must match the layout in the shader
            3,          // size
            GL_FLOAT,   // type
            GL_FALSE,   // normalized?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            1,          // attribute 1
            2,          // size : U+V => 2
            GL_FLOAT,   // type
            GL_FALSE,   // normalized?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(
            2,          // attribute 2
            3,          // size : normals => 3
            GL_FLOAT,   // type
            GL_FALSE,   // normalized?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 4th attribute buffer : tangents
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
        glVertexAttribPointer(
            3,          // attribute 3
            3,          // size : normals => 3
            GL_FLOAT,   // type
            GL_FALSE,   // normalized?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // 5th attribute buffer : bitangents
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
        glVertexAttribPointer(
            4,          // attribute 4
            3,          // size : normals => 3
            GL_FLOAT,   // type
            GL_FALSE,   // normalized?
            0,          // stride
            (void*)0    // array buffer offset
        );

        // index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // draw the triangles from the VBO
        glDrawElements(
            GL_TRIANGLES,       // mode
            indices.size(),     // count
            GL_UNSIGNED_SHORT,  // type
            (void*)0            // element array buffer offset
            );

        // disable connection to the shader
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);

        char text[256];
        sprintf(text, "%.2f sec", glfwGetTime());
        printText2D(
            text,   // text to be displayed
            10,     // position x
            500,    // position y
            30      // size
        );

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // check if the ESC kez was pressed or the window closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0);

    // cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &tangentbuffer);
    glDeleteBuffers(1, &bitangentbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &DiffuseTexture);
    glDeleteTextures(1, &NormalTexture);
    glDeleteTextures(1, &SpecularTexture);
    glDeleteVertexArrays(1, &VertexArrayID);

    // delete the text's VBO, the shader and the texture
    cleanupText2D();

    // close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}