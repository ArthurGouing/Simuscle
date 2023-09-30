//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: renderer
// Parent: app
// Date: 23/09/23
// Content: Contain the class which manage the Opengl render of the App
// //**********************************************************************************//
#ifndef ELEMENT_H
#define ELEMENT_H

//******** LIBRARY ******** 
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 

// Files
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace glm {
  struct vert_arr {
    vec3 pos;
    vec3 normal;
  };
};

class Triangle;

class Vertex {
  public:
    //Constructer
    Vertex(int id, std::vector<glm::vert_arr> *vert_values);
    // Vertex(int id, float x, float y, float z);
    Vertex(Vertex* vert);
    void compute_vert_normal(std::vector<Triangle> *faces);
    // Get
    glm::vec3 get_pos() {return glm::vec3(_pos->x, _pos->y, _pos->z);};
    // Variable
    int _id;
    glm::vec3 *_pos;
    glm::vec3 *_normal;

    void add_face_neighbor(int id_face);
    void add_vert_neighbor(int id_vert);

    // Neighbor variable
    int n_face_neighbor;
    int n_vert_neighbor;
    std::vector<int> _face_neighbor;
    std::vector<int> _vert_neighbor; // TODO: Pour l'instant inutile.
 
  private:
};

class Triangle {
  public:
    // Constructer
    Triangle(int id, Vertex* v1, Vertex* v2, Vertex* v3);
    ~Triangle();

    // Normal
    void compute_normal();
    glm::vec3 get_normal() {return _normal;};

    void get_vert_neighbor(Vertex *v1, Vertex **v2, Vertex **v3);

    // variable
    int n_vert = 3;
  private:
    int _id;
    Vertex *_v1, *_v2, *_v3;
    glm::vec3 _normal;
};

#endif // !ELEMENT_H
