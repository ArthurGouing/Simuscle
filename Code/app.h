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
#include "Tools/tools.h"
#include "renderer.h"
#include "timeline.h"
#include "Geometry/geometry.h"


class App {
  public:
    // Constructor
    App(Renderer* renderer, Skeleton* skeleton, MuscleSystem* muscles);
    // Init
    void Init();
    // Run
    void Run();
    //Destructor
    ~App();

    // Inner Classs
    Skeleton         *_skel;
    MuscleSystem     *_musc;
    Renderer         *_renderer;
    Timeline          _timeline;
    // SimulationManager _simulator;

    // App variable 
    float size_window; // TODO: SUPPR
    bool show_demo_window;

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
    
    // UI Element
    void UI_control_pannel(ImGuiIO& io);

  private:
    // mouse variable
    bool firstMouse;
    glm::vec2 _lastmousepos;

    // Camera projection matrix
    glm::vec3 camera_translation;
    bool mouse_on_viewport;
    bool camera_is_moving;
    bool rot;
    bool mov;
    bool scale;

  int lastframe; // a mettre dans timeline plutot

  //UI parameters
  bool is_simulating;

  // Test
  float img_size;
  glm::vec2 pannel_size;
};
#endif // !APP_H
