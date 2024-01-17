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
#include "element.h"
#include "interpolator.h"
#include "Muscles/curve.h"
#include "Muscles/solver.h"
#include "Tools/tools.h"

// #include <glm/gtc/type_ptr.hpp>



class Geometry/*(Bone)*/ {
  public:
    Geometry(std::vector<float> *vertices, std::vector<int> *indices); // Debreciated
    Geometry();
    Geometry(std::string name);
    Geometry(std::string name, std::string file);
    ~Geometry();
    void create_from_file(std::string file);

    void compute_normals();

    glm::vert_arr compute_value(int vert_id);

    // Compute the point where the mesh and the ray intersect
    glm::vec3 ray_intersection(Ray ray); // ray cast on _base_mesh 
    glm::vec3 ray_intersection_approx(Ray ray); // ray cast on _base_mesh 
                                                             // peut être mettre directemetn un Ray en entrée ??

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
};

class GeometryInterpo
{
  public: 
    // Constructor
    GeometryInterpo(Curve* base_curve,   // Curve du muscle
                    Geometry* base_mesh, // La geometry à interpoler
                    Solver* Solver
                   );
    void rebuild_interpolator(int n_z,     // le nombre de points d'interpolation selon z (aussi noté t)
                              int n_theta  // le nombre de points d'interpolation selon theta
                             );
    void create_geometry(int n,  // Le nombre de points selon z (ou t) de la geometry à rendrer
                         int m   // Le nombre de points selon theta de la geometry à rendrer
                        );
    void compute_normals();
    void compute_curve_projection();
    //Interpolater get_interpolation_class();

    // Function to compute deformations on Geometry
    void update_solution(Deformations* new_solution); 
    void set_id_offset(int offset) {offset_id = offset;};

    // return the mesh point data of the vertex id, after the deformation
    glm::vert_arr compute_value(int id);

    // Destructor
    void clear(){vertex_list.clear(); face_list.clear();};
    ~GeometryInterpo();

  public: // Variable
    std::string name;
    int n_verts;
    int n_faces;
    // Offset need to build the indices array in VAO
    int offset_id;

    // Verteices
    std::vector<Vertex>   vertex_list;
    // Faces
    std::vector<Triangle> face_list;

  private:
    // Mean Curve of the muscles
    Curve* _curve;
    // Mesh to interpole
    Geometry* _base_mesh;
    // Interpolations class
    Interpolator lagrange2D; // compute interpo points, lagrange poly, Pij
    // Solution for compute deformation of the geometry
    Deformations* _deformation;

    // constant
    float pi;
};

#endif // !GEOMETRY_H
