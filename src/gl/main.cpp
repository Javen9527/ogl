// Created by jianbo
// Date: 2021/11/27
// All rights reserved

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaderManager/ShaderManager.h"
#include "camera/camera.h"
#include "model/model.h"

#include <iostream>
#include <string>

namespace wind
{
    // viewport
    const unsigned int SCR_WIDTH = 1600;
    const unsigned int SCR_HEIGHT = 1200;

    // camera
    cam::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;
    bool firstMouse = true;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

} // namespace master

GLFWwindow *init(const std::string name, const unsigned int width, const unsigned int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(width, height, "ogl", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    
    // cursor
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // wind changes
    auto frameBufferCallback = [](GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);

    // mouse move
    auto mouseCallback = [](GLFWwindow *window, double xpos, double ypos)
    {
        if (wind::firstMouse)
        {
            wind::lastX = xpos;
            wind::lastY = ypos;
            wind::firstMouse = false;
        }

        float xoffset = xpos - wind::lastX;
        // reversed since y-coordinates go from bottom to top
        float yoffset = wind::lastY - ypos; 

        wind::lastX = xpos;
        wind::lastY = ypos;

        wind::camera.ProcessMouseMovement(xoffset, yoffset);
    };
    glfwSetCursorPosCallback(window, mouseCallback);

    // scroll
    auto scrollCallback = [](GLFWwindow *window, double xoffset, double yoffset)
    {
        wind::camera.ProcessMouseScroll(yoffset);
    };
    glfwSetScrollCallback(window, scrollCallback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}

void render(GLFWwindow *window, ShaderManager &pShader, Model &pModel)
{
    if (!window)
        return;

    glEnable(GL_DEPTH_TEST);

    auto processInput = [](GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            wind::camera.ProcessKeyboard(cam::eFORWARD, wind::deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            wind::camera.ProcessKeyboard(cam::eBACKWARD, wind::deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            wind::camera.ProcessKeyboard(cam::eLEFT, wind::deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            wind::camera.ProcessKeyboard(cam::eRIGHT, wind::deltaTime);
    };

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        wind::deltaTime = currentFrame - wind::lastFrame;
        wind::lastFrame = currentFrame;

        // input
        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pShader.use();

        {
            // model
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -8.0f, 0.0f)); 
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));      
            pShader.setMat4("model", model);

            // view
            glm::mat4 view = wind::camera.GetViewMatrix();
            pShader.setMat4("view", view);

            //projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(wind::camera.GetZoomLevel()), (float)wind::SCR_WIDTH / (float)wind::SCR_HEIGHT, 0.1f, 100.0f);
            pShader.setMat4("projection", projection);
        }

        // draw
        pModel.Draw(pShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void destroy(GLFWwindow *window)
{
    if (window)
        glfwTerminate();
}

int main()
{
    auto window = init("ogl", wind::SCR_WIDTH, wind::SCR_HEIGHT);

    // flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // build and compile shaders
    const std::string path = "d:/CODE/ogl/src/gl/resources/"; // current dir

    ShaderManager ourShader((path + "shader/vertex.vs").c_str(), (path + "shader/fragment.fs").c_str());
    Model ourModel((path + "model/nanosuit/nanosuit.obj").c_str());

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render(window, ourShader, ourModel);

    destroy(window);
    return 0;
}
