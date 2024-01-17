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
#include "Tools/tools.h"
// #include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace glm {
  struct vert_arr {
    static const int size = 6;
    vec3 pos;
    vec3 normal;
  };
};

struct color_arr {
  float x, y, z;
  float r, g, b;
};

class Triangle;

class Vertex { // Plutot un struct qu'une class
  public:
    //Constructer
    Vertex(int id, glm::vec3 init_pos);
    // Vertex(int id, float x, float y, float z);
    Vertex(Vertex* vert);

    // Compute normal
    void compute_vert_normal(std::vector<Triangle> *faces);

    void set_t(float new_t) {t = new_t;};

  public: // Variable
    int id;
    glm::vec3 pos;
    glm::vec3 normal;
    float t; // le parametre de la courbe, la plus proche de la curve

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
    int get_v1_id() {return _v1->id;};
    int get_v2_id() {return _v2->id;};
    int get_v3_id() {return _v3->id;};

    glm::vec3 get_v1_pos() {return _v1->pos;};
    glm::vec3 get_v2_pos() {return _v2->pos;};
    glm::vec3 get_v3_pos() {return _v3->pos;};
    // glm::vec3 get_normal() {return _normal;};

    void get_vert_neighbor(Vertex *v1, Vertex **v2, Vertex **v3);

  public: // variable
    glm::vec3 normal;
    float distance; // needed to define a affine surface in 3D
    int n_vert = 3;
  private:
    int _id;
    Vertex *_v1, *_v2, *_v3;
};

#endif // !ELEMENT_H
