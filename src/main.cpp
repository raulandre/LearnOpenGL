#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "model.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"

#include <iostream>

const unsigned int WIDTH = 800, HEIGHT = 600;
Camera camera(glm::vec3(0.0, 0.0, 3.0));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

void processMouse(GLFWwindow*, double, double);
void processScroll(GLFWwindow*, double, double);

int main() {
    if(!glfwInit()) {
        std::cerr << "failed to init glfw\n";
        return 1;
    }

    glfwSetErrorCallback([](int code, const char *message){
        std::cerr << "[" << code << "]: " << message << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
    if(!window) {
        std::cerr << "failed to create glfw window\n";
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "failed to init glad\n";
        return 1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height){
        glViewport(0, 0, width, height);
    });

    Shader shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
    shader.Use();
    shader.SetFloat("material.shininess", 64.0f);

    Shader lightingShader("shaders/lightingVertexShader.glsl", "shaders/lightingFragmentShader.glsl");
    lightingShader.Use();

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    auto processInput = [&](GLFWwindow *window){
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.SetSpeed(50.0f);
        else
            camera.SetSpeed(20.0f);
    };

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    stbi_set_flip_vertically_on_load(true);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, processMouse);
    glfwSetScrollCallback(window, processScroll);

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    //Model backpack("models/backpack/backpack.obj");
    Model sponza("models/sponza/sponza.obj");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        shader.Use();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // directional light
        shader.SetVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.SetVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
        shader.SetVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.SetVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        shader.SetVec3("pointLights[0].position", pointLightPositions[0]);
        shader.SetVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shader.SetVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader.SetVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader.SetFloat("pointLights[0].constant", 1.0f);
        shader.SetFloat("pointLights[0].linear", 0.09f);
        shader.SetFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        shader.SetVec3("pointLights[1].position", pointLightPositions[1]);
        shader.SetVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shader.SetVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader.SetVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader.SetFloat("pointLights[1].constant", 1.0f);
        shader.SetFloat("pointLights[1].linear", 0.09f);
        shader.SetFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        shader.SetVec3("pointLights[2].position", pointLightPositions[2]);
        shader.SetVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shader.SetVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader.SetVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader.SetFloat("pointLights[2].constant", 1.0f);
        shader.SetFloat("pointLights[2].linear", 0.09f);
        shader.SetFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        shader.SetVec3("pointLights[3].position", pointLightPositions[3]);
        shader.SetVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shader.SetVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader.SetVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader.SetFloat("pointLights[3].constant", 1.0f);
        shader.SetFloat("pointLights[3].linear", 0.09f);
        shader.SetFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        shader.SetVec3("spotLight.position", camera.Position);
        shader.SetVec3("spotLight.direction", camera.Front);
        shader.SetVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.SetVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader.SetVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader.SetFloat("spotLight.constant", 1.0f);
        shader.SetFloat("spotLight.linear", 0.09f);
        shader.SetFloat("spotLight.quadratic", 0.032f);
        shader.SetFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.SetFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);
        shader.SetMat4("proj", projection);
        glm::mat4 view = camera.GetViewMatrix();
        shader.SetMat4("view", view);
        shader.SetVec3("viewPos", camera.Position);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
        shader.SetMat4("model", model);
        sponza.Draw(shader);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void processMouse(GLFWwindow*, double xPosIn, double yPosIn)
{
    auto xPos = static_cast<float>(xPosIn);
    auto yPos = static_cast<float>(yPosIn);

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; 

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void processScroll(GLFWwindow*, double, double yOffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}
