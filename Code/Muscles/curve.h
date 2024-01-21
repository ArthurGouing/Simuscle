//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: curve.cpp
// Parent: muscle.cpp
// Date: 30/11/23
// Content: Class that contain all the function for the curve representing the middle section of the muscle 
//**********************************************************************************//
#ifndef CURVE_H
#define CURVE_H

//******** LIBRARY ******** 
#include <cmath>
#include <string>
#include <vector>
#include <limits>
#include <glm/glm.hpp>
#include "Eigen_src/Sparse"
#include "material_property.h"
#include "Geometry/element.h"
#include "Geometry/interpolator.h"
#include "Tools/tools.h"

struct Qpoint {  // Peut changer, dépend de commment on utilise pos et rot pour déformer le muscle
  Qpoint(){};
  Qpoint(glm::vec3 init_pos, glm::vec3 init_rot): pos(init_pos), rot(init_rot) {};
  Qpoint operator=(Qpoint q) {pos =q.pos; rot=q.rot; return *this;};
  Qpoint operator+(Qpoint q) {return Qpoint(pos+q.pos, rot+q.rot);};
  Qpoint operator/(float scalar) {return Qpoint(pos/scalar, rot/scalar);};
  Qpoint operator*(float scalar) {return Qpoint(scalar*pos, scalar*rot);};
  glm::vec3 pos;
  glm::vec3 rot;
};

class Deformations
{
  public:
    Deformations(){};
    Deformations(int init_size, Qpoint v0):deform(init_size, v0), size(init_size){};

    void resize(int s){deform.resize(s);};
    Qpoint at(int i){return deform.at(i);};
    Qpoint at(float t) {
      // find the corresponding i and i+1 which arround t
      if (t==1.f)
        return at(size-1);
      int i;
      float a;
      i = floor((size-1) * t);
      a = (size-1)*t-i;
      Qpoint Q_mean;
      Q_mean = (at(i+1)*a + at(i)*(1-a));
      return Q_mean;
    };
    glm::vec3 apply_rotation(glm::vec3 point, glm::vec3 rotation_center, float t)
    {
      glm::vec3 rot = at(t).rot;
      glm::mat4 transform(1.0f);
      transform = translate(transform, rotation_center);
      transform = rotate(transform, rot.x, glm::vec3(1.0f, 0.0f, 0.0f));
      transform = rotate(transform, rot.y, glm::vec3(0.0f, 1.0f, 0.0f));
      transform = rotate(transform, rot.z, glm::vec3(0.0f, 0.0f, 1.0f));
      transform = translate(transform, -rotation_center);
      glm::vec3 rotated_point = glm::vec3(transform * glm::vec4(point, 1.f));
      return rotated_point;
    };
    void update_deform(Eigen::SparseVector<float> x, Eigen::SparseVector<float> x_p){
      Qpoint p0;
      p0.pos.x = x_p.coeffRef(0); p0.pos.y = x_p.coeffRef(1); p0.pos.z = x_p.coeffRef(2);
      p0.rot.x = x_p.coeffRef(3); p0.rot.y = x_p.coeffRef(4); p0.rot.z = x_p.coeffRef(5);
      deform[0] = p0;
      for (int i = 0; i < x.size()/6; i++) {
        Qpoint p;
        p.pos.x = x.coeffRef(6*i + 0); p.pos.y = x.coeffRef(6*i + 1); p.pos.z = x.coeffRef(6*i + 2);
        p.rot.x = x.coeffRef(6*i + 3); p.rot.y = x.coeffRef(6*i + 4); p.rot.z = x.coeffRef(6*i + 5);
        deform[i+1] = p; // Note Eigen ne recommande pas d'utiliser les coeffRef, il faudra chercher une méthode plus opti
      }
      Qpoint pnp1;
      pnp1.pos.x = x_p.coeffRef(6); pnp1.pos.y = x_p.coeffRef(7); pnp1.pos.z = x_p.coeffRef(8);
      pnp1.rot.x = x_p.coeffRef(9); pnp1.rot.y = x_p.coeffRef(10); pnp1.rot.z = x_p.coeffRef(11);
      deform[deform.size()-1] = pnp1;

    };
    void print(){
      for (size_t i = 0; i < deform.size(); i++) {
        std::cout << "p"<<i<<" pos: "<<deform[i].pos.x <<" "<<deform[i].pos.y <<" "<<deform[i].pos.z<<"  |  ";
        std::cout << "rot: "<<deform[i].rot.x<<" "<<deform[i].rot.y<<" "<<deform[i].rot.z<<" "<< std::endl;
        }
    }

  private:
    std::vector<Qpoint> deform;
    int size;
};

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
  glm::vec3 cast(float t) { return origin + t*direction;};
};
// typedef std::vector<Qpoint> Deformations;

class Curve
{
  public:
    Curve(){};
    Curve(const Curve&) = default;
    Curve(std::string name, int n_point, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3);
    ~Curve();

    // Compute Bezier curve
    glm::vec3 compute_curve_point(float t);
    glm::vec3 tangent(float t);
    glm::vec3 derive_second(float t);
    Ray get_ray(float t, float theta); // return the ray orientated along er(z, theta)
    float sdf_approx(glm::vec3 p, Interpolator &r);
    float sdf_approx(Curve& c, Interpolator &r);

    // Get final position
    void update_values(std::vector<glm::vec3>* values, Deformations* deform); // depreciated
    glm::vert_arr compute_value(int id);

    // float get_L(int element_id);
    MaterialProperty* get_property(int element_id){return &elements[element_id];};;
    void set_id_offset(int new_id_offset){offset_id=new_id_offset;};

    void update_solution(Deformations* new_solution) {_solution = new_solution;};

  public:
    std::string name;
    int n_verts;
    int n_elements; // 1 element or more
    int offset_id;

    std::vector<glm::vec3> curve_points;
    Deformations* _solution;
  private:
    glm::vec3 seem_dir;

    std::vector<glm::vec3> control_points;
    std::vector<MaterialProperty> elements; // Petit Template !???
};

#endif // !CURVE_H
