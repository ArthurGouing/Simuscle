#ifndef MUSCLE_CPP
#define MUSCLE_CPP

#include "muscle.h"

Muscle::Muscle(std::string name, std::string geometry_path, Curve muscle_curve, Bone* insertion_begin, Bone* insertion_end) :
  _name(name), _geometry_path(geometry_path), _curve(muscle_curve),
  _insertion_begin(insertion_begin), _insertion_end(insertion_end)
{
}


void Muscle::create_geometry(int* indice_offset)
{
  _mesh.create_from_file(_geometry_path);
  Info_Print(_name + " | offset: "+std::to_string(*indice_offset));
  for (int i = 0; i < _mesh.face_indices.size(); i++){
    _mesh.face_indices[i] += *indice_offset;
  }
  *indice_offset += _mesh.n_verts;
}

Muscle::~Muscle()
{

}

#endif // !MUSCLE_CPP
