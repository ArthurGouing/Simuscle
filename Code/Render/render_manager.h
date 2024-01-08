//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: renderer
// Parent: app
// Date: 22/12/23
// Content: Contain the class which manage the Opengl render of the App
// //**********************************************************************************//
#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

//******** LIBRARY ******** 
// Opengl
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Standard
#include <vector>

// Files
#include "imgui.h"
#include "renderer.h"
#include "Tools/tools.h"


class Renderer;

class RenderManager
{
  public: // Methode
    // Constructor
    RenderManager();

    // Initialisation
    void Init(int width, int height);

    // Add renderer
    void add_renderer(Renderer* renderer) {_renderers.push_back(renderer);};

    // Render a new frame 
    void draw_all(); // use the static _vpmap

    void resize_fbo(int width, int height);

    // update camera movement
    void update_cameradist(float dist);
    void update_rotation(glm::vec2 mouse_offset, float rotationAngle); 
    void update_camerapos(glm::vec2 mouse_offset);
    void update_projection(float aspect);

    // Get function
    glm::mat4 get_rotation() {return _rotation;};

    void UI_pannel();

    // other
    void reset_view();

    ~RenderManager();
  public: //Variable (ou protexted ??)
    // FBO texture id
    unsigned int textureColorbuffer;
  private:
    // Renderer
    std::vector<Renderer*> _renderers;

    // Background color
    ImVec4 _clear_color; // TODO: suppr
 
    // Camera projection matrices
    glm::mat4 _vpmat;
    glm::mat4 _view; 
  
    // Camera view variable
    float     _zNear, _zFar, _fov;
    glm::mat4 _projection;
    glm::vec3 _camerapos;
    float     _cameradist;

    glm::mat4 _rotation, _r0;

    // FrameBufferObject variable
    unsigned int _framebuffer;
    unsigned int _rbo;
    unsigned int _textureid, _fragmentShader;
    bool _del_fbo;
};

#endif // !RENDER_MANAGER_H
