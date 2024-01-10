//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: renderer
// Parent: app
// Date: 23/09/23
// Content: Contain the class which manage the Opengl render of the App
// //**********************************************************************************//
#ifndef GEOMETRY_H
#define GEOMETRY_H

//******** LIBRARY ******** 
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h> 

#include <iostream>
#include <fstream>
#include <sstream>

// Files
#include "Tools/tools.h"
#include "element.h"

// #include <glm/gtc/type_ptr.hpp>


// Faire une classe abstrait et 2 class fille pour les geometry bone, geometry muscle et geometry skin

class Geometry {
  public:
    Geometry(std::vector<float> *vertices, std::vector<int> *indices);
    Geometry(std::string file);
    Geometry();
    ~Geometry();
    void create_from_file(std::string file);

    void compute_normals();

    glm::vert_arr compute_value(int vert_id);

    void set_transform(glm::mat4 new_transformation) {_transformation = new_transformation;};
    void set_id_offset(int offset) {offset_id = offset;};

    // std::vector<glm::vert_arr> vert_values;
    // std::vector<int>           face_indices;
    // int n_values;
    // int n_indices;
    //
  public:
    std::string name;
    // Verteices
    std::vector<Vertex>   vertex_list;
    int n_verts;
    // Faces
    std::vector<Triangle> face_list;
    int n_faces;

    int offset_id;

  private:
    glm::mat4 _transformation;
    // use vector can use L.data() gives you access to the int[] array buffer and you can L.resize() the vector later.
    // L.get() gives you a pointer to the int[] array.
    // std::vector<float> vert_norm; 

};


#endif // !GEOMETRY_H
