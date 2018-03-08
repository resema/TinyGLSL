#include "controls.hpp"

extern GLFWwindow* window;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() 
{
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix()
{
    return ProjectionMatrix;
}

// initial position : on +Z
glm::vec3 position = glm::vec3(0,0,5);
// initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// initial vertical angle : none
float verticalAngle = 0.f;
// initial Field of View
float initialFoV = 45.f;

float speed = 3.f;  // 3 units per second
float mouseSpeed = 0.005f;


void computeMatricesFromInputs()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // reset mouse position for next frame
    glfwSetCursorPos(window, 1024/2, 768/2);

    // compute new orientation
    horizontalAngle += mouseSpeed * float(1024/2 - xpos);
    verticalAngle   += mouseSpeed * float( 768/2 - ypos);

    // direction : spherical coord to cartesian coord conversion
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // right vector
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f/2.f),
        0,                                  // always horizontal
        cos(horizontalAngle - 3.14f/2.f)
    );

    // up vector
    glm::vec3 up = glm::cross(right, direction);

    // move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * deltaTime * speed;
    }
    // move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * deltaTime * speed;
    }
    // move forward
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    // move forward
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
    // move up
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position += up * deltaTime * speed;
    }
    // move down
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        position -= up * deltaTime * speed;
    }
    
    // Now GLFW3 requires setting a callback for this
    float FoV = initialFoV;

    // projection matrix : 45˚ FoV, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(
        glm::radians(FoV),  // fovy
        4.f / 3.f,          // aspect
        0.1f,               // near
        100.f               // far
    );

    // camera matrix
    ViewMatrix = glm::lookAt(
        position,               // camera is here
        position + direction,   // and looks here
        up                      // head is up
    );

    // for the next frame, the last time will be now
    lastTime = currentTime;
}