//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: renderer
// Parent: app
// Date: 22/12/23
// Content: Contain the class which manage the Opengl render of the App
// //**********************************************************************************//
#ifndef RENDER_H
#define RENDER_H

//******** LIBRARY ******** 
// Opengl
#include <glad/glad.h>
// #include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Writing
#include <string>

// Files
#include "Geometry/geometry.h"
#include "Muscles/curve.h" // curve --> element ??
#include "imgui.h"
#include "Tools/stb_image.h"
#include "Tools/tools.h"

// class RayRenderer for ray marching render ??

//******** ABSTRACT CLASS ******** 
class Renderer // Abstract class --> Matcap Renderer
{
  public:
    // Constructor
    Renderer(std::string name, std::string vert_path, std::string frag_path); // access to the _vp_mat

    // Initialisation
    virtual void Init(); // load shader
    void load_shader();   // Compile and link Shaders, create the GLprogram
    void load_texture(std::string texture_path);   // Ou load a texture on GPU

    // VBO operation
    virtual void init_VBO() = 0;
    virtual void update_VBO() = 0;

    virtual void add_object (Geometry* geometry) = 0;

    // Render a new frame 
    virtual void draw(glm::mat4 vp_mat, glm::mat4 view) = 0; //  --> Draw(int frame) si on veut faire du cache ?   

    // Get functions
    unsigned int get_texture_id() {return _textureid;};

    // Destructor
    virtual ~Renderer();

  // Variables
  public:
    std::vector<glm::vert_arr> _values;
    std::vector<int>           _indices;
  protected:
    // Name
    std::string _name;
    // Shader
    std::string _vshader_path, _fshader_path;
    unsigned int _shaderProgram;
    // VBO for the geometry
    unsigned int _VAO, _VBO, _EBO;
    int _n_values;
    // Texture
    std::string _texture_path;
    unsigned int _textureid;
  private:
};

//******** INHERITED CLASS :: Triangle Geometry ******** 
// Needed Struct
enum RenderMode {wire, mesh, stick};

class GeomRenderer: public Renderer
{
  public:
    // Constructor
    GeomRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path, std::vector<Geometry*> geometries);
    GeomRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path); // Si on connait pas encore les geometries à rendre
                                                                                                            //
    // Init
    void Init() override;

    // Send geometry to GPU
    void init_VBO();
    void update_VBO();

    // Render a new frame 
    void draw(glm::mat4 vp_mat, glm::mat4 view);

    // UI pannel
    void UI_pannel();

    // Geom manager
    void add_object(Geometry *object) {if (object->n_verts > 0) _allgeom.push_back(object);}; 
    // void del_geom(Geometry *geom); // No idea how to do that
    //  ...
    //  ...

    ~GeomRenderer();

  private:
    // Geometry
    std::vector<Geometry*> _allgeom;

    // Texture variable
    std::string _texture_path;
    // cf abstract class

    // Render parameters
    RenderMode render_mode; // faire un enum plutot
};


//******** INHERITED CLASS :: Curve ******** 

// class CurveRenderer: public Renderer
// {
//   public:
//     // Constructor
//     CurveRenderer(RenderManager &manager, std::string vert_path, std::string frag_path, std::vector<Curve> curves);
//     CurveRenderer(RenderManager &manager, std::string vert_path, std::string frag_path); // Si on connait pas encore les geometries à rendre
// 
//     // Send geometry to GPU
//     void init_VBO(); // sinon je mets les genBuffers dans le constructpr et je vire le init_VBO
//     void update_VBO();
// 
//     // Render a new frame 
//     void Draw(int frame); // use the static _vpmap
// 
//    // Curve manager
//    void add_geom(Curve *curve) {_allcurve.push_back(curve);};
//    void del_geom(Curve *curve); // No idea how to do that
//    //  ...
//    //  ...
// 
//   ~CurveRenderer();
// 
//   private:
//     // Curve
//     std::vector<Curve*> _allcurve;
// 
//     // Shader
//     unsigned int _vertexShader, _fragmentShader;
//     unsigned int _shaderProgram;
// 
//     // VBO for the geometry
//     unsigned int _VAO, _VBO, _EBO;
//     std::vector<glm::vert_arr> _values;
//     std::vector<int>           _indices;
//     int _n_values;
// 
//     // Texture variable
//     int width, height, nrChannels;
// };


//******** INHERITED CLASS :: Curve ******** 
// ( A mettre en premier si j'arrive à la faire marcher)

// class BGroundRenderer: MatRenderer
// {
//   public:
//     // Constructor
//     BGroundRender(); // en l'appelant, je vais d'abord faire MatRenderer::MatRenderer() puis BGroundRender::BGroundRender()
// 
//     // Initialisation
//     void Init(int width, int height);
//     // already on the parent
//     // void load_shader(std::string fshader_src, std::string vshader_src,
//         unsigned int& fragmentShader, unsigned int& vertexShader,  unsigned int& shaderProgram);
// 
//     // Render a new frame 
//     virtual void Draw(int frame); // use the static _vpmap
// 
//     // Destructor
//     ~BGroundRender();
//   private:
//     float fog_distance;
//     float grid_size;
//     glm::vec4 background_color;
//     glm::vec4 ground_color;
// }


#endif // !RENDER_H
