#ifndef MUSCLE_CPP
#define MUSCLE_CPP

#include "muscle.h"

using namespace glm;


Muscle::Muscle(std::string name, std::string geometry_path, Bone* insertion_begin, Bone* insertion_end, 
    int n_points, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3) :
  _name(name), _geometry_path(geometry_path), 
  _curve(name+"_curve", n_points, P0, P1, P2, P3),
  _insertion_begin(insertion_begin), _insertion_end(insertion_end),
  _solver(n_points, &_curve)
{
}


void Muscle::create_geometry(int* indice_offset)
{
  _mesh.create_from_file(_geometry_path);
  Info_Print(_name + " | offset: "+std::to_string(*indice_offset));
  for (int i = 0; i < _mesh.face_indices.size(); i++){
    _mesh.face_indices[i] += *indice_offset;
  }
  _id_offset = *indice_offset;
  *indice_offset += _mesh.n_verts;
}

void Muscle::update_values(std::vector<glm::vert_arr>* values, int frame)
{
  for (int i = 0; i < _mesh.n_verts; i++) {
    values->at(i + _id_offset) = _mesh.vert_values[i];
  }
}

void Muscle::solve(int frame)
{
  Qpoint q_0, q_np1;
  q_0.pos = vec3(_insertion_begin->transformation * vec4(_curve.curve_points[0], 1.f)) - _curve.curve_points[0];
  // q_0.rot = ... ;
  q_np1.pos = vec3(_insertion_end->transformation * vec4(_curve.curve_points[_curve.n_points-1], 1.f)) - _curve.curve_points[_curve.n_points-1];
  // q_np1.rot = ... ;
  _solver.solve_iteration(q_0, q_np1);
}
void Muscle::UI_pannel()
{
  char name[_name.size()];
  strcpy(name, _name.c_str());
  ImGui::SeparatorText(name);
  if (ImGui::Button("Impulse"))
    _solver.impulse = true;

  ImGui::Text("On met ici tous les paramètres relatif au muscle selectioné !");
}

void Muscle::toogle_gravity()
{
  if (_solver.gravity) {
    _solver.gravity = false;
  } else {
    _solver.gravity = true;
  }
}

Muscle::~Muscle()
{

}

#endif // !MUSCLE_CPP
