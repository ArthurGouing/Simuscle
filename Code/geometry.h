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
#include "tools.h"
#include "element.h"

// #include <glm/gtc/type_ptr.hpp>



class Geometry {
  public:

    Geometry(std::vector<float> *vertices, std::vector<int> *indices);
    Geometry(std::string file);
    ~Geometry();

    void compute_vert_normal();

    void set_Buffers();
    void send_VBO();

    unsigned int VAO, VBO, EBO;
    int n_verts;
    int n_faces;

    std::vector<glm::vert_arr> vert_values;
    std::vector<int>   face_indices;

    std::vector<Vertex>   vertex_list;
    std::vector<Triangle> face_list;
  private:
    // use vector can use L.data() gives you access to the int[] array buffer and you can L.resize() the vector later.
    // L.get() gives you a pointer to the int[] array.
    // std::vector<float> vert_norm; 

};


#endif // !GEOMETRY_H
