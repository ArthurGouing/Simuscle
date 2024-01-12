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
class Line; // TODO
 
// class RayRenderer for ray marching render ??

//******** ABSTRACT CLASS ******** 
class Renderer // Interface class
{
  public:
    // Constructor
    Renderer(std::string name, std::string vert_path, std::string frag_path); // access to the _vp_mat

    // Initialisation
    void load_shader();   // Compile and link Shaders, create the GLprogram
    virtual void Init() = 0;

    // Render a new frame 
    void update_projection(int width, int height, float fov, float zNear, float zFar);
    virtual void draw(glm::vec3 camera_pos, glm::mat4 rotation, float camera_dist) = 0; //  --> Draw(int frame) si on veut faire du cache ?   

    // ImGui pannel
    virtual void UI_pannel();

    // Destructor
    virtual ~Renderer() = 0;

  // Variables
  protected:
    // Name
    std::string _name;
    // Shader
    std::string _vshader_path, _fshader_path;
    unsigned int _shaderProgram;

    // constant parameters for GPU
    glm::vec2 _viewport_size;
    glm::mat4 _projection;
  private:
};

//******** INHERITED CLASS :: Triangle Geometry ******** 
// Needed Struct
enum RenderMode {wire, mesh};

// Type selector
template <typename Object>
struct FormatSelector {
  using type = glm::vert_arr;
};
template <>
struct FormatSelector<Curve> {
  using type = glm::vert_arr; //TODO: ...
};
template <>
struct FormatSelector<Line> {
  using type = glm::vert_arr; // we want position and color, so technically vert_arr can work ...
};

template <typename Object> // _values dépend du type Object
class MatcapRenderer: public Renderer
{
  typedef typename FormatSelector<Object>::type ValueFormat;
  public:
    // Constructor
    MatcapRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path, std::vector<Object*> object);
    MatcapRenderer(std::string name, std::string vert_path, std::string frag_path, std::string texture_path); // Si on connait pas encore les geometries à rendre

    // Init
    void Init() override;
    void load_texture(std::string texture_path);

    // Send geometry to GPU
    void init_val_id(Object* obj);
    void init_VBO();

    void update_values();
    void update_VBO();

    // Render a new frame 
    void draw(glm::vec3 camera_pos, glm::mat4 rotation, float camera_dist) override;
    void draw_elements();

    // UI pannel
    // void UI_pannel() override;

    // Geom manager
    void add_object(Object *object) {if (object->n_verts > 0) _allgeom.push_back(object);}; 
    // void del_geom(Geometry *geom); // No idea how to do that
    //  ...
    //  ...

    ~MatcapRenderer();

  // Variables
  private:
    // Geometry
    std::vector<Object*> _allgeom;

    // Buffers values
    std::vector<ValueFormat> _values;
    std::vector<int>         _indices;
    
    // GL BUffers variables
    unsigned int _VAO, _VBO, _EBO;
    int _n_values;

    // Texture variables
    std::string _texture_path;
    unsigned int _textureid;

    // UI parameters

    // Render parameters
    RenderMode render_mode; // faire un enum plutot
};


//******** INHERITED CLASS :: Ray Marching Render (for background) ******** 

class GroundRenderer: public Renderer
{
  public:
    // Constructor
    GroundRenderer(std::string name, std::string vert_path, std::string frag_path);

    // Init
    void Init() override;
    void init_VBO();

    // Render a new frame
    void draw(glm::vec3 camera_pos, glm::mat4 rotation, float camera_dist) override;

    // UI pannel
    // void UI_pannel() ovveride;
    
    // Destructor
    ~GroundRenderer();
  private: // Parameters
    // Geometry parameters
    float _ground_size;
    // GL BUffers variables
    unsigned int _VAO, _VBO, _EBO;


};

class MarchingRenderer: public Renderer // TODO: grosse duplication de code ici, en soit on change juste l'init_VBO
                                        // Il faudrait faire une class rasterization dont MatcapRenderer serait la fille
                                        // La meilleur solution serait surement de faire 1 seulclass avec le fond et le sol
{
  public:
    //Constructor
    MarchingRenderer(std::string name, std::string vert_path, std::string frag_path);

    void Init() override;

    // Send geometry to GPU
    void init_VBO();

    // Render a new frame 
    void draw(glm::vec3 camera_pos, glm::mat4 rotation, float camera_dist) override;

    // UI pannel
    // void UI_pannel() override;

    // Destructor
    ~MarchingRenderer();

  private:
    // Shader parameters
    glm::vec3 background_color;
    float fog_factor_sky, fog_factor_ground;
    float white_color, black_color, grid_size;
    bool is_gamma_correction;

    // Buffers values
    std::vector<float> _values;

    // GL BUffers variables
    unsigned int _VAO, _VBO;
};

template class MatcapRenderer<Geometry>;
template class MatcapRenderer<Curve>;
//  template class MatcapRenderer<Line>;


#endif // !RENDER_H
