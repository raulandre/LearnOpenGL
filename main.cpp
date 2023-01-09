#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
const unsigned int WIDTH = 800, HEIGHT = 600;

int main() {
    if(!glfwInit()) {
        cerr << "failed to init glfw\n";
        return 1;
    }

    glfwWindowHint(GLFW_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_VERSION_MINOR, 3);

    auto *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);
    if(!window) {
        cerr << "failed to create glfw window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cerr << "failed to init glad\n";
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height){
        glViewport(0, 0, width, height);
    });

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.2f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
