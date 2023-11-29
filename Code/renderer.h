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
#include "Skeleton/skeleton.h"
#include "Muscles/muscle_system.h"
#include "Tools/stb_image.h"
#include "Tools/tools.h"

class Renderer {
  public:
    // Constructeur
    Renderer(Skeleton *skeleton, MuscleSystem *muscles);
    ~Renderer();

    // Init
    void Init(int width, int height);
    void load_shader(std::string fshader_src, std::string vshader_src,
        unsigned int& fragmentShader, unsigned int& vertexShader,  unsigned int& shaderProgram);
    void load_texture(std::string texture_path, unsigned int* textureid);

    // Inner class
    Skeleton * _skel;
    MuscleSystem *_musclesys;
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
    unsigned int texture_matcap;
    unsigned int texture_muscle_matcap;
  private:
    // Camera view variable
    float     zNear, zFar, fov;
    glm::mat4 _vpmat, _view, _projection;
    glm::vec3 _camerapos;
    float     _cameradist;

    // Shader variable
    float _theta;
    bool _mymatcap;

    // Shader ids
    unsigned int skin_vertexShader;
    unsigned int skin_fragmentShader;
    unsigned int skin_shaderProgram;
    // muscle shader
    unsigned int muscle_vertexShader;
    unsigned int muscle_fragmentShader;
    unsigned int muscle_shaderProgram;
    // ground shader
    // ...
    // TODO
    // ...

    ImVec4 clear_color;

    // Render mode
    std::string skel_mode; // choice: ["stick", "wire", "mesh]
    std::string muscle_mode; // choice: ["curve", "wire", "mesh"]
    // std::string skin_mode; // choice; ["wire", "mesh"]

    // Texture variable
    int width, height, nrChannels;

};

#endif // !RENDER_H
