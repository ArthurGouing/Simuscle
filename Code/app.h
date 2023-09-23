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
#include "renderer.h"


class App {
  public:
    // Constructor
    App(Renderer* renderer);
    // Init
    void Init();
    // Run
    void Run();
    //Destructor
    ~App();

    // Inner Classs
    Renderer* _renderer;

    // App variable
    float size_window;
    float deltaTime;
    float lastFrame;
    bool show_demo_window;
    bool show_another_window;

    // window
    GLFWwindow* window;

    // Callback functions
    void window_resize_event(int width, int height);
    void mouse_cursor_event(double xpos, double ypos);
    void mouse_scroll_event(double yoffset);
    void mouse_button_event(int button, int action, int mods);

    // Sensibility movement
    float sensi_rot;
    float sensi_mov;
    float sensi_scale;

    void processInput(GLFWwindow *window);

  private:
    // mouse variable
    bool firstMouse;
    // glm::vec2 diff;
    glm::vec2 _lastmousepos;

    // Camera projection matrix
    // glm::mat4 projection;
    // glm::mat4 view;
    glm::vec3 camera_translation;
    // float cameradist;
    // glm::mat4 rotation;
    bool camera_is_moving;
    bool rot;
    bool mov;
    bool scale;
};


#endif // !APP_H
