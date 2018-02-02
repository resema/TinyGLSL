#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

void render_scene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    // lookAt
    glColor3f(.8, 0.f, 1.f);
}

int main() 
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit()) return -1;

    // Create a wwindows mode window and its OpenGL context
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TinyGLSL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // render here
        render_scene();

        // swap front and back buffers
        glfwSwapBuffers(window);

        // poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}