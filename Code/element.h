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


class Vertex {
  public:
    //Constructer
    Vertex(int id, std::vector<glm::vec3> *vert_values);
    // Vertex(int id, float x, float y, float z);
    Vertex(Vertex* vert);
    void compute_vert_normal(int face_id, glm::vec3 face_normal);
    // Get
    glm::vec3 get_pos() {return glm::vec3(_pos->x, _pos->y, _pos->z);};
    // Variable
    int n_face_neighbor;
    int n_vert_neighbor;
    int _id;
    glm::vec3 *_pos;
  private:
    glm::vec3 _normal;
    int* _face_neighbor;
    int* _vert_neighbor; // TODO: Pour l'instant inutile.
};

class Triangle {
  public:
    // Constructer
    Triangle(int id, Vertex* v1, Vertex* v2, Vertex* v3);
    ~Triangle();

    // Get
    glm::vec3 get_normal() {return _normal;};

    // variable
    int n_vert = 3;
  private:
    int _id;
    Vertex *_v1, *_v2, *_v3;
    glm::vec3 _normal;
};

#endif // !ELEMENT_H
