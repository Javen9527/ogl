// Created by jianbo
// Date: 2021.11.27
// All rights reserved

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

GLFWwindow* init(const std::string name, const unsigned int width, const unsigned int height)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    // set callbacks
    auto frameBufferCallback = [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, frameBufferCallback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    return window;
}

void destroy()
{
    glfwTerminate();
}

void render(GLFWwindow* window)
{
    // input
    auto processInput = [](GLFWwindow* window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    };

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main()
{
    // init
    auto window = init("ogl", 800, 600);

    // render loop
    if(window)
        render(window);
    
    // destroy
    destroy();

    return 0;
}


