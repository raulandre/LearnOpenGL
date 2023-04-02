#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <reactphysics3d/reactphysics3d.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <unistd.h>

#include "glm/gtc/type_ptr.hpp"
#include "model.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"

using namespace reactphysics3d;

const unsigned int WIDTH = 1366, HEIGHT = 768;
Camera camera(glm::vec3(0.0, 0.0, 3.0));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

void processMouse(GLFWwindow*, double, double);
void processScroll(GLFWwindow*, double, double);
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma);
unsigned int loadCubemap(std::vector<std::string> faces);

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

    Shader shader("shaders/vertexShader.glsl", "shaders/geometryShader.glsl", "shaders/fragmentShader.glsl");
    shader.Use();
    shader.SetFloat("material.shininess", 64.0f);

    Shader skyboxShader("shaders/skyboxVertex.glsl", "shaders/skyboxFrag.glsl");

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    bool propulsion = false;
    auto processInput = [&](GLFWwindow *window){
        ImGuiIO &io = ImGui::GetIO();
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            propulsion = true;
        else propulsion = false;

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
            return;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.SetSpeed(10.0f);
        else
            camera.SetSpeed(5.0f);

    };

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glfwSetCursorPosCallback(window, processMouse);
    glfwSetScrollCallback(window, processScroll);
    glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int mods){
        ImGuiIO &io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, action == GLFW_PRESS);
        if(io.WantCaptureMouse)
            return;
    });
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    unsigned int VAO, VBO;
    unsigned int cubemapVAO, cubemapVBO;
    glGenVertexArrays(1, &cubemapVAO);
    glBindVertexArray(cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    auto index1 = glGetUniformBlockIndex(shader.GetId(), "Matrices");
    auto index2 = glGetUniformBlockIndex(skyboxShader.GetId(), "Matrices");
    glUniformBlockBinding(shader.GetId(), index1, 0);
    glUniformBlockBinding(skyboxShader.GetId(), index2, 0);

    unsigned int UBO;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(glm::mat4));

    std::vector<std::string> faces
    {
        "textures/right.jpg",
        "textures/left.jpg",
        "textures/top.jpg",
        "textures/bottom.jpg",
        "textures/front.jpg",
        "textures/back.jpg"
    };
    auto cubemapTexture = loadCubemap(faces);

    stbi_set_flip_vertically_on_load(true);
    Model backpack("models/backpack/backpack.obj");

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();
        shader.SetVec3("viewPos", camera.Position);
        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0, 0.0, -3.0f));
        shader.SetMat4("model", model);
        shader.SetFloat("time", glfwGetTime());
        backpack.Draw(shader);

        glDepthMask(GL_FALSE);
        skyboxShader.Use();
        skyboxShader.SetInt("skybox", 0);
        glBindVertexArray(cubemapVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            static float f = 0.0f;
            ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Position: %.1f, %.1f, %.1f", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void processMouse(GLFWwindow *window, double xPosIn, double yPosIn)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddMousePosEvent(xPosIn, yPosIn);
    if(io.WantCaptureMouse)
        return;

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

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera.ProcessMouseMovement(xOffset, yOffset);
    }
}

void processScroll(GLFWwindow *, double xOffset, double yOffset)
{
    ImGuiIO &io = ImGui::GetIO();
    io.AddMouseWheelEvent(xOffset, yOffset);
    if(io.WantCaptureMouse)
        return;

    camera.ProcessMouseScroll(static_cast<float>(yOffset));
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for(auto i = 0U; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        } else {
            std::cerr << "Cubemap tex failed to load:" << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}