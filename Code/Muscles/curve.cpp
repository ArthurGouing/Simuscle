#ifndef CURVE_CPP
#define CURVE_CPP

#include "curve.h"

using namespace glm;


Curve::Curve(std::string curve_name, int n_pts, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3):
  name(curve_name), n_points(n_pts), n_elements(n_pts-1), id_offset(0), elements(n_elements)
{
  control_points.push_back(P0);
  control_points.push_back(P1);
  control_points.push_back(P2);
  control_points.push_back(P3);

  // for (int i = 0; i < control_points.size(); i++) {
  //   Vec3_Print("Controle point n°"+std::to_string(i), control_points[i]);
  // }

  // Create Curve point
  for (int i = 0; i < n_points; i++)
  {
    vec3 point;
    point = compute_curve_point(float(i)/float(n_points-1));
    curve_points.push_back(point);
  }

  // Init Elements
  for (int i = 0; i < n_elements; i++)
  {
    vec3 p1 = curve_points[i];
    vec3 p2 = curve_points[i+1];

    elements[i].set_pts(p1, p2);
  }
}

vec3 Curve::compute_curve_point(float t)
{
  vec3 point;
  point = float(pow(1-t, 3)) * control_points[0] 
        + float(3*pow(1-t, 2)*t) * control_points[1]
        + float(3*(1-t)*pow(t, 2)) * control_points[2]
        + float(pow(t, 3)) * control_points[3];
  return point;
}

void Curve::update_values(std::vector<glm::vec3>* values, Deformations* deform)
{
  for (int i = 0; i < n_points; i++) {
    values->at(i + id_offset) = curve_points[i] + deform->at(i).pos;
  }
}

Curve::~Curve()
{}

#endif // !CURVE_CPP