//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: renderer
// Parent: app
// Date: 23/09/23
// Content: Contain the class which manage the Opengl render of the App
// //**********************************************************************************//
#ifndef RENDER_H
#define RENDER_H

//******** LIBRARY ******** 
// Opengl
#include <glad/glad.h>
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Writing
#include <string>
#include <fstream>
#include <sstream>
#include <set>

// Files
#include "skeleton.h"
#include "stb_image.h"
#include "tools.h"

class Renderer {
  public:
    // Constructeur
    Renderer(Skeleton *skeleton);
    ~Renderer();

    // Init
    void Init(int width, int height);

    // Inner class
    Skeleton * _skel;
    // MuscleSystem *_musclesys;
    // Skin         *_char;

    // Render a new frame 
    void Draw();
    void UI_pannel();

    // methode
    void resize_fbo(int width, int height);

    // update camera movement
    void update_cameradist(float dist);
    void update_rotation(glm::vec2 mouse_offset, float rotationAngle); 
    void update_camerapos(glm::vec2 mouse_offset);
    void update_projection(float aspect);

    // other
    void reset_view();

    // to go private ??
    glm::mat4 _rotation;
    // Framebuffer variable
    unsigned int framebuffer;
    unsigned int textureColorbuffer;
    unsigned int rbo;
    unsigned int texture;
  private:
    // Camera view variable
    float     zNear, zFar, fov;
    glm::mat4 _vpmat, _view, _projection;
    glm::vec3 _camerapos;
    float     _cameradist;

    // Shader ids
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    ImVec4 clear_color;

    // Render mode
    std::string skel_mode; // choice: ["stick", "wire", "mesh]
    // std::string muscle_mode;
    // std::string skin_mode;

    // Texture variable
    int width, height, nrChannels;

};

#endif // !RENDER_H
