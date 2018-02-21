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


void computeMatricesFromInput()
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
        0,
        cos(horizontalAngle - 3.14f/2.f)
    );

    // up vector
    glm::vec3 up = glm::cross(right, direction);

    // move forward
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        position += direction * deltaTime * speed;
    }
    // move backward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        position -= direction * deltaTime * speed;
    }
    // move forward
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        position += right * deltaTime * speed;
    }
    // move forward
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        position -= right * deltaTime * speed;
    }
}