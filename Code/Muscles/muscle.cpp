#ifndef MUSCLE_CPP
#define MUSCLE_CPP

#include "muscle.h"

using namespace glm;


Muscle::Muscle(std::string name, std::string geometry_path, Bone* insertion_begin, Bone* insertion_end, 
    int n_points, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, Solver_param* solver_param) :
  _name(name), _mesh(), _curve(name+"_curve", n_points, P0, P1, P2, P3), _solver(n_points, _curve, solver_param),
  _geometry_path(geometry_path), _insertion_begin(insertion_begin), _insertion_end(insertion_end)
{}


void Muscle::create_geometry(int* indice_offset)
{
  _mesh.create_from_file(_geometry_path);
  Info_Print(_name + " | offset: "+std::to_string(*indice_offset));
  for (int i = 0 ; i < _mesh.n_verts ; i++){
    _mesh.vertex_list[i].pos += *indice_offset;
  }
  _id_offset = *indice_offset;
  *indice_offset += _mesh.n_verts;
}

void Muscle::update_values(std::vector<glm::vert_arr>* values, int frame)
{
  for (int i = 0; i < _mesh.n_verts; i++)
  {
    values->at(i + _id_offset).pos = _mesh.vertex_list[i].pos;
  }
}

void Muscle::solve(int frame)
{
  Qpoint q_0, q_np1;
  q_0.pos = vec3(_insertion_begin->transformation * vec4(_curve.curve_points[0], 1.f)) - _curve.curve_points[0];
  float rx, ry, rz;
  extractEulerAngleXYZ(_insertion_begin->transformation, rx, ry, rz);
  // if (_name=="Biceps") {
  //   ImGui::Begin("Test");
  //   ImGui::SliderFloat("rx", &_rx, -6.28, 6.28);
  //   ImGui::SliderFloat("rz", &_rz, -6.28, 6.28);
  //   ImGui::SliderFloat("ry", &_ry, -6.28, 6.28);
  //   ImGui::End();
  // }
  std::cout<<rx<<" "<<ry<<" "<<rz<<std::endl;
  q_0.rot = vec3(rx, ry, rz);
  q_np1.pos = vec3(_insertion_end->transformation * vec4(_curve.curve_points[_curve.n_points-1], 1.f)) - _curve.curve_points[_curve.n_points-1];
  extractEulerAngleXYZ(_insertion_end->transformation, rx, ry, rz);
  q_np1.rot = vec3(rx, ry, rz);
  // Vec3_Draw(q_0.pos, q_0.rot);
  // Vec3_Draw(q_0.pos, vec3(1.f, 0.f, 0.f));
  _solver.solve_iteration(q_0, q_np1);
}
void Muscle::UI_pannel()
{
  char name[_name.size()];
  static float new_rho=100.f; // a remplacer par un static MaterialProperty a terme
  strcpy(name, _name.c_str());
  ImGui::SeparatorText(name);
  if (ImGui::Button("Impulse"))
    _solver.impulse = true;

  ImGui::InputFloat("rho", &new_rho, 0.f, 10000.f);

  if (ImGui::Button("Rebuild Equations"))
  {
    for (int i = 0; i < _curve.n_elements; i++)
      _curve.get_property(i)->set_rho(new_rho);
    _solver.update_matrices();
  }
  ImGui::Text("On met ici tous les paramètres relatif au muscle selectioné !");
}

Muscle::~Muscle()
{

}

#endif // !MUSCLE_CPP
