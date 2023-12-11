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
#include "curve.h"
#include "Geometry/geometry.h"
#include "Skeleton/bone.h"
#include "Tools/tools.h"
#include <string>

class Muscle
{
  public:
    // Muscle(){};
    Muscle(std::string name, std::string geometry_path, Curve muscle_curve, Bone* insertion_begin, Bone* insertion_end);
    ~Muscle();

    void create_geometry(int* indice_offset);

    Geometry _mesh;
    std::string _name;
    Curve _curve;

  private:
    std::string _geometry_path;
    Bone* _insertion_begin;
    Bone* _insertion_end;

};

#endif // !MUSCLE_H
