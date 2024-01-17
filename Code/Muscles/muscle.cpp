#ifndef MUSCLE_CPP
#define MUSCLE_CPP

#include "muscle.h"

using namespace glm;

Muscle::Muscle(Muscle&& muscle):
  name(std::move(muscle.name)), _curve(std::move(muscle._curve)), _solver(std::move(muscle._solver)), _mesh(std::move(muscle._mesh)), _interpo_mesh(&_curve, &_mesh, &_solver),
  _geometry_path(std::move(muscle._geometry_path)), _insertion_begin(std::move(muscle._insertion_begin)), _insertion_end(std::move(muscle._insertion_end))
{
  Info_Print("use muscle move constructor");
  _curve.update_solution(_solver.get_solution());
}

Muscle::Muscle(const Muscle& muscle):
  name(muscle.name), _curve(muscle._curve), _solver(muscle._solver), _mesh(muscle._mesh), _interpo_mesh(&_curve, &_mesh, &_solver),
  _geometry_path(muscle._geometry_path), _insertion_begin(muscle._insertion_begin), _insertion_end(muscle._insertion_end)
{
  Info_Print("use muscle copy constructor of "+name);
  Warn_Print("The solver should loose his Curve??? No because it s not a pointer so the _solver copy will recrate another curve fully, with all points");
  _curve.update_solution(_solver.get_solution());
}



Muscle::Muscle(std::string name, std::string geometry_path, Bone* insertion_begin, Bone* insertion_end, 
    int n_points, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3, Solver_param* solver_param) :
  name(name), _curve(name+"_curve", n_points, P0, P1, P2, P3),  _solver(n_points, _curve, solver_param), _mesh(name+"_mesh"), _interpo_mesh(&_curve, &_mesh, &_solver),
  _geometry_path(geometry_path), _insertion_begin(insertion_begin), _insertion_end(insertion_end)
{
  pi = 4*atan(1.f);
  _curve.update_solution(_solver.get_solution());
}


void Muscle::create_geometry(int* indice_offset)
{
  _mesh.create_from_file(_geometry_path);
  _mesh.set_id_offset(*indice_offset);
  *indice_offset += _mesh.n_verts;
}

void Muscle::create_interpolated_geometry(int* indice_offset)
{
  // Declaration des variables
  int n_i = 4; // nb_interpo point selon x//z
  int n_j = 7; // nb_interpo point selon y//theta
  int n = 15; // nb_point to render selon x//z
  int m = 15; // nb_point to render selon y//theta

  // find chebychev points
  _interpo_mesh.rebuild_interpolator(n_i, n_j);

  // Build mesh frome interpolation formula
  _interpo_mesh.create_geometry(n, m);

  // Add indices offest
  _interpo_mesh.set_id_offset(*indice_offset);
  *indice_offset += _interpo_mesh.n_verts;
}

void Muscle::solve(int frame)
{
  Qpoint q_0, q_np1;
  q_0.pos = vec3(_insertion_begin->transformation * vec4(_curve.curve_points[0], 1.f)) - _curve.curve_points[0];
  q_0.rot = vec3(0.f);
  // Tentative de rotation peu concluante: 
  // float rx, ry, rz;
  // extractEulerAngleXYZ(_insertion_begin->transformation, rx, ry, rz);
  // // if (_name=="Biceps") {
  // //   ImGui::Begin("Test");
  // //   ImGui::SliderFloat("rx", &_rx, -6.28, 6.28);
  // //   ImGui::SliderFloat("rz", &_rz, -6.28, 6.28);
  // //   ImGui::SliderFloat("ry", &_ry, -6.28, 6.28);
  // //   ImGui::End();
  // // }
  // std::cout<<rx<<" "<<ry<<" "<<rz<<std::endl;
  // q_0.rot = vec3(rx, ry, rz);
  q_np1.pos = vec3(_insertion_end->transformation * vec4(_curve.curve_points[_curve.n_verts-1], 1.f)) - _curve.curve_points[_curve.n_verts-1];
  q_np1.rot = vec3(0.f);

  // IDEM
  // extractEulerAngleXYZ(_insertion_end->transformation, rx, ry, rz);
  // q_np1.rot = vec3(rx, ry, rz);
  // Vec3_Draw(q_0.pos, q_0.rot);
  // Vec3_Draw(q_0.pos, vec3(1.f, 0.f, 0.f));
  //
  _solver.solve_iteration(q_0, q_np1);

  // send (values or pointer...) to geometry for renderer
  _curve.update_solution(_solver.get_solution());
  _interpo_mesh.update_solution(_solver.get_solution());
}

void Muscle::UI_pannel()
{
  char name_char[name.size()];
  static float new_rho=100.f; // a remplacer par un static MaterialProperty a terme
  strcpy(name_char, name.c_str());
  ImGui::SeparatorText(name_char);
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
