//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: interpolatro.cpp
// Parent: geometryinterpo.cpp
// Date: 13/01/24
// Content: Class that compute interpolation of function
// Or surface, or analytique mesh surface. could ve template also to use different alg/methode to modelize the surface of the mesh
//**********************************************************************************//
#ifndef REPERE_H
#define REPERE_H

//******** LIBRARY ******** 
#include <cmath>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Tools/tools.h"

class Repere
{
  // Methode
  public:
    // Constructor
    Repere();
    Repere(glm::vec3 pos, glm::mat3 rot);
    Repere(glm::vec3 pos, float rot_x, float rot_y, float rot_z);

    // Init

    // Operator
    Repere operator=(Repere const& R1);
    Repere operator+(Repere const& R1);
    Repere operator-(Repere const& R1);

    // Set functions
    void set_pos(vec3 pos);
    void set_rot(mat3 rot);
    void set_rot(float rot_x, float rot_y, float rot_z);

    void compute_transform();
    // void set_transform ???

    // Point transform
    glm::vec3 local_to_world(glm::vec3 local_position); // Templater pour faire ca sur des vertex, 
                                                        // ou sur des listes de vertex,
                                                        // ou sur un mesh/geometry

    glm::vec3 world_to_local(glm::vec3 world_position); // idem


    // Destructor
    ~Repere();


  // Parameters
  private: 
    glm::vec3 _pos;
    glm::mat3 _rot;

    glm::mat4 _transformation;
    glm::mat4 _inv_transformation;

};

#endif // !REPERE_H
