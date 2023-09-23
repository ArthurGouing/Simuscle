//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: main.cpp
// Parent: None
// Date: 08/09/23
// Content: Main file which launch the app
//**********************************************************************************//
#ifndef APP_H
#define APP_H

//******** LIBRARY ******** 
// OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// IO
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Files
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "tools.h"


class App {
  public:
    // Constructor
    App();
    // Run
    void Run();
    //Destructor
    ~App();

    // Inner Classs
    // App variable
    bool firstMouse;
    float size_window;
    int frame;
    ImVec4 clear_color;
    float deltaTime;
    float lastFrame;
    bool show_demo_window;
    bool show_another_window;

    // window
    GLFWwindow* window;

    // Callback functions
    void window_resize_event(int width, int height);
    void mouse_cursor_event(double xpos, double ypos);
    void mouse_button_event(int button, int action, int mods);

    void processInput(GLFWwindow *window);

  private:
    

    // mouse variable
    glm::vec2 diff;
    glm::vec2 lastmousepos;
    glm::vec3 rotationAxis;
    float rotationAngle;
    float sensitivity;

    // Camera projection matrix
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 camerapos;
    float cameradist;
    glm::mat4 rotation;
    bool camera_is_moving;
    bool rot;
    bool mov;
    bool scale;


    unsigned int vertexShader;
    unsigned int fragmendShader;
    unsigned int shaderProgram;

    // class CallBackWrapper
    // {
    // public:
    //     CallBackWrapper();

    //     static void window_size_callback(GLFWwindow* window, int width, int height);
    //     static void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
    //     static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    // private:
    //     static App* _application = this;
    // };
};


#endif // !APP_H
