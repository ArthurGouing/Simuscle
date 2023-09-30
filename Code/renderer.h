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
#include "stb_image.h"
#include "tools.h"
#include "geometry.h"

class Renderer {
  public:
    // Constructeur
    Renderer();
    ~Renderer();

    // Init
    void Init();

    // Inner class
    Geometry * _geom;
    void add_geom(Geometry *geom);

    // Render a new frame 
    void Draw();
    unsigned int VAO, VBO, EBO;

    // update camera movement
    void update_cameradist(float dist);
    void update_rotation(glm::vec2 mouse_offset, float rotationAngle); 
    void update_camerapos(glm::vec2 mouse_offset);
    void update_projection(float aspect);

  private:
    // Camera view variable
    float     zNear, zFar, fov;
    glm::mat4 _vpmat, _view, _projection;
    glm::mat4 _rotation;
    glm::vec3 _camerapos;
    float     _cameradist;

    // Shader ids
    unsigned int vertexShader;
    unsigned int fragmendShader;
    unsigned int shaderProgram;

    ImVec4 clear_color;

    // Texture variable
    int width, height, nrChannels;
    unsigned int texture;
};

#endif // !RENDER_H
