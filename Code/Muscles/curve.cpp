#ifndef CURVE_CPP
#define CURVE_CPP

#include "curve.h"

using namespace glm;


Curve::Curve(std::string curve_name, int n_pts, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3):
  name(curve_name), n_verts(n_pts), n_elements(n_pts-1), offset_id(0), seem_dir(vec3(0.f, -1.f, 0.f)), elements(n_elements)
{
  control_points.push_back(P0);
  control_points.push_back(P1);
  control_points.push_back(P2);
  control_points.push_back(P3);

  // for (int i = 0; i < control_points.size(); i++) {
  //   Vec3_Print("Controle point n°"+std::to_string(i), control_points[i]);
  // }

  // Create Curve point
  for (int i = 0; i < n_verts; i++)
  {
    vec3 point;
    point = compute_curve_point(float(i)/float(n_verts-1));
    curve_points.push_back(point);
  }

  // Init Elements
  for (int i = 0; i < n_elements; i++)
  {
    vec3 p1 = curve_points[i];
    vec3 p2 = curve_points[i+1];

    elements[i].set_pts(p1, p2, seem_dir);
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

vec3 Curve::tangent(float t)
{
  vec3 point;
  float t2 = t*t;
  point = float(-3*t2 +6*t -3) * control_points[0]
        + float(9*t2 -12*t +3) * control_points[1]
        + float(-9*t2 +6*t) * control_points[2]
        + float(3*t*t) * control_points[3];
  return point;
}

vec3 Curve::derive_second(float t)
{
  vec3 point;
  point = float(6*(1-t)) * control_points[0]
       + float(18*t -12) * control_points[1]
       + float(-18*t +6) * control_points[2]
       + float(6*t) * control_points[3];
  return point;
}

Ray Curve::get_ray(float t, float theta)
{
  Ray r;
  r.origin = compute_curve_point(t);
  vec3 e_z = normalize(tangent(t));
  vec3 e_theta_0 = normalize(cross(e_z, seem_dir));
  r.direction = normalize(rotate(e_theta_0, theta, e_z));
  return r;
}

float Curve::sdf_approx(glm::vec3 p, Interpolator &r)
/* Compute the sdf of a point p for segments of the curve */
{
  // Find the minimum on all elements
  float d = std::numeric_limits<float>::max();
  for(auto& el : elements) {
    float d_test = el.sdf_approx(p, r);
    if (d_test<d) {
      d = d_test;
    }
  }
  return d;
}

float Curve::sdf_approx(Curve& crv, Interpolator &r)
/* Compute the sdf of a point p for segments of the curve */
{
  // Find the minimum on all elements
  float d = std::numeric_limits<float>::max();
  for(auto& el1 : elements) {
    for(auto& el2 : crv.elements) {
      float d_test = el1.sdf_approx(el2, r);
      if (d_test<d) {
        d = d_test;
      }
    }
  }
  return d;
}

// glm::vec3 Deformations::apply_rotation(glm::vec3 point, glm::vec3 rotation_center, float t) // TODO: faire la translation ici ausisi !!!!
// {
//   vec3 rot = at(t).rot;
//   mat4 transform(1.0f);
//   transform = translate(transform, -rotation_center);
//   transform = rotate(transform, rot.x, vec3(1.0f, 0.0f, 0.0f));
//   transform = rotate(transform, rot.y, vec3(0.0f, 1.0f, 0.0f));
//   transform = rotate(transform, rot.z, vec3(0.0f, 0.0f, 1.0f));
//   transform = translate(transform, rotation_center);
//   vec3 rotated_point = vec3(transform * vec4(point, 1.f));
//   return rotated_point;
// }

vert_arr Curve::compute_value(int id)
{
  vert_arr arr;
  arr.pos   = curve_points[id] + _solution->at(id).pos; 
  arr.normal = _solution->at(id).pos;
  return arr;
}

// void Curve::update_values(std::vector<glm::vec3>* values, Deformations* deform)
// {
//   for (int i = 0; i < n_verts; i++) {
//     values->at(i + id_offset) = curve_points[i] + deform->at(i).pos;
//   }
// }

Curve::~Curve()
{}

#endif // !CURVE_CPP
