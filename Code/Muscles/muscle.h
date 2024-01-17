//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: muscle.cpp
// Parent: muscle_system.cpp
// Date: 27/11/23
// Content: class for the muscle object
//**********************************************************************************//
#ifndef MUSCLE_H
#define MUSCLE_H

//******** LIBRARY ******** 
#include "solver.h"
#include "curve.h"
#include "Geometry/geometry.h"
#include "Skeleton/bone.h"
#include "Tools/tools.h"
#include <string>
#include <glm/gtx/euler_angles.hpp>

class Muscle
{
  public:
    // Muscle(){};
    // Constructor
    Muscle(std::string name, std::string geometry_path, Bone* insertion_begin, Bone* insertion_end,
        int n_points, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, Solver_param* solver_param);
    // Move Constructor (used in push_back)
    Muscle(Muscle&& muscle); // useless... pushback utilise la copy
    // COpy Constructor
    Muscle(const Muscle&);
    ~Muscle();

    void solve(int frame);

    void create_geometry(int* indice_offset);
    void create_interpolated_geometry(int* indice_offset);

    // void update_values_crv(std::vector<glm::vert_arr>* values, int frame);
    // void update_values(std::vector<glm::vert_arr>* values, int frame);

    // UI functions
    void UI_pannel();

  public:
    std::string name;

    Curve _curve;
    Solver _solver; // A mettre dans curve ? ca revient au meme...

    Geometry _mesh;
    GeometryInterpo _interpo_mesh;
    // MaterialProperty _property; // cf in curve // Pourtant c'est propre au muscle non ? 
                                  // Non, si on veut faire une partie muscle et une partie tendon avec des propries différents

  private:
    std::string _geometry_path;
    Bone* _insertion_begin;
    Bone* _insertion_end;

    int _id_offset;
    float pi;
};

#endif // !MUSCLE_H
